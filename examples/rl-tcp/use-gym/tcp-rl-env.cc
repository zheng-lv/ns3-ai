/*
 * Copyright (c) 2018 Technische Universität Berlin
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
 * Author: Piotr Gawlowicz <gawlowicz@tkn.tu-berlin.de>
 * Modify: Muyuan Shen <muyuan_shen@hust.edu.cn>
 */

#include "tcp-rl-env.h"

#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/simulator.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-socket-base.h"

#include <numeric>
#include <vector>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("tcp-rl-env-gym");

NS_OBJECT_ENSURE_REGISTERED(TcpEnvBase);

TcpEnvBase::TcpEnvBase()
//在构造函数 TcpEnvBase::TcpEnvBase() 中，使用了 NS_LOG_FUNCTION 宏来记录函数的调用信息，并调用了 SetOpenGymInterface 函数来设置一个 OpenGymInterface 对象。
{
    NS_LOG_FUNCTION(this);
    SetOpenGymInterface(OpenGymInterface::Get());
}

TcpEnvBase::~TcpEnvBase()
//在析构函数 TcpEnvBase::~TcpEnvBase() 中，同样使用了 NS_LOG_FUNCTION 宏来记录函数的调用信息。
{
    NS_LOG_FUNCTION(this);
}

TypeId
TcpEnvBase::GetTypeId()
//这个函数的作用是为 TcpEnvBase 类提供唯一的类型标识符，以便在其他地方使用该类时能够唯一地识别它。类型标识符通常用于动态类型识别、反射和序列化等操作。
{
    static TypeId tid = TypeId("ns3::TcpEnvBase").SetParent<OpenGymEnv>().SetGroupName("Ns3Ai");

    return tid;
}

void
TcpEnvBase::DoDispose()
//在这个函数中，使用了 NS_LOG_FUNCTION 宏来记录函数的调用信息，并传入了当前对象的指针 this。
//虚函数 DoDispose 的存在使得 TcpEnvBase 类可以被继承，并在子类中重写该函数以实现特定的资源释放和清理操作。通过使用虚函数，子类可以根据自己的需求进行扩展和定制。
//当需要销毁 TcpEnvBase 类或其子类的对象时，可以调用 DoDispose 函数来释放资源并执行清理操作。这有助于确保资源的正确释放，避免内存泄漏等问题。
{
    NS_LOG_FUNCTION(this);
}

void
TcpEnvBase::SetNodeId(uint32_t id)
//这个函数的作用是设置 TcpEnvBase 类对象的节点 ID，通过将节点 ID 存储在成员变量中，后续可以通过该类的其他成员函数或外部代码访问和使用这个节点 ID。
{
    NS_LOG_FUNCTION(this);
    m_nodeId = id;
}

void
TcpEnvBase::SetSocketUuid(uint32_t id)
//这个函数的作用是设置 TcpEnvBase 类对象的套接字 UUID，通过将 UUID 存储在成员变量中，后续可以通过该类的其他成员函数或外部代码访问和使用这个套接字 UUID。
{
    NS_LOG_FUNCTION(this);
    m_socketUuid = id;
}

std::string
TcpEnvBase::GetTcpCongStateName(const TcpSocketState::TcpCongState_t state)
    //这个函数接受一个 TcpSocketState::TcpCongState_t 类型的参数 state，用于表示 TCP 拥塞状态。
//使用一个 switch 语句根据传入的 state 值来设置 stateName 的值。每个 case 分支对应一个特定的 TCP 拥塞状态，并将对应的状态名赋值给 stateName。
//如果 state 不属于任何已知的状态，使用默认分支将 stateName 设置为 "UNKNOWN"。
//最后，函数返回 stateName，即表示指定的 TCP 拥塞状态的字符串。这个函数的作用是将 TCP 拥塞状态值转换为对应的字符串表示，以便在程序中进行显示或其他处理。
{
    std::string stateName = "UNKNOWN";
    switch (state)
    {
    case TcpSocketState::CA_OPEN:
        stateName = "CA_OPEN";
        break;
    case TcpSocketState::CA_DISORDER:
        stateName = "CA_DISORDER";
        break;
    case TcpSocketState::CA_CWR:
        stateName = "CA_CWR";
        break;
    case TcpSocketState::CA_RECOVERY:
        stateName = "CA_RECOVERY";
        break;
    case TcpSocketState::CA_LOSS:
        stateName = "CA_LOSS";
        break;
    case TcpSocketState::CA_LAST_STATE:
        stateName = "CA_LAST_STATE";
        break;
    default:
        stateName = "UNKNOWN";
        break;
    }
    return stateName;
}

