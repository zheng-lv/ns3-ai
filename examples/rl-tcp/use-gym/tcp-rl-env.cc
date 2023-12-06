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

    //向 box 中添加与 TCP（传输控制协议）相关的各种参数，如套接字 UUID、节点 ID、慢启动阈值、拥塞窗口、段大小、在飞行中的字节数、已确认的段数、平均往返时延（RTT）、最小 RTT、平均传输间隔、平均接收间隔和吞吐量。
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

    return box;//返回 box，可能用作强化学习代理的观测数据。
}

void
TcpTimeStepEnv::TxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
//一个用于跟踪 TCP 数据包传输的函数 TxPktTrace
//返回类型为 void，表示不返回任何值。
//函数名为 TxPktTrace。
//接受三个参数：
//Ptr<const Packet>：指向常量数据包的指针。
//const TcpHeader&：TCP 头部的常量引用。
//Ptr<const TcpSocketBase>：指向常量 TCP 套接字基类的指针。
{
    NS_LOG_FUNCTION(this); //使用 NS-3 的日志功能记录函数调用，包括当前对象的地址（this）。
    if (m_lastPktTxTime > MicroSeconds(0.0))  //检查 m_lastPktTxTime 是否大于零，即上一个数据包传输时间是否已经记录。
    {
        Time interTxTime = Simulator::Now() - m_lastPktTxTime;
        m_interTxTimeSum += interTxTime;
        m_interTxTimeNum++;
        //如果上一个数据包传输时间已经记录，计算当前时间与上一个数据包传输时间的时间间隔，并将其加到 m_interTxTimeSum 中，同时增加 m_interTxTimeNum 计数器。
    }

    m_lastPktTxTime = Simulator::Now(); //更新 m_lastPktTxTime 为当前模拟时间，以备下一次调用时使用。
}

void
TcpTimeStepEnv::RxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
//一个用于跟踪 TCP 数据包接收的函数 RxPktTrace
{
    NS_LOG_FUNCTION(this);
    if (m_lastPktRxTime > MicroSeconds(0.0))  //检查 m_lastPktRxTime 是否大于零，即上一个数据包接收时间是否已经记录。
    {
        Time interRxTime = Simulator::Now() - m_lastPktRxTime;
        m_interRxTimeSum += interRxTime;
        m_interRxTimeNum++;
        //如果上一个数据包接收时间已经记录，计算当前时间与上一个数据包接收时间的时间间隔，并将其加到 m_interRxTimeSum 中，同时增加 m_interRxTimeNum 计数器。
    }

    m_lastPktRxTime = Simulator::Now(); //更新 m_lastPktRxTime 为当前模拟时间，以备下一次调用时使用。
}


////返回类型：
//uint32_t，表示该函数返回一个 32 位的无符号整数。
//参数：
//Ptr<const TcpSocketState> tcb：指向常量 TcpSocketState 对象的指针。
//uint32_t bytesInFlight：表示当前传输的字节数。
//目的是获取慢启动阈值（Slow Start Threshold），并在此过程中记录相关的信息，如字节数和模拟时间。在函数执行的过程中，可能还会触发一些状态更新或调度，
uint32_t
TcpTimeStepEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " GetSsThresh, BytesInFlight: " << bytesInFlight);
    //使用 NS-3 的日志功能记录函数调用，包括当前对象的地址（this）、模拟时间、节点 ID 以及当前的字节数。
    
    m_tcb = tcb;  //将传递给函数的 tcb 参数保存到成员变量 m_tcb 中。
    m_bytesInFlight.push_back(bytesInFlight);  //将当前的 bytesInFlight 添加到成员变量 m_bytesInFlight 中，这是一个存储字节数的容器。

    if (!m_started)
    {
        m_started = true;
        //        Notify();
        ScheduleNextStateRead();
        //如果 m_started 为假（false），表示这是第一次调用该函数，则将 m_started 设为真（true），并调度下一个状态读取（ScheduleNextStateRead）。
    }

    // action
    return m_new_ssThresh;  //返回成员变量 m_new_ssThresh，该变量表示新的慢启动阈值。
}

