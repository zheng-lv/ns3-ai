#include "multi-bss.h"

#include <ns3/ai-module.h>

#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

PYBIND11_MODULE(ns3ai_multibss_stru_py, m) {

    py::class_<std::array<double, 5>>(m, "RxPowerArray")
        .def(py::init<>())
        .def("size", &std::array<double, 5>::size)
        .def("__len__", [](const std::array<double, 5> &arr) {
            return arr.size();
        })
        .def("__getitem__", [](const std::array<double, 5> &arr, uint32_t i){
            if (i >= arr.size()) {
                std::cerr << "Invalid index " << i << " for std::array, whose size is " << arr.size() << std::endl;
                exit(1);
            }
            return arr.at(i);
        })
        ;

    py::class_<Env>(m, "PyEnvStruct")
        .def(py::init<>())
        .def_readwrite("txNode", &Env::txNode)
        .def_readwrite("rxPower", &Env::rxPower)
        .def_readwrite("mcs", &Env::mcs)
        .def_readwrite("holDelay", &Env::holDelay)
        .def_readwrite("throughput", &Env::throughput)
        ;

    py::class_<Act>(m, "PyActStruct")
        .def(py::init<>())
        .def_readwrite("newCcaSensitivity", &Act::newCcaSensitivity)
        .def_readwrite("cpu_cycle_after", &Act::cpu_cycle_after)
        ;

    py::class_<ns3::Ns3AiMsgInterfaceImpl<Env, Act>>(m, "Ns3AiMsgInterfaceImpl")
        .def(py::init<bool, bool, bool, uint32_t, const char*, const char*, const char*, const char*>())
        .def("PyRecvBegin",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::PyRecvBegin)
        .def("PyRecvEnd",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::PyRecvEnd)
        .def("PySendBegin",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::PySendBegin)
        .def("PySendEnd",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::PySendEnd)
        .def("PyGetFinished",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::PyGetFinished)
        .def("GetCpp2PyStruct",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::GetCpp2PyStruct,
             py::return_value_policy::reference)
        .def("GetPy2CppStruct",
             &ns3::Ns3AiMsgInterfaceImpl<Env, Act>::GetPy2CppStruct,
             py::return_value_policy::reference)
        ;

}