std::string
TcpEnvBase::GetTcpCAEventName(const TcpSocketState::TcpCAEvent_t event)
    //这段代码定义了一个名为 GetTcpCAEventName 的函数，它是 TcpEnvBase 类的一部分。
//这个函数接受一个 TcpSocketState::TcpCAEvent_t 类型的参数 event，用于表示 TCP 拥塞避免事件。
//使用一个 switch 语句根据传入的 event 值来设置 eventName 的值。每个 case 分支对应一个特定的 TCP 拥塞避免事件，并将对应的事件名赋值给 eventName。
//如果 event 不属于任何已知的事件，使用默认分支将 eventName 设置为 "UNKNOWN"。
//最后，函数返回 eventName，即表示指定的 TCP 拥塞避免事件的字符串。这个函数的作用是将 TCP 拥塞避免事件值转换为对应的字符串表示，以便在程序中进行显示或其他处理。
{
    std::string eventName = "UNKNOWN";
    switch (event)
    {
    case TcpSocketState::CA_EVENT_TX_START:
        eventName = "CA_EVENT_TX_START";
        break;
    case TcpSocketState::CA_EVENT_CWND_RESTART:
        eventName = "CA_EVENT_CWND_RESTART";
        break;
    case TcpSocketState::CA_EVENT_COMPLETE_CWR:
        eventName = "CA_EVENT_COMPLETE_CWR";
        break;
    case TcpSocketState::CA_EVENT_LOSS:
        eventName = "CA_EVENT_LOSS";
        break;
    case TcpSocketState::CA_EVENT_ECN_NO_CE:
        eventName = "CA_EVENT_ECN_NO_CE";
        break;
    case TcpSocketState::CA_EVENT_ECN_IS_CE:
        eventName = "CA_EVENT_ECN_IS_CE";
        break;
    case TcpSocketState::CA_EVENT_DELAYED_ACK:
        eventName = "CA_EVENT_DELAYED_ACK";
        break;
    case TcpSocketState::CA_EVENT_NON_DELAYED_ACK:
        eventName = "CA_EVENT_NON_DELAYED_ACK";
        break;
    default:
        eventName = "UNKNOWN";
        break;
    }
    return eventName;
}

/*
Define action space
*/
Ptr<OpenGymSpace>
TcpEnvBase::GetActionSpace()
{
    // new_ssThresh
    // new_cWnd
    uint32_t parameterNum = 2;
    float low = 0.0;
    float high = 65535;
    std::vector<uint32_t> shape = {
    //函数中使用了 std::vector 来存储动作空间的形状，其中 parameterNum 表示参数数量，low 和 high 表示参数的最小值和最大值。
        parameterNum,
    };
    std::string dtype = TypeNameGet<uint32_t>();//然后，使用 TypeNameGet<uint32_t>() 来获取 uint32_t 类型的名称，并将其存储在 dtype 变量中。

    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace>(low, high, shape, dtype);//使用 CreateObject<OpenGymBoxSpace>() 创建一个 OpenGymBoxSpace 对象，并将其返回。
    NS_LOG_INFO("MyGetActionSpace: " << box);//NS_LOG_INFO 宏会将字符串 "MyGetActionSpace: " 和变量 box 的值连接起来，并将结果输出到控制台。
    return box;
}

/*
Define game over condition
*/
bool
TcpEnvBase::GetGameOver()
//具体来说，GetGameOver() 函数用于判断游戏是否结束。根据代码中的实现，它总是返回 false，表示游戏尚未结束。
{
    return false;
}