//该函数的目的是增加 TCP 拥塞窗口的大小，并在此过程中记录相关的信息，如被确认的数据段数量和模拟时间。
//Ptr<TcpSocketState> tcb：指向 TcpSocketState 对象的指针，表示 TCP 套接字的状态。
//uint32_t segmentsAcked：表示被确认的数据段数量。
void
TcpTimeStepEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
    m_tcb = tcb;  //将传递给函数的 tcb 参数保存到成员变量 m_tcb 中，表示当前 TCP 套接字的状态。
    m_segmentsAcked.push_back(segmentsAcked);  //将 segmentsAcked 添加到成员变量 m_segmentsAcked 中，这是一个存储被确认数据段数量的容器。
    m_bytesInFlight.push_back(tcb->m_bytesInFlight); //将套接字的字节数添加到成员变量 m_bytesInFlight 中，这是一个存储字节数的容器。

    if (!m_started)
    {
        m_started = true;
        //        Notify();
        ScheduleNextStateRead();
        //如果 m_started 为假（false），表示这是第一次调用该函数，则将 m_started 设为真（true），并调度下一个状态读取（ScheduleNextStateRead）
    }
    // action 执行动作（action），即将套接字状态中的拥塞窗口大小 m_cWnd 更新为成员变量 m_new_cWnd。
    tcb->m_cWnd = m_new_cWnd;
}

//处理数据包被确认的事件，记录相关信息如被确认的数据段数量和往返时延，
//Ptr<TcpSocketState> tcb：指向 TcpSocketState 对象的指针，表示 TCP 套接字的状态。
//uint32_t segmentsAcked：表示被确认的数据段数量。
//const Time& rtt：表示往返时延（Round-Trip Time）。
void
TcpTimeStepEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " PktsAcked, SegmentsAcked: "
                                 << segmentsAcked << " Rtt: " << rtt);
    m_tcb = tcb;  //将传递给函数的 tcb 参数保存到成员变量 m_tcb 中，表示当前 TCP 套接字的状态
    m_rttSum += rtt;  //将往返时延 rtt 累加到成员变量 m_rttSum 中，并增加 m_rttSampleNum。
    m_rttSampleNum++;
}

//该函数的目的是设置拥塞状态，并记录相关信息。在实际的网络仿真中，TCP 协议会根据网络状况动态调整拥塞窗口大小，以实现网络的高效利用。拥塞状态的改变可能触发一系列操作，如调整拥塞窗口大小、改变传输速率等。
//Ptr<TcpSocketState> tcb：指向 TcpSocketState 对象的指针，表示 TCP 套接字的状态。
//const TcpSocketState::TcpCongState_t newState：表示设置的拥塞状态。
void
TcpTimeStepEnv::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState)
{
    //使用 NS-3 的日志功能记录函数调用，包括当前对象的地址（this）、模拟时间、节点 ID、设置的拥塞状态值和状态名称。
    NS_LOG_FUNCTION(this);
    std::string stateName = GetTcpCongStateName(newState);
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CongestionStateSet: " << newState
                                 << " " << stateName);
    m_tcb = tcb; //将传递给函数的 tcb 参数保存到成员变量 m_tcb 中，表示当前 TCP 套接字的状态。
}

//是处理与 TCP 拥塞控制相关的事件
//该函数接受一个 Ptr<TcpSocketState> 类型的参数 tcb，表示一个指向 TcpSocketState 对象的智能指针。该对象表示 TCP 套接字状态，其中包含有关当前连接的信息。
//函数还接受一个 TcpSocketState::TcpCAEvent_t 类型的参数 event，表示一个 TcpCAEvent_t 类型的事件。TcpCAEvent_t 是一个枚举类型，定义了与 TCP 拥塞控制相关的事件类型。
void
TcpTimeStepEnv::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this);
    std::string eventName = GetTcpCAEventName(event); //使用 GetTcpCAEventName() 方法将事件类型转换为一个字符串，并将其存储在 eventName 变量中。
    //它使用 NS_LOG_INFO() 宏记录一个日志消息，其中包含当前时间、节点 ID、事件类型和事件名称。
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CwndEvent: " << event << " "
                                 << eventName);
    m_tcb = tcb;  //该函数将 tcb 变量赋值为传入的 tcb 参数，以便在后续操作中使用。
}

