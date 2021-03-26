#ifndef IDOCP_IMPULSE_SPLIT_DIRECTION_HPP_ 
#define IDOCP_IMPULSE_SPLIT_DIRECTION_HPP_

#include "Eigen/Core"

#include "idocp/robot/robot.hpp"
#include "idocp/robot/impulse_status.hpp"


namespace idocp {

///
/// @class ImpulseSplitDirection
/// @brief Newton direction of the solution of the optimal control problem 
/// split into an impulse stage. 
///
class ImpulseSplitDirection {
public:
  ///
  /// @brief Construct a impulse split direction.
  /// @param[in] robot Robot model. 
  ///
  ImpulseSplitDirection(const Robot& robot);

  ///
  /// @brief Default constructor.  
  ///
  ImpulseSplitDirection();

  ///
  /// @brief Destructor. 
  ///
  ~ImpulseSplitDirection();

  ///
  /// @brief Default copy constructor. 
  ///
  ImpulseSplitDirection(const ImpulseSplitDirection&) = default;

  ///
  /// @brief Default copy operator. 
  ///
  ImpulseSplitDirection& operator=(const ImpulseSplitDirection&) = default;

  ///
  /// @brief Default move constructor. 
  ///
  ImpulseSplitDirection(ImpulseSplitDirection&&) noexcept = default;

  ///
  /// @brief Default move assign operator. 
  ///
  ImpulseSplitDirection& operator=(ImpulseSplitDirection&&) noexcept = default;

  ///
  /// @brief Sets impulse status, i.e., set the dimension of the impulse forces.
  /// @param[in] impulse_status Impulse status.
  ///
  void setImpulseStatus(const ImpulseStatus& impulse_status);

  ///
  /// @brief Sets impulse status, i.e., set the dimension of the impulse forces.
  /// @param[in] dimf Dimension of the impulse.
  ///
  void setImpulseStatusByDimension(const int dimf);

  ///
  /// @brief Newton direction of ImpulseSplitSolution::lmd and 
  /// SplitSolution::gmm. Size is 2 * Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dlmdgmm();

  ///
  /// @brief const verison of ImpulseSplitDirection::dlmdgmm().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dlmdgmm() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::lmd. Size is 
  /// Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dlmd();

  ///
  /// @brief const version of ImpulseSplitDirection::dlmd().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dlmd() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::gmm. Size is 
  /// Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dgmm();

  ///
  /// @brief const version of ImpulseSplitDirection::dgmm().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dgmm() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::q. Size is Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dq();

  ///
  /// @brief const version of ImpulseSplitDirection::dq().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dq() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::v. Size is Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dv();

  ///
  /// @brief const version of ImpulseSplitDirection::dv().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dv() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::q and 
  /// ImpulseSplitSolution::v. Size is 2 * Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dx();

  ///
  /// @brief const version of ImpulseSplitDirection::dx().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dx() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::dv and 
  /// ImpulseSplitSolution::f. Size is ImpulseStatus::dimf() + Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> ddvf();

  ///
  /// @brief const version of ImpulseSplitDirection::ddvf().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> ddvf() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::dv. Size is Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> ddv();

  ///
  /// @brief const version of ImpulseSplitDirection::ddv().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> ddv() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::f_stack(). 
  /// Size is ImpulseStatus::dimf().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> df();

  ///
  /// @brief const version of ImpulseSplitDirection::df().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> df() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::beta and 
  /// ImpulseSplitSolution::mu_stack(). Size is Robot::dimv() + 
  /// ImpulseStatus::dimf().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dbetamu();

  ///
  /// @brief const version of ImpulseSplitDirection::dbetamu(). 
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dbetamu() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::beta. Size 
  /// is Robot::dimv().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dbeta();

  ///
  /// @brief const version of ImpulseSplitDirection::dbeta().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dbeta() const;

  ///
  /// @brief Newton direction of ImpulseSplitSolution::mu_stack(). 
  /// Size is ImpulseSplitSolution::dimf().
  /// @return Reference to the Newton direction.
  ///
  Eigen::VectorBlock<Eigen::VectorXd> dmu();

  ///
  /// @brief const version of ImpulseSplitDirection::dmu().
  ///
  const Eigen::VectorBlock<const Eigen::VectorXd> dmu() const;

  ///
  /// @brief Set the all directions zero.
  ///
  void setZero();

  ///
  /// @brief Returns the dimension of the stack of the contact forces at the 
  /// current contact status.
  /// @return Dimension of the stack of the contact forces.
  ///
  int dimf() const;

  ///
  /// @brief Returns true if two SplitDirection have the same values and false if 
  /// not. 
  /// @param[in] other Split direction that is compared with this object.
  ///
  bool isApprox(const ImpulseSplitDirection& other) const;

  ///
  /// @brief Sets each component vector by random value based on the current 
  /// contact status. 
  ///
  void setRandom();

  ///
  /// @brief Sets each component vector by random value. Impulse status is reset.
  /// @param[in] impulse_status Impulse status.
  ///
  void setRandom(const ImpulseStatus& impulse_status);

  ///
  /// @brief Generates split direction filled randomly.
  /// @return Split direction filled randomly.
  /// @param[in] robot Robot model. Must be initialized by URDF or XML.
  ///
  static ImpulseSplitDirection Random(const Robot& robot);

  ///
  /// @brief Generates split direction filled randomly.
  /// @return Split direction filled randomly.
  /// @param[in] robot Robot model. Must be initialized by URDF or XML.
  /// @param[in] impulse_status Impulse status.
  ///
  static ImpulseSplitDirection Random(const Robot& robot, 
                                      const ImpulseStatus& impulse_status);

  ///
  /// @brief Stack of the Newton directions dlmd(), dgmm(), dq(), dv().
  ///
  Eigen::VectorXd split_direction;

private:
  Eigen::VectorXd ddvf_full_, dbetamu_full_;
  int dimv_, dimx_, dimf_;
  bool has_floating_base_;

};

} // namespace idocp 

#include "idocp/impulse/impulse_split_direction.hxx"

#endif // IDOCP_IMPULSE_SPLIT_DIRECTION_HPP_ 