/*
Define reward function
*/
//具体来说，GetReward() 函数用于获取环境的奖励值。它首先使用 NS_LOG_INFO 宏将环境奖励值 m_envReward 输出到控制台。然后，函数返回 m_envReward 的值
float
TcpEnvBase::GetReward()
{
    NS_LOG_INFO("MyGetReward: " << m_envReward);
    return m_envReward;
}

/*
Define extra info. Optional
*/
std::string
TcpEnvBase::GetExtraInfo()
//具体来说，GetExtraInfo() 函数用于获取额外的信息。它首先使用 NS_LOG_INFO 宏将额外信息 m_info 输出到控制台。然后，函数返回 m_info 的值。

{
    NS_LOG_INFO("MyGetExtraInfo: " << m_info);
    return m_info;
}

/*
Execute received actions
*/
bool
TcpEnvBase::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    //具体来说，函数首先将参数 action 转换为 OpenGymBoxContainer<uint32_t> 类型的指针 box，并获取盒子中的两个值，分别存储在变量 m_new_ssThresh 和 m_new_cWnd 中。
//然后，函数使用 NS_LOG_INFO 宏将 action 输出到控制台。
//最后，函数返回 true。
    Ptr<OpenGymBoxContainer<uint32_t>> box = DynamicCast<OpenGymBoxContainer<uint32_t>>(action);
    m_new_ssThresh = box->GetValue(0);
    m_new_cWnd = box->GetValue(1);

    NS_LOG_INFO("MyExecuteActions: " << action);
    return true;
}

NS_OBJECT_ENSURE_REGISTERED(TcpTimeStepEnv);//具体来说，NS_OBJECT_ENSURE_REGISTERED 是一个宏，它用于检查对象是否已经被注册。如果对象没有被注册，它将调用 objc_registerClass 函数来注册该对象。

TcpTimeStepEnv::TcpTimeStepEnv()
    : TcpEnvBase()
{//构造函数使用了初始化列表来初始化类的成员变量。在这个例子中，只有一个成员变量 m_envReward 被初始化为 0.0。
    NS_LOG_FUNCTION(this);//构造函数还使用了 NS_LOG_FUNCTION 宏来记录函数的调用信息。这个宏会在控制台输出一条日志消息，其中包含函数名和对象的地址。奖励值初始为0
    m_envReward = 0.0;
}

void
TcpTimeStepEnv::ScheduleNextStateRead()
//这段代码的作用是安排一个定时事件，以便在指定的时间间隔后再次调用 TcpTimeStepEnv::ScheduleNextStateRead() 函数。通过这种方式，可以实现周期性的状态读取或其他定时操作。
{
    NS_LOG_FUNCTION(this);
    Simulator::Schedule(m_timeStep, &TcpTimeStepEnv::ScheduleNextStateRead, this);
    //该函数使用了 Simulator 类的 Schedule() 方法来安排一个定时事件，该事件将在指定的时间间隔（m_timeStep）后触发。在这个例子中，定时事件的处理函数是 TcpTimeStepEnv::ScheduleNextStateRead 本身，并且参数是 this 指针，这意味着定时事件将在当前对象上触发。
    Notify();
    //函数使用 Notify() 方法来通知其他对象或观察者关于定时事件的安排。这个通知可能用于协调其他对象的操作或进行状态更新。
}

TcpTimeStepEnv::~TcpTimeStepEnv()
//这段代码的作用是在 TcpTimeStepEnv 对象被销毁时记录函数的调用信息
{
    NS_LOG_FUNCTION(this);
}

TypeId
TcpTimeStepEnv::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpTimeStepEnv")//创建一个新的 TypeId 对象，并将其命名为 "ns3::TcpTimeStepEnv"。
                            .SetParent<TcpEnvBase>()//将该 TypeId 的父类设置为 TcpEnvBase。
                            .SetGroupName("Ns3Ai")//将该 TypeId 的组名设置为 "Ns3Ai"。
                            .AddConstructor<TcpTimeStepEnv>()//添加一个构造函数，该构造函数的参数为 TcpTimeStepEnv 类型。
                            .AddAttribute("StepTime",
                                          "Step interval used in TCP env. Default: 100ms",
                                          TimeValue(MilliSeconds(100)),//添加一个名为 "StepTime" 的属性。该属性的默认值为 MilliSeconds(100)，表示步长时间间隔为 100 毫秒
                                          MakeTimeAccessor(&TcpTimeStepEnv::m_timeStep),//用于访问和检查该属性的值
                                          MakeTimeChecker());

    return tid;
}