NS_OBJECT_ENSURE_REGISTERED(TcpEventBasedEnv); //确保 TcpEventBasedEnv 类在运行时被注册到对象系统中。

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
                            .AddConstructor<TcpEventBasedEnv>();//AddConstructor<TcpEventBasedEnv>(); 是一个模板方法，它接受一个类型参数 TcpEventBasedEnv，表示要注册构造函数的类。该方法将注册 TcpEventBasedEnv 类的默认构造函数，以便在运行时可以创建该类的对象。

    return tid;
}

void
TcpEventBasedEnv::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

//通过调用 TcpEventBasedEnv::SetReward() 函数，你可以设置 TcpEventBasedEnv 类的奖励值。奖励值通常用于强化学习算法中，表示环境对智能体的奖励或惩罚。通过设置奖励值，你可以影响智能体的学习行为和策略。
void
TcpEventBasedEnv::SetReward(float value)
{
    NS_LOG_FUNCTION(this);
    m_reward = value; //该函数接受一个浮点数类型的参数 value，并将其赋值给类的成员变量 m_reward。
}

//通过调用 TcpEventBasedEnv::SetPenalty() 函数，你可以设置 TcpEventBasedEnv 类的惩罚值。惩罚值通常用于强化学习算法中，表示环境对智能体的惩罚或负面反馈。通过设置惩罚值，你可以影响智能体的学习行为和策略。
void
TcpEventBasedEnv::SetPenalty(float value)
{
    NS_LOG_FUNCTION(this);
    m_penalty = value; //该函数接受一个浮点数类型的参数 value，并将其赋值给类的成员变量 m_penalty。
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
    uint32_t parameterNum = 15; //定义了一个名为 parameterNum 的变量，它表示空间中元素的数量。
    float low = 0.0;
    float high = 1000000000.0;  //定义了一个名为 low 和 high 的变量，它们分别表示空间中元素的最小值和最大值
    std::vector<uint32_t> shape = {
        parameterNum,
    }; //它创建了一个包含 parameterNum 个元素的向量 shape，并将其数据类型设置为 TypeNameGet<uint64_t>()。
    std::string dtype = TypeNameGet<uint64_t>();

    //它使用 CreateObject<OpenGymBoxSpace>() 函数创建了一个 OpenGym 盒子空间，并将其返回。
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
//该函数创建了一个 OpenGym 数据容器，该容器用于表示从环境中观察到的状态。该容器的形状是一个包含 15 个元素的向量，每个元素的类型是 uint64_t

    //该函数使用 AddValue() 方法将一些值添加到容器中
    //这些值包括：socketUuid、0（可能是一个占位符）、当前时间（以微秒为单位）、节点 ID、ssThresh、cWnd、segmentSize、segmentsAcked、bytesInFlight、rtt（往返时间）、minRtt（最小往返时间）、calledFunc、congState（拥塞状态）、event（事件）和 ecnState（ECN 状态）。
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

    // Print data 该函数打印容器中的数据，并返回容器本身。
    NS_LOG_INFO("MyGetObservation: " << box);
    return box;
}

//该函数接受三个参数：一个指向 const Packet 对象的指针、一个 const TcpHeader 对象和一个指向 const TcpSocketBase 对象的指针。
//这些参数表示要传输的数据包、数据包的 TCP 头部和与该数据包相关的 TcpSocketBase 对象。
void
TcpEventBasedEnv::TxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
}

//该函数接受三个参数：一个指向 const Packet 对象的指针、一个 const TcpHeader 对象和一个指向 const TcpSocketBase 对象的指针。这些参数表示接收到的数据包、数据包的 TCP 头部和与该数据包相关的 TcpSocketBase 对象。
void
TcpEventBasedEnv::RxPktTrace(Ptr<const Packet>, const TcpHeader&, Ptr<const TcpSocketBase>)
{
    NS_LOG_FUNCTION(this);
}

