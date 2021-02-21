#ifndef IDOCP_CONTACT_SEQUENCE_HXX_
#define IDOCP_CONTACT_SEQUENCE_HXX_ 

#include "idocp/hybrid/contact_sequence.hpp"

#include <stdexcept>
#include <cassert>
#include <algorithm>

namespace idocp {

inline ContactSequence::ContactSequence(const Robot& robot, 
                                        const int max_num_events)
  : max_num_events_(max_num_events),
    default_contact_status_(robot.createContactStatus()),
    contact_statuses_(max_num_events+1),
    impulse_events_(max_num_events),
    event_index_impulse_(max_num_events), 
    event_index_lift_(max_num_events),
    event_time_(max_num_events),
    impulse_time_(max_num_events),
    lift_time_(max_num_events),
    is_impulse_event_(max_num_events) {
  try {
    if (max_num_events <= 0) {
      throw std::out_of_range("invalid argument: max_num_events must be positive!");
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  clear_all();
  contact_statuses_.push_back(default_contact_status_);
}


inline ContactSequence::ContactSequence()
  : max_num_events_(0),
    default_contact_status_(),
    contact_statuses_(),
    impulse_events_(),
    event_index_impulse_(), 
    event_index_lift_(),
    event_time_(),
    impulse_time_(),
    lift_time_(),
    is_impulse_event_() {
}


inline ContactSequence::~ContactSequence() {
}


inline void ContactSequence::setContactStatusUniformly(
    const ContactStatus& contact_status) {
  clear_all();
  contact_statuses_.push_back(contact_status);
}


inline void ContactSequence::push_back(const DiscreteEvent& discrete_event, 
                                       const double event_time) {
  try {
    if (numContactPhases() == 0) {
      throw std::runtime_error(
          "Call setContactStatusUniformly() before calling push_back()!");
    }
    if (!discrete_event.existDiscreteEvent()) {
      throw std::runtime_error(
          "discrete_event.existDiscreteEvent() must be true!");
    }
    if (discrete_event.preContactStatus() != contact_statuses_.back()) {
      throw std::runtime_error(
          "discrete_event.preContactStatus() is not consistent with the last contact status!");
    }
    if (numDiscreteEvents()+1 > max_num_events_) {
      throw std::runtime_error(
          "Number of discrete events=" + std::to_string(numDiscreteEvents()+1)  
          + " exceeds predefined max_num_events=" 
          + std::to_string(max_num_events_) + "!");
    }
    if (numImpulseEvents() > 0 || numLiftEvents() > 0) {
      if (event_time <= event_time_.back()) {
        throw std::runtime_error(
            "event_time=" + std::to_string(event_time) 
            + " must be larger than the last event time=" 
            + std::to_string(event_time_.back()) + "!");
      }
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  contact_statuses_.push_back(discrete_event.postContactStatus());
  event_time_.push_back(event_time);
  if (discrete_event.existImpulse()) {
    impulse_events_.push_back(discrete_event);
    event_index_impulse_.push_back(contact_statuses_.size()-2);
    impulse_time_.push_back(event_time);
    is_impulse_event_.push_back(true);
  }
  else {
    event_index_lift_.push_back(contact_statuses_.size()-2);
    lift_time_.push_back(event_time);
    is_impulse_event_.push_back(false);
  }
}


inline void ContactSequence::push_back(const ContactStatus& contact_status, 
                                       const double event_time) {
  DiscreteEvent discrete_event(contact_statuses_.back(), contact_status);
  push_back(discrete_event, event_time);
}


inline void ContactSequence::pop_back() {
  if (numDiscreteEvents() > 0) {
    if (is_impulse_event_.back()) {
      impulse_events_.pop_back();
      event_index_impulse_.pop_back();
      impulse_time_.pop_back();
    }
    else {
      event_index_lift_.pop_back();
      lift_time_.pop_back();
    }
    event_time_.pop_back();
    is_impulse_event_.pop_back();
    contact_statuses_.pop_back();
  }
  else if (numContactPhases() > 0) {
    assert(numContactPhases() == 1);
    contact_statuses_.pop_back();
    contact_statuses_.push_back(default_contact_status_);
  }
}


inline void ContactSequence::pop_front() {
  if (numDiscreteEvents() > 0) {
    if (is_impulse_event_.front()) {
      impulse_events_.pop_front();
      event_index_impulse_.pop_front();
      impulse_time_.pop_front();
    }
    else {
      event_index_lift_.pop_front();
      lift_time_.pop_front();
    }
    event_time_.pop_front();
    is_impulse_event_.pop_front();
    contact_statuses_.pop_front();
  }
  else if (numContactPhases() > 0) {
    assert(numContactPhases() == 1);
    contact_statuses_.pop_front();
    contact_statuses_.push_back(default_contact_status_);
  }
}


inline void ContactSequence::updateImpulseTime(const int impulse_index, 
                                               const double impulse_time) {
  try {
    if (numImpulseEvents() <= 0) {
      throw std::runtime_error(
          "numImpulseEvents() must be positive when calling this method!");
    }
    if (impulse_index < 0) {
      throw std::runtime_error("impulse_index must be non-negative!");
    }
    if (impulse_index >= numImpulseEvents()) {
      throw std::runtime_error(
          "impulse_index=" + std::to_string(impulse_index) 
          + " must be less than numImpulseEvents()=" 
          + std::to_string(numImpulseEvents()) + "!");
    }
    const int event_index = event_index_impulse_[impulse_index];
    if (event_index > 0) {
      if (event_time_[event_index-1] >= impulse_time) {
        throw std::runtime_error(
            "impulse_time=" + std::to_string(impulse_time) 
            + " must be larger than event_time_[event_index-1]=" 
            + std::to_string(event_time_[event_index-1]) + "!");
      }
    }
    else if (event_index+1 < numDiscreteEvents()) {
      if (event_time_[event_index+1] <= impulse_time) {
        throw std::runtime_error(
            "impulse_time=" + std::to_string(impulse_time) 
            + " must be larger than event_time_[event_index+1]=" 
            + std::to_string(event_time_[event_index+1]) + "!");
      }
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  impulse_time_[impulse_index] = impulse_time;
  event_time_[event_index_impulse_[impulse_index]] = impulse_time;
}


inline void ContactSequence::updateLiftTime(const int lift_index, 
                                            const double lift_time) {
  try {
    if (numLiftEvents() <= 0) {
      throw std::runtime_error(
          "numLiftEvents() must be positive when calling this method!");
    }
    if (lift_index < 0) {
      throw std::runtime_error("lift_index must be non-negative!");
    }
    if (lift_index >= numLiftEvents()) {
      throw std::runtime_error(
          "lift_index=" + std::to_string(lift_index) 
          + " must be less than numLiftEvents()=" 
          + std::to_string(numLiftEvents()) + "!");
    }
    const int event_index = event_index_lift_[lift_index];
    if (event_index > 0) {
      if (event_time_[event_index-1] >= lift_time) {
        throw std::runtime_error(
            "lift_time=" + std::to_string(lift_time) 
            + " must be larger than event_time_[event_index-1]=" 
            + std::to_string(event_time_[event_index-1]) + "!");
      }
    }
    else if (event_index+1 < numDiscreteEvents()) {
      if (event_time_[event_index+1] <= lift_time) {
        throw std::runtime_error(
            "lift_time=" + std::to_string(lift_time) 
            + " must be larger than event_time_[event_index+1]=" 
            + std::to_string(event_time_[event_index+1]) + "!");
      }
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  lift_time_[lift_index] = lift_time;
  event_time_[event_index_lift_[lift_index]] = lift_time;
}


inline void ContactSequence::setContactPoints(
    const int contact_phase, 
    const std::vector<Eigen::Vector3d>& contact_points) {
  try {
    if (contact_phase >= numContactPhases()) {
      throw std::runtime_error(
          "contact_phase=" + std::to_string(contact_phase) 
          + " must be smaller than numContactPhases()" 
          + std::to_string(numContactPhases()) + "!");
    }
  }
  catch(const std::exception& e) {
    std::cerr << e.what() << '\n';
    std::exit(EXIT_FAILURE);
  }
  contact_statuses_[contact_phase].setContactPoints(contact_points);
  if (contact_phase > 0 && is_impulse_event_[contact_phase-1]) {
    impulse_events_[contact_phase-1].setContactPoints(contact_points);
  }
}


inline int ContactSequence::numImpulseEvents() const {
  return impulse_events_.size();
}


inline int ContactSequence::numLiftEvents() const {
  return (numDiscreteEvents()-numImpulseEvents());
}


inline int ContactSequence::numDiscreteEvents() const {
  return (numContactPhases()-1);
}


inline int ContactSequence::numContactPhases() const {
  return contact_statuses_.size();
}


inline const ContactStatus& ContactSequence::contactStatus(
    const int contact_phase) const {
  assert(contact_phase >= 0);
  assert(contact_phase < numContactPhases());
  return contact_statuses_[contact_phase];
}


inline const ImpulseStatus& ContactSequence::impulseStatus(
    const int impulse_index) const {
  assert(impulse_index >= 0);
  assert(impulse_index < numImpulseEvents());
  return impulse_events_[impulse_index].impulseStatus();
}


inline double ContactSequence::impulseTime(const int impulse_index) const {
  assert(impulse_index >= 0);
  assert(impulse_index < numImpulseEvents());
  return impulse_time_[impulse_index];
}


inline double ContactSequence::liftTime(const int lift_index) const {
  assert(lift_index >= 0);
  assert(lift_index < numLiftEvents());
  return lift_time_[lift_index];
}


inline void ContactSequence::clear_all() {
  contact_statuses_.clear();
  impulse_events_.clear();
  event_index_impulse_.clear(); 
  event_index_lift_.clear();
  event_time_.clear();
  impulse_time_.clear();
  lift_time_.clear();
  is_impulse_event_.clear();
}

} // namespace idocp 

#endif // IDOCP_CONTACT_SEQUENCE_HXX_ 