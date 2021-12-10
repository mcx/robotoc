#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/numpy.h>

#include "robotoc/cost/time_varying_task_space_6d_cost.hpp"
#include "robotoc/robot/se3.hpp"


namespace robotoc {
namespace python {

class PyTimeVaryingTaskSpace6DRefBase : public TimeVaryingTaskSpace6DRefBase {
public:
  // Inherit the constructors
  using TimeVaryingTaskSpace6DRefBase::TimeVaryingTaskSpace6DRefBase;

  void update_x6d_ref(const double t, SE3& x6d_ref) const override {
    PYBIND11_OVERRIDE_PURE(void, TimeVaryingTaskSpace6DRefBase, 
                           update_x6d_ref, 
                           t, x6d_ref);
  }

  bool isActive(const double t) const override {
    PYBIND11_OVERRIDE_PURE(bool, TimeVaryingTaskSpace6DRefBase, 
                           isActive, t);
  }
};


namespace py = pybind11;

PYBIND11_MODULE(time_varying_task_space_6d_ref_base, m) {
  py::class_<TimeVaryingTaskSpace6DRefBase, 
             PyTimeVaryingTaskSpace6DRefBase,
             std::shared_ptr<TimeVaryingTaskSpace6DRefBase>>(m, "TimeVaryingTaskSpace6DRefBase")
    .def(py::init<>())
    .def("update_x6d_ref", &TimeVaryingTaskSpace6DRefBase::update_x6d_ref,
          py::arg("t"), py::arg("x6d_ref"))
    .def("isActive", &TimeVaryingTaskSpace6DRefBase::isActive,
          py::arg("t"));
}

} // namespace python
} // namespace robotoc