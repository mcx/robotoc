#include "robotoc/mpc/mpc_jumping.hpp"

#include <stdexcept>
#include <iostream>
#include <cassert>
#include <cmath>
#include <algorithm>


namespace robotoc {

MPCJumping::MPCJumping(const OCP& ocp, const int nthreads)
  : robot_(ocp.robot()),
    contact_sequence_(std::make_shared<robotoc::ContactSequence>(
        ocp.robot(), ocp.maxNumEachDiscreteEvents())),
    sto_constraints_(ocp.sto_constraints()),
    ocp_solver_(ocp, contact_sequence_, SolverOptions::defaultOptions(), nthreads), 
    solver_options_(SolverOptions::defaultOptions()),
    cs_ground_(ocp.robot().createContactStatus()),
    cs_flying_(ocp.robot().createContactStatus()),
    contact_placements_(),
    contact_placements_local_(),
    contact_placements_goal_(),
    contact_placements_store_(),
    R_jump_yaw_(Eigen::Matrix3d::Identity()),
    s_(),
    jump_length_(Eigen::Vector3d::Zero()),
    jump_yaw_(0),
    flying_time_(0),
    min_flying_time_(0),
    ground_time_(0),
    min_ground_time_(0),
    T_(ocp.T()),
    dt_(ocp.T()/ocp.N()),
    dtm_(1.5*(ocp.T()/ocp.N())),
    t_mpc_start_(0),
    eps_(std::sqrt(std::numeric_limits<double>::epsilon())),
    N_(ocp.N()),
    current_step_(0) {
  for (int i=0; i<cs_ground_.maxNumContacts(); ++i) {
    cs_ground_.activateContact(i);
  }
}


MPCJumping::MPCJumping() {
}


MPCJumping::~MPCJumping() {
}


void MPCJumping::setJumpPattern(const Eigen::Vector3d& jump_length, 
                                const double jump_yaw, 
                                const double flying_time, 
                                const double min_flying_time, 
                                const double ground_time, 
                                const double min_ground_time) {
  try {
    if (flying_time <= 0) {
      throw std::out_of_range("invalid value: flying_time must be positive!");
    }
    if (min_flying_time <= 0) {
      throw std::out_of_range("invalid value: min_flying_time must be positive!");
    }
    if (ground_time <= 0) {
      throw std::out_of_range("invalid value: ground_time must be positive!");
    }
    if (min_ground_time <= 0) {
      throw std::out_of_range("invalid value: min_ground_time must be positive!");
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  jump_length_ = jump_length;
  jump_yaw_ = jump_yaw;
  R_jump_yaw_ << std::cos(jump_yaw), -std::sin(jump_yaw), 0, 
                 std::sin(jump_yaw), std::cos(jump_yaw),  0,
                 0, 0, 1;
  flying_time_ = flying_time;
  min_flying_time_ = min_flying_time;
  ground_time_ = ground_time;
  min_ground_time_ = min_ground_time;
}


void MPCJumping::init(const double t, const Eigen::VectorXd& q, 
                      const Eigen::VectorXd& v, 
                      const SolverOptions& solver_options, const bool sto) {
  current_step_ = 0;
  contact_sequence_->initContactSequence(cs_ground_);
  const double t_lift_off   = t + T_ - ground_time_ - flying_time_;
  const double t_touch_down = t + T_ - ground_time_;
  contact_sequence_->push_back(cs_flying_, t_lift_off, sto);
  contact_sequence_->push_back(cs_ground_, t_touch_down, sto);
  resetMinimumDwellTimes(t, dtm_);
  resetGoalContactPlacements(q);
  resetContactPlacements(q);
  ocp_solver_.setSolution("q", q);
  ocp_solver_.setSolution("v", v);
  ocp_solver_.setSolverOptions(solver_options);
  ocp_solver_.solve(t, q, v, true);
  s_ = ocp_solver_.getSolution();
  const auto ts = contact_sequence_->eventTimes();
  ground_time_ = t + T_ - ts[1];
  flying_time_ = t + T_ - ts[0] - ground_time_;
  t_mpc_start_ = t;
}


void MPCJumping::reset(const double t, const Eigen::VectorXd& q, 
                       const Eigen::VectorXd& v, 
                       const SolverOptions& solver_options, const bool sto) {
  current_step_ = 0;
  contact_sequence_->initContactSequence(cs_ground_);
  const double t_lift_off   = t + T_ - ground_time_ - flying_time_;
  const double t_touch_down = t + T_ - ground_time_;
  contact_sequence_->push_back(cs_flying_, t_lift_off, sto);
  contact_sequence_->push_back(cs_ground_, t_touch_down, sto);
  resetMinimumDwellTimes(t, dtm_);
  resetGoalContactPlacements(q);
  resetContactPlacements(q);
  for (auto& e : s_.data)    { e.q.head<3>().noalias() += R_jump_yaw_ * jump_length_; }
  for (auto& e : s_.impulse) { e.q.head<3>().noalias() += R_jump_yaw_ * jump_length_; }
  for (auto& e : s_.aux)     { e.q.head<3>().noalias() += R_jump_yaw_ * jump_length_; }
  for (auto& e : s_.lift)    { e.q.head<3>().noalias() += R_jump_yaw_ * jump_length_; }
  ocp_solver_.setSolution(s_);
  ocp_solver_.setSolverOptions(solver_options);
  ocp_solver_.meshRefinement(t);
  ocp_solver_.solve(t, q, v, true);
  s_ = ocp_solver_.getSolution();
  const auto ts = contact_sequence_->eventTimes();
  ground_time_ = t + T_ - ts[1];
  flying_time_ = t + T_ - ts[0] - ground_time_;
  t_mpc_start_ = t;
}


void MPCJumping::setSolverOptions(const SolverOptions& solver_options) {
  ocp_solver_.setSolverOptions(solver_options);
}


void MPCJumping::updateSolution(const double t, const double dt,
                                const Eigen::VectorXd& q, 
                                const Eigen::VectorXd& v) {
  assert(dt > 0);
  const auto ts = contact_sequence_->eventTimes();
  bool remove_step = false;
  if (!ts.empty()) {
    if (ts.front()+eps_ < t+dt) {
      ocp_solver_.extrapolateSolutionInitialPhase(t);
      contact_sequence_->pop_front();
      remove_step = true;
      ++current_step_;
    }
  }
  resetMinimumDwellTimes(t, dt);
  resetContactPlacements(q);
  ocp_solver_.solve(t, q, v, true);
}


const Eigen::VectorXd& MPCJumping::getInitialControlInput() const {
  return ocp_solver_.getSolution(0).u;
}


double MPCJumping::KKTError(const double t, const Eigen::VectorXd& q, 
                            const Eigen::VectorXd& v) {
  return ocp_solver_.KKTError(t, q, v);
}


double MPCJumping::KKTError() const {
  return ocp_solver_.KKTError();
}


void MPCJumping::resetMinimumDwellTimes(const double t, const double min_dt) {
  const int num_switches = contact_sequence_->numDiscreteEvents();
  if (num_switches > 0) {
    std::vector<double> minimum_dwell_times;
    switch (current_step_) {
      case 0:
        minimum_dwell_times.push_back(min_dt);
        minimum_dwell_times.push_back(min_flying_time_);
        break;
      case 1:
        minimum_dwell_times.push_back(min_dt);
        break;
      default:
        // if current_step_ >= 2, num_switches == 0.
        break;
    }
    minimum_dwell_times.push_back(min_ground_time_+(t-t_mpc_start_));
    sto_constraints_->setMinimumDwellTimes(minimum_dwell_times);
  }
}


void MPCJumping::resetGoalContactPlacements(const Eigen::VectorXd& q) {
  robot_.updateFrameKinematics(q);
  const Eigen::Quaterniond quat_init = Eigen::Quaterniond(q.coeff(6), q.coeff(3), q.coeff(4), q.coeff(5));
  const Eigen::Matrix3d R_init = quat_init.toRotationMatrix();
  contact_placements_local_.clear();
  for (const auto frame : robot_.contactFrames()) {
    contact_placements_local_.emplace_back(
        R_init.transpose() * robot_.frameRotation(frame),
        R_init.transpose() * (robot_.framePosition(frame) - q.template head<3>()));
  }
  const Eigen::Matrix3d R_goal = R_jump_yaw_ * R_init;
  const Eigen::Quaterniond quat_goal = Eigen::Quaterniond(R_goal);
  Eigen::VectorXd q_goal = q;
  q_goal.template head<3>().noalias() += R_jump_yaw_ * jump_length_;
  q_goal.template segment<4>(3) = quat_goal.coeffs();
  robot_.updateFrameKinematics(q_goal);
  contact_placements_goal_.clear();
  for (int i=0; i<robot_.contactFrames().size(); ++i) {
    contact_placements_goal_.emplace_back(
        R_goal * contact_placements_local_[i].rotation(),
        q.template head<3>() + R_goal * contact_placements_local_[i].translation()
                             + R_jump_yaw_ * jump_length_);
  }
}


void MPCJumping::resetContactPlacements(const Eigen::VectorXd& q) {
  robot_.updateFrameKinematics(q);
  contact_placements_.clear();
  // update contact points only if the current step is standing
  if (current_step_ == 0 || current_step_ == 2) {
    contact_placements_store_.clear();
    for (const auto frame : robot_.contactFrames()) {
      contact_placements_.push_back(robot_.framePlacement(frame));
      contact_placements_store_.push_back(robot_.framePlacement(frame));
    }
  }
  else {
    for (const auto& e : contact_placements_store_) {
      contact_placements_.push_back(e);
    }
  }
  if (current_step_ == 0) {
    contact_sequence_->setContactPlacements(0, contact_placements_);
    contact_sequence_->setContactPlacements(1, contact_placements_goal_);
    contact_sequence_->setContactPlacements(2, contact_placements_goal_);
  }
  else if (current_step_ == 1) {
    contact_sequence_->setContactPlacements(0, contact_placements_goal_);
    contact_sequence_->setContactPlacements(1, contact_placements_goal_);
  }
  else {
    contact_sequence_->setContactPlacements(0, contact_placements_);
  }
}

} // namespace robotoc 