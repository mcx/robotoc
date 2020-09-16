#ifndef IDOCP_CONSTRAINTS_HXX_
#define IDOCP_CONSTRAINTS_HXX_

#include <assert.h>

namespace idocp {

inline Constraints::Constraints() 
  : constraints_() {
}


inline Constraints::~Constraints() {
}


inline void Constraints::push_back(
    const std::shared_ptr<ConstraintComponentBase>& constraint) {
  constraints_.push_back(constraint);
}


inline void Constraints::clear() {
  constraints_.clear();
}


inline bool Constraints::isEmpty() const {
  return constraints_.empty();
}


inline bool Constraints::useKinematics() const {
  for (const auto constraint : constraints_) {
    if (constraint->useKinematics()) {
      return true;
    }
  }
  return false;
}


inline ConstraintsData Constraints::createConstraintsData(
    const Robot& robot) const {
  ConstraintsData data;
  for (int i=0; i<constraints_.size(); ++i) {
    data.push_back(ConstraintComponentData(constraints_[i]->dimc()));
  }
  return data;
}


inline bool Constraints::isFeasible(Robot& robot, ConstraintsData& data, 
                                    const SplitSolution& s) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    bool feasible = constraints_[i]->isFeasible(robot, data[i], s);
    if (!feasible) {
      return false;
    }
  }
  return true;
}


inline void Constraints::setSlackAndDual(Robot& robot, ConstraintsData& data, 
                                         const double dtau, 
                                         const SplitSolution& s) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->setSlackAndDual(robot, data[i], dtau, s);
  }
}


inline void Constraints::augmentDualResidual(Robot& robot, 
                                             ConstraintsData& data, 
                                             const double dtau, 
                                             KKTResidual& kkt_residual) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->augmentDualResidual(robot, data[i], dtau, kkt_residual);
  }
}


inline void Constraints::augmentDualResidual(const Robot& robot, 
                                             ConstraintsData& data, 
                                             const double dtau, 
                                             Eigen::VectorXd& lu) const {
  assert(data.size() == constraints_.size());
  assert(lu.size() == robot.dimv());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->augmentDualResidual(robot, data[i], dtau, lu);
  }
}


inline void Constraints::condenseSlackAndDual(Robot& robot,
                                              ConstraintsData& data, 
                                              const double dtau, 
                                              const SplitSolution& s,
                                              KKTMatrix& kkt_matrix, 
                                              KKTResidual& kkt_residual) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->condenseSlackAndDual(robot, data[i], dtau, s, kkt_matrix, 
                                          kkt_residual);
  }
}


inline void Constraints::condenseSlackAndDual(const Robot& robot,
                                              ConstraintsData& data, 
                                              const double dtau, 
                                              const Eigen::VectorXd& u,
                                              Eigen::MatrixXd& Quu, 
                                              Eigen::VectorXd& lu) const {
  assert(data.size() == constraints_.size());
  assert(u.size() == robot.dimv());
  assert(Quu.rows() == robot.dimv());
  assert(Quu.cols() == robot.dimv());
  assert(lu.size() == robot.dimv());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->condenseSlackAndDual(robot, data[i], dtau, u, Quu, lu);
  }
}


inline void Constraints::computeSlackAndDualDirection(
    Robot& robot, ConstraintsData& data, const double dtau, 
    const SplitDirection& d) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->computeSlackAndDualDirection(robot, data[i], dtau, d);
  }
}


inline double Constraints::maxSlackStepSize(const ConstraintsData& data) const {
  assert(data.size() == constraints_.size());
  double min_step_size = 1;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    const double step_size = constraints_[i]->maxSlackStepSize(data[i]);
    if (step_size < min_step_size) {
      min_step_size = step_size;
    }
  }
  return min_step_size;
}


inline double Constraints::maxDualStepSize(const ConstraintsData& data) const {
  assert(data.size() == constraints_.size());
  double min_step_size = 1;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    const double step_size = constraints_[i]->maxDualStepSize(data[i]);
    if (step_size < min_step_size) {
      min_step_size = step_size;
    }
  }
  return min_step_size;
}


inline void Constraints::updateSlack(ConstraintsData& data, 
                                     const double step_size) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->updateSlack(data[i], step_size);
  }
}


inline void Constraints::updateDual(ConstraintsData& data, 
                                    const double step_size) const {
  assert(data.size() == constraints_.size());
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    constraints_[i]->updateDual(data[i], step_size);
  }
}


inline double Constraints::costSlackBarrier(const ConstraintsData& data) const {
  assert(data.size() == constraints_.size());
  double cost = 0;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    cost += constraints_[i]->costSlackBarrier(data[i]);
  }
  return cost;
}


inline double Constraints::costSlackBarrier(const ConstraintsData& data, 
                                            const double step_size) const {
  assert(data.size() == constraints_.size());
  double cost = 0;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    cost += constraints_[i]->costSlackBarrier(data[i], step_size);
  }
  return cost;
}


inline double Constraints::residualL1Nrom(Robot& robot, ConstraintsData& data, 
                                          const double dtau, 
                                          const SplitSolution& s) const {
  assert(data.size() == constraints_.size());
  double l1_norm = 0;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    l1_norm += constraints_[i]->residualL1Nrom(robot, data[i], dtau, s);
  }
  return l1_norm;
}


inline double Constraints::squaredKKTErrorNorm(Robot& robot, 
                                               ConstraintsData& data, 
                                               const double dtau, 
                                               const SplitSolution& s) const {
  assert(data.size() == constraints_.size());
  double squared_norm = 0;
  for (int i=0; i<constraints_.size(); ++i) {
    assert(data[i].dimc() == constraints_[i]->dimc());
    assert(data[i].checkDimensionsOfVectors());
    squared_norm += constraints_[i]->squaredKKTErrorNorm(robot, data[i], dtau, s);
  }
  return squared_norm;
}

} // namespace idocp

#endif // IDOCP_CONSTRAINTS_HXX_