//该函数接受两个参数：一个指向 const TcpSocketState 对象的指针 tcb 和一个表示当前飞行中的字节数的 uint32_t 类型的变量 bytesInFlight。
uint32_t
TcpEventBasedEnv::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this); //首先使用 NS_LOG_FUNCTION() 宏记录函数的调用信息，包括函数名和当前对象的指针。
    // pkt was lost, so penalty 
    m_envReward = m_penalty;

    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " GetSsThresh, BytesInFlight: " << bytesInFlight);
    //函数设置了一些变量的值，包括 m_envReward、m_calledFunc、m_info、m_tcb 和 m_bytesInFlight，并调用了 Notify() 函数。
    m_calledFunc = CalledFunc_t::GET_SS_THRESH;
    m_info = "GetSsThresh";
    m_tcb = tcb;
    m_bytesInFlight = bytesInFlight;
    Notify();
    return m_new_ssThresh;  //函数返回一个 uint32_t 类型的值 m_new_ssThresh，但没有说明该值的具体含义。
}

//Ptr<TcpSocketState> tcb：指向 TcpSocketState 对象的指针，表示 TCP 套接字的状态。
//uint32_t segmentsAcked：表示已被确认的 TCP 段的数量。
void
TcpEventBasedEnv::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this); //使用 NS-3 的日志功能记录函数调用，包括当前对象的地址（this）、模拟时间、节点 ID 和已确认的 TCP 段数量。
    // pkt was acked, so reward
    m_envReward = m_reward;

    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId
                                 << " IncreaseWindow, SegmentsAcked: " << segmentsAcked);
    m_calledFunc = CalledFunc_t::INCREASE_WINDOW;
    m_info = "IncreaseWindow";
    m_tcb = tcb;
    m_segmentsAcked = segmentsAcked; //将传递给函数的 segmentsAcked 参数保存到成员变量 m_segmentsAcked 中，表示已确认的 TCP 段的数量。
    Notify(); //通过调用 Notify() 函数通知相关的观察者（observers）。
    tcb->m_cWnd = m_new_cWnd;
}

//该函数接受三个参数：一个指向 TcpSocketState 对象的指针 tcb，一个表示确认的数据包数的 uint32_t 类型的变量 segmentsAcked，以及一个表示往返时间（Round Trip Time，RTT）的 Time 对象 rtt。
void
TcpEventBasedEnv::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this); //首先使用 NS_LOG_FUNCTION() 宏记录函数的调用信息，包括函数名和当前对象的指针。
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " PktsAcked, SegmentsAcked: "
                                 << segmentsAcked << " Rtt: " << rtt); //它使用 NS_LOG_INFO() 宏记录一些日志信息，包括当前时间、节点 ID、确认的数据包数和 RTT。
   //函数设置了一些变量的值，包括 m_calledFunc、m_info、m_tcb 和 m_segmentsAcked。这些变量可能在函数的后续操作中使用。
    m_calledFunc = CalledFunc_t::PKTS_ACKED;
    m_info = "PktsAcked";
    m_tcb = tcb;
    m_segmentsAcked = segmentsAcked;
    m_rtt = rtt;
}

//用于记录 TCP 拥塞状态变更事件的函数
void
TcpEventBasedEnv::CongestionStateSet(Ptr<TcpSocketState> tcb,
                                     const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this);
    //用于记录 TCP 拥塞状态变更事件的函数
    std::string stateName = GetTcpCongStateName(newState);
    //// 输出信息级别的日志，记录节点 ID、拥塞状态值和对应的状态字符串
    NS_LOG_INFO(Simulator::Now() << " Node: " << m_nodeId << " CongestionStateSet: " << newState
                                 << " " << stateName);

    //// 记录函数调用
    m_calledFunc = CalledFunc_t::CONGESTION_STATE_SET; //记录函数调用类型为 CONGESTION_STATE_SET，这可能是一个用于在类内部跟踪函数调用的成员变量。
    m_info = "CongestionStateSet"; //记录函数调用类型为 CONGESTION_STATE_SET，这可能是一个用于在类内部跟踪函数调用的成员变量。
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