void
TcpTimeStepEnv::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
TcpTimeStepEnv::GetObservationSpace()
//该函数创建了一个 Ptr<OpenGymBoxSpace> 对象，该对象表示一个 OpenGym 框空间。框空间是一种用于表示连续或离散的数值范围的空间，常用于强化学习环境中。
{
    // socket unique ID
    // tcp env type: event-based = 0 / time-based = 1
    // sim time in us
    // node ID
    // ssThresh
    // cWnd
    // segmentSize
    // bytesInFlightSum
    // bytesInFlightAvg
    // segmentsAckedSum
    // segmentsAckedAvg
    // avgRtt
    // minRtt
    // avgInterTx
    // avgInterRx
    // throughput
    uint32_t parameterNum = 16;
    float low = 0.0;  //框空间的下限值，这里设置为 0.0。
    float high = 1000000000.0;//框空间的上限值，这里设置为 1000000000.0
    std::vector<uint32_t> shape = {
    //：框空间的形状，这里是一个包含一个元素的向量，表示只有一个参数。
        parameterNum,
    };
    std::string dtype = TypeNameGet<uint64_t>(); //框空间的数据类型，这里设置为 TypeNameGet<uint64_t>()，表示使用 64 位无符号整数类型。

    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace>(low, high, shape, dtype);
    NS_LOG_INFO("MyGetObservationSpace: " << box);
    return box;  //该函数将创建的框空间对象返回。
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer>
TcpTimeStepEnv::GetObservation() //初始化
{
    uint32_t parameterNum = 16;  //被设置为 16，表示观测中的参数数量。
    std::vector<uint32_t> shape = {  //一个指定观测形状的向量。
        parameterNum,
    };

    Ptr<OpenGymBoxContainer<uint64_t>> box = CreateObject<OpenGymBoxContainer<uint64_t>>(shape); //创建一个 OpenGymBoxContainer<uint64_t> 类型的对象，命名为 box，并使用指定的形状进行初始化。

    //使用 AddValue 方法向容器中添加数值
    box->AddValue(m_socketUuid);
    box->AddValue(1);
    box->AddValue(Simulator::Now().GetMicroSeconds());
    box->AddValue(m_nodeId);
    box->AddValue(m_tcb->m_ssThresh);
    box->AddValue(m_tcb->m_cWnd);
    box->AddValue(m_tcb->m_segmentSize);

    // bytesInFlightSum
    uint64_t bytesInFlightSum = std::accumulate(m_bytesInFlight.begin(), m_bytesInFlight.end(), 0);
    box->AddValue(bytesInFlightSum);

    // bytesInFlightAvg
    uint64_t bytesInFlightAvg = 0;
    if (!m_bytesInFlight.empty())
    {
        bytesInFlightAvg = bytesInFlightSum / m_bytesInFlight.size();
    }
    box->AddValue(bytesInFlightAvg);

    // segmentsAckedSum
    uint64_t segmentsAckedSum = std::accumulate(m_segmentsAcked.begin(), m_segmentsAcked.end(), 0);
    box->AddValue(segmentsAckedSum);

    // segmentsAckedAvg
    uint64_t segmentsAckedAvg = 0;
    if (!m_segmentsAcked.empty())
    {
        segmentsAckedAvg = segmentsAckedSum / m_segmentsAcked.size();
    }
    box->AddValue(segmentsAckedAvg);

    // avgRtt
    Time avgRtt = Seconds(0.0);
    if (m_rttSampleNum)
    {
        avgRtt = m_rttSum / m_rttSampleNum;
    }
    box->AddValue(avgRtt.GetMicroSeconds());

    // m_minRtt
    box->AddValue(m_tcb->m_minRtt.GetMicroSeconds());

    // avgInterTx
    Time avgInterTx = Seconds(0.0);
    if (m_interTxTimeNum)
    {
        avgInterTx = m_interTxTimeSum / m_interTxTimeNum;
    }
    box->AddValue(avgInterTx.GetMicroSeconds());

    // avgInterRx
    Time avgInterRx = Seconds(0.0);
    if (m_interRxTimeNum)
    {
        avgInterRx = m_interRxTimeSum / m_interRxTimeNum;
    }
    box->AddValue(avgInterRx.GetMicroSeconds());

    // throughput  bytes/s
    float throughput = (segmentsAckedSum * m_tcb->m_segmentSize) / m_timeStep.GetSeconds();
    box->AddValue(throughput);

    // Print data 使用 NS-3 日志记录（NS_LOG_INFO）记录 box 中的数据
    NS_LOG_INFO("MyGetObservation: " << box);

    //清空或重置各种数据结构，为下一次生成观测数据做准备。
    m_bytesInFlight.clear();
    m_segmentsAcked.clear();

    m_rttSampleNum = 0;
    m_rttSum = MicroSeconds(0.0);

    m_interTxTimeNum = 0;
    m_interTxTimeSum = MicroSeconds(0.0);

    m_interRxTimeNum = 0;
    m_interRxTimeSum = MicroSeconds(0.0);

    return box;
}

void
TcpTimeStepEnv::TxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
    if (m_lastPktTxTime > MicroSeconds(0.0))
    {
        Time interTxTime = Simulator::Now() - m_lastPktTxTime;
        m_interTxTimeSum += interTxTime;
        m_interTxTimeNum++;
    }

    m_lastPktTxTime = Simulator::Now();
}

