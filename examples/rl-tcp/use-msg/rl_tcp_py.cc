/*
 * Copyright (c) 2023 Huazhong University of Science and Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Muyuan Shen <muyuan_shen@hust.edu.cn>
 这个 Python 模块ns3ai_rltcp_msg_py将 ns3::TcpRlEnv 和 ns3::TcpRlAct 结构体以及 Ns3AiMsgInterfaceImpl 类进行了封装，以便在 Python 中进行操作。
它定义了一个 PyEnvStruct 类来表示 ns3::TcpRlEnv 结构体，并提供了属性访问器（def_readwrite）来读写结构体的成员变量。类似地，它定义了一个 PyActStruct 类来表示 ns3::TcpRlAct 结构体。
此外，它定义了一个 Ns3AiMsgInterfaceImpl 类，该类是 ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct> 类的 Python 包装。这个类提供了方法来在 Python 中处理与 TcpRlEnv 和 TcpRlAct 相关的消息。
这个模块的目的是为了在 Python 中与 ns3 的 TcpRlEnv 和 TcpRlAct 结构体进行交互，以便进行人工智能（AI）相关的操作。
 */

#include "tcp-rl-env.h"

#include <ns3/ai-module.h>

#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(ns3ai_rltcp_msg_py, m)
{
    py::class_<ns3::TcpRlEnv>(m, "PyEnvStruct")
        .def(py::init<>())
        .def_readwrite("nodeId", &ns3::TcpRlEnv::nodeId)
        .def_readwrite("socketUid", &ns3::TcpRlEnv::socketUid)
        .def_readwrite("envType", &ns3::TcpRlEnv::envType)
        .def_readwrite("simTime_us", &ns3::TcpRlEnv::simTime_us)
        .def_readwrite("ssThresh", &ns3::TcpRlEnv::ssThresh)
        .def_readwrite("cWnd", &ns3::TcpRlEnv::cWnd)
        .def_readwrite("segmentSize", &ns3::TcpRlEnv::segmentSize)
        .def_readwrite("segmentsAcked", &ns3::TcpRlEnv::segmentsAcked)
        .def_readwrite("bytesInFlight", &ns3::TcpRlEnv::bytesInFlight);

    py::class_<ns3::TcpRlAct>(m, "PyActStruct")
        .def(py::init<>())
        .def_readwrite("new_ssThresh", &ns3::TcpRlAct::new_ssThresh)
        .def_readwrite("new_cWnd", &ns3::TcpRlAct::new_cWnd);

    py::class_<ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>>(m, "Ns3AiMsgInterfaceImpl")
        .def(py::init<bool,
                      bool,
                      bool,
                      uint32_t,
                      const char*,
                      const char*,
                      const char*,
                      const char*>())
        .def("PyRecvBegin", &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::PyRecvBegin)
        .def("PyRecvEnd", &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::PyRecvEnd)
        .def("PySendBegin", &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::PySendBegin)
        .def("PySendEnd", &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::PySendEnd)
        .def("PyGetFinished",
             &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::PyGetFinished)
        .def("GetCpp2PyStruct",
             &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::GetCpp2PyStruct,
             py::return_value_policy::reference)
        .def("GetPy2CppStruct",
             &ns3::Ns3AiMsgInterfaceImpl<ns3::TcpRlEnv, ns3::TcpRlAct>::GetPy2CppStruct,
             py::return_value_policy::reference);
}
