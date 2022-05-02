#ifndef ROBOTOC_FLYING_TROTTING_FOOT_STEP_PLANNER_HPP_
#define ROBOTOC_FLYING_TROTTING_FOOT_STEP_PLANNER_HPP_

#include <vector>
#include <iostream>

#include "Eigen/Core"
#include "Eigen/Geometry"

#include "robotoc/robot/robot.hpp"
#include "robotoc/robot/contact_status.hpp"
#include "robotoc/robot/se3.hpp"
#include "robotoc/utils/aligned_vector.hpp"
#include "robotoc/mpc/foot_step_planner_base.hpp"


namespace robotoc {

///
/// @class FlyingTrottingFootStepPlanner
/// @brief Foot step planner for the flying trotting gait of quadrupeds. 
///
class FlyingTrottingFootStepPlanner final : public FootStepPlannerBase {
public:
  ///
  /// @brief Constructs the planner.
  /// @param[in] quadruped_robot Quadruped robot model. 
  ///
  FlyingTrottingFootStepPlanner(const Robot& quadruped_robot);

  ///
  /// @brief Default constructor. 
  ///
  FlyingTrottingFootStepPlanner();

  ///
  /// @brief Destructor. 
  ///
  ~FlyingTrottingFootStepPlanner();

  ///
  /// @brief Default copy constructor. 
  ///
  FlyingTrottingFootStepPlanner(const FlyingTrottingFootStepPlanner&) = default;

  ///
  /// @brief Default copy assign operator. 
  ///
  FlyingTrottingFootStepPlanner& operator=(const FlyingTrottingFootStepPlanner&) = default;

  ///
  /// @brief Default move constructor. 
  ///
  FlyingTrottingFootStepPlanner(FlyingTrottingFootStepPlanner&&) noexcept = default;

  ///
  /// @brief Default move assign operator. 
  ///
  FlyingTrottingFootStepPlanner& operator=(FlyingTrottingFootStepPlanner&&) noexcept = default;

  ///
  /// @brief Sets the gait pattern. 
  /// @param[in] step_length Step length of the gait. 
  /// @param[in] yaw_rate Yaw-rate of the gait. 
  ///
  void setGaitPattern(const Eigen::Vector3d& step_length, const double yaw_rate);

  void init(const Eigen::VectorXd& q) override;

  bool plan(const Eigen::VectorXd& q, const Eigen::VectorXd& v, 
            const ContactStatus& contact_status, 
            const int planning_steps) override;

  ///
  /// @brief This is invalid in FlyingTrottingFootStepPlanner. 
  ///
  const aligned_vector<SE3>& contactPlacement(const int step) const override;

  ///
  /// @brief This is invalid in CrawlingFootStepPlanner. 
  ///
  const aligned_vector<aligned_vector<SE3>>& contactPlacement() const override;

  const std::vector<Eigen::Vector3d>& contactPosition(const int step) const override;

  const std::vector<std::vector<Eigen::Vector3d>>& contactPosition() const override;

  const Eigen::Vector3d& com(const int step) const override;

  const std::vector<Eigen::Vector3d>& com() const override;

  const Eigen::Matrix3d& R(const int step) const override;

  const std::vector<Eigen::Matrix3d>& R() const override;

  void disp(std::ostream& os) const;

  friend std::ostream& operator<<(std::ostream& os, 
                                  const FlyingTrottingFootStepPlanner& planner);

  friend std::ostream& operator<<(std::ostream& os, 
                                  const std::shared_ptr<FlyingTrottingFootStepPlanner>& planner);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  Robot robot_;
  int LF_foot_id_, LH_foot_id_, RF_foot_id_, RH_foot_id_, current_step_;
  aligned_vector<aligned_vector<SE3>> contact_placement_ref_;
  std::vector<std::vector<Eigen::Vector3d>> contact_position_ref_;
  std::vector<Eigen::Vector3d> com_ref_, com_to_contact_position_local_;
  std::vector<Eigen::Matrix3d> R_;
  Eigen::Vector3d step_length_;
  Eigen::Matrix3d R_yaw_;

};

} // namespace robotoc 

#endif // ROBOTOC_FLYING_TROTTING_FOOT_STEP_PLANNER_HPP_ 