void
TcpTimeStepEnv::RxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
    if (m_lastPktRxTime > MicroSeconds(0.0))
    {
        Time interRxTime = Simulator::Now() - m_lastPktRxTime;
        m_interRxTimeSum += interRxTime;
        m_interRxTimeNum++;
    }

    m_lastPktRxTime = Simulator::Now();
}

uint32_t
TcpTimeStepEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " GetSsThresh, BytesInFlight: " << bytesInFlight);
    m_tcb = tcb;
    m_bytesInFlight.push_back(bytesInFlight);

    if (!m_started)
    {
        m_started = true;
        //        Notify();
        ScheduleNextStateRead();
    }

    // action
    return m_new_ssThresh;
}

void
TcpTimeStepEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
    m_tcb = tcb;
    m_segmentsAcked.push_back(segmentsAcked);
    m_bytesInFlight.push_back(tcb->m_bytesInFlight);

    if (!m_started)
    {
        m_started = true;
        //        Notify();
        ScheduleNextStateRead();
    }
    // action
    tcb->m_cWnd = m_new_cWnd;
}

void
TcpTimeStepEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " PktsAcked, SegmentsAcked: "
                                 << segmentsAcked << " Rtt: " << rtt);
    m_tcb = tcb;
    m_rttSum += rtt;
    m_rttSampleNum++;
}

void
TcpTimeStepEnv::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this);
    std::string stateName = GetTcpCongStateName(newState);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CongestionStateSet: " << newState
                                 << " " << stateName);
    m_tcb = tcb;
}

void
TcpTimeStepEnv::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this);
    std::string eventName = GetTcpCAEventName(event);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CwndEvent: " << event << " "
                                 << eventName);
    m_tcb = tcb;
}

NS_OBJECT_ENSURE_REGISTERED(TcpEventBasedEnv);

TcpEventBasedEnv::TcpEventBasedEnv()
    : TcpEnvBase()
{
    NS_LOG_FUNCTION(this);
}

TcpEventBasedEnv::~TcpEventBasedEnv()
{
    NS_LOG_FUNCTION(this);
}

TypeId
TcpEventBasedEnv::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpEventBasedEnv")
                            .SetParent<TcpEnvBase>()
                            .SetGroupName("Ns3Ai")
                            .AddConstructor<TcpEventBasedEnv>();

    return tid;
}

void
TcpEventBasedEnv::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

void
TcpEventBasedEnv::SetReward(float value)
{
    NS_LOG_FUNCTION(this);
    m_reward = value;
}

void
TcpEventBasedEnv::SetPenalty(float value)
{
    NS_LOG_FUNCTION(this);
    m_penalty = value;
}

/*
Define observation space
*/
Ptr<OpenGymSpace>
TcpEventBasedEnv::GetObservationSpace()
{
    // socket unique ID
    // tcp env type: event-based = 0 / time-based = 1
    // sim time in us
    // node ID
    // ssThresh
    // cWnd
    // segmentSize
    // segmentsAcked
    // bytesInFlight
    // rtt in us
    // min rtt in us
    // called func
    // congetsion algorithm (CA) state
    // CA event
    // ECN state
    uint32_t parameterNum = 15;
    float low = 0.0;
    float high = 1000000000.0;
    std::vector<uint32_t> shape = {
        parameterNum,
    };
    std::string dtype = TypeNameGet<uint64_t>();

    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace>(low, high, shape, dtype);
    NS_LOG_INFO("MyGetObservationSpace: " << box);
    return box;
}

/*
Collect observations
*/
Ptr<OpenGymDataContainer>
TcpEventBasedEnv::GetObservation()
{
    uint32_t parameterNum = 15;
    std::vector<uint32_t> shape = {
        parameterNum,
    };

    Ptr<OpenGymBoxContainer<uint64_t>> box = CreateObject<OpenGymBoxContainer<uint64_t>>(shape);

    box->AddValue(m_socketUuid);
    box->AddValue(0);
    box->AddValue(Simulator::Now().GetMicroSeconds());
    box->AddValue(m_nodeId);
    box->AddValue(m_tcb->m_ssThresh);
    box->AddValue(m_tcb->m_cWnd);
    box->AddValue(m_tcb->m_segmentSize);
    box->AddValue(m_segmentsAcked);
    box->AddValue(m_bytesInFlight);
    box->AddValue(m_rtt.GetMicroSeconds());
    box->AddValue(m_tcb->m_minRtt.GetMicroSeconds());
    box->AddValue(m_calledFunc);
    box->AddValue(m_tcb->m_congState);
    box->AddValue(m_event);
    box->AddValue(m_tcb->m_ecnState);

    // Print data
    NS_LOG_INFO("MyGetObservation: " << box);
    return box;
}

void
TcpEventBasedEnv::TxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
}

void
TcpEventBasedEnv::RxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
}

uint32_t
TcpEventBasedEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this);
    // pkt was lost, so penalty
    m_envReward = m_penalty;

    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " GetSsThresh, BytesInFlight: " << bytesInFlight);
    m_calledFunc = CalledFunc_t::GET_SS_THRESH;
    m_info = "GetSsThresh";
    m_tcb = tcb;
    m_bytesInFlight = bytesInFlight;
    Notify();
    return m_new_ssThresh;
}

void
TcpEventBasedEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this);
    // pkt was acked, so reward
    m_envReward = m_reward;

    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
    m_calledFunc = CalledFunc_t::INCREASE_WINDOW;
    m_info = "IncreaseWindow";
    m_tcb = tcb;
    m_segmentsAcked = segmentsAcked;
    Notify();
    tcb->m_cWnd = m_new_cWnd;
}

void
TcpEventBasedEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " PktsAcked, SegmentsAcked: "
                                 << segmentsAcked << " Rtt: " << rtt);
    m_calledFunc = CalledFunc_t::PKTS_ACKED;
    m_info = "PktsAcked";
    m_tcb = tcb;
    m_segmentsAcked = segmentsAcked;
    m_rtt = rtt;
}

void
TcpEventBasedEnv::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                     const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this);
    std::string stateName = GetTcpCongStateName(newState);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CongestionStateSet: " << newState
                                 << " " << stateName);

    m_calledFunc = CalledFunc_t::CONGESTION_STATE_SET;
    m_info = "CongestionStateSet";
    m_tcb = tcb;
}

void
TcpEventBasedEnv::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this);
    std::string eventName = GetTcpCAEventName(event);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CwndEvent: " << event << " "
                                 << eventName);

    m_calledFunc = CalledFunc_t::CWND_EVENT;
    m_info = "CwndEvent";
    m_tcb = tcb;
    m_event = event;
}

} // namespace ns3
