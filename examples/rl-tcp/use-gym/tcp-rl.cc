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

#include "tcp-rl.h"

#include "tcp-rl-env.h"

#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/node-list.h"
#include "ns3/object.h"
#include "ns3/simulator.h"
#include "ns3/tcp-header.h"
#include "ns3/tcp-l4-protocol.h"
#include "ns3/tcp-socket-base.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("tcp-rl-gym");

NS_OBJECT_ENSURE_REGISTERED(TcpSocketDerived);

TypeId
TcpSocketDerived::GetTypeId()  //定义了一个 TypeId 函数，用于获取 TcpSocketDerived 类的类型标识符
{
    static TypeId tid = TypeId("ns3::TcpSocketDerived")
                            .SetParent<TcpSocketBase>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpSocketDerived>();
    return tid;
}

TypeId
TcpSocketDerived::GetInstanceTypeId() const //TypeId 函数：TcpSocketDerived::GetInstanceTypeId() 是一个 const 成员函数，它返回 TcpSocketDerived 类的类型标识符。该函数使用 TcpSocketDerived::GetTypeId() 来获取类型标识符。
{
    return TcpSocketDerived::GetTypeId(); //GetTypeId() 是一个静态成员函数，它返回一个 TypeId 对象，该对象包含了 TcpSocketDerived 类的相关信息，如类名、父类、所属群组等。
}
//构造函数 TcpSocketDerived：该构造函数是 TcpSocketDerived 类的默认构造函数，它没有任何参数。
TcpSocketDerived::TcpSocketDerived()
{
}

Ptr<TcpCongestionOps>
TcpSocketDerived::GetCongestionControlAlgorithm() //定义了一个 TcpSocketDerived 类的成员函数 GetCongestionControlAlgorithm() 
{
    return m_congestionControl; //m_congestionControl 是一个成员变量，它可能存储了指向拥塞控制算法对象的指针。
}

TcpSocketDerived::~TcpSocketDerived() //一个析构函数 ~TcpSocketDerived()。
{
}

NS_OBJECT_ENSURE_REGISTERED(TcpRlBase);

TypeId
TcpRlBase::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpRlBase")
                            .SetParent<TcpCongestionOps>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpRlBase>();
    return tid;
}

TcpRlBase::TcpRlBase()
    : TcpCongestionOps() //TcpRlBase() 构造函数使用了初始化列表 : TcpCongestionOps()，这表示它调用了基类 TcpCongestionOps 的构造函数。
{
    NS_LOG_FUNCTION(this);//输出了当前对象的地址（this 指针）。
    m_tcpSocket = nullptr;
    m_TcpEnvBase = nullptr; //m_tcpSocket 和 m_TcpEnvBase 是 TcpRlBase 类的成员变量。在构造函数中，它们被初始化为 nullptr，即空指针。这些变量可能用于在类的其他方法中引用与TCP和环境相关的对象。
}

TcpRlBase::TcpRlBase(const TcpRlBase& sock)
    : TcpCongestionOps(sock) //TcpRlBase(const TcpRlBase& sock) 拷贝构造函数使用了初始化列表 : TcpCongestionOps(sock)，这表示它调用了基类 TcpCongestionOps 的拷贝构造函数，将 sock 对象的状态拷贝给新创建的对象。
{
    NS_LOG_FUNCTION(this);
    m_tcpSocket = nullptr;
    m_TcpEnvBase = nullptr;
}

TcpRlBase::~TcpRlBase()
{
    m_tcpSocket = nullptr;
    m_TcpEnvBase = nullptr; //在这个特定的析构函数中，将 m_tcpSocket 和 m_TcpEnvBase 设置为 nullptr，可能是为了确保在对象被销毁时，相关的指针不再指向有效的内存。
}

uint64_t
TcpRlBase::GenerateUuid() //这是一个用于生成唯一标识符（UUID）的函数 TcpRlBase::GenerateUuid()。它使用了一个静态变量 uuid 来保持生成的唯一标识符的状态，每次调用函数时，uuid 递增并返回其当前值。
{
    static uint64_t uuid = 0;
    uuid++;
    return uuid;
}
//TcpRlBase 提供了一个虚函数 CreateGymEnv，但是它本身不提供具体的实现。子类 TcpRlEventBased 和 TcpRlTimeBased 应该分别提供它们自己的实现。
void
TcpRlBase::CreateGymEnv()
{
    NS_LOG_FUNCTION(this);
    // should never be called, only child classes: TcpRlEventBased and TcpRlTimeBased 这是一个名为 CreateGymEnv 的函数，它在 TcpRlBase 类中定义。根据函数的注释，这个函数应该永远不会被调用，而是应该在 TcpRlBase 的子类中实现。具体来说，它注释说这个函数应该只在 TcpRlEventBased 和 TcpRlTimeBased 这两个子类中被调用，而在基类 TcpRlBase 中，这个函数没有实际的实现。
}

void
TcpRlBase::ConnectSocketCallbacks() //这是一个用于连接套接字回调的函数，主要用于在模拟中找到使用当前 TcpRlBase 拥塞控制算法的套接字。以下是该函数的主要注释和解释：
{
    NS_LOG_FUNCTION(this);

    //标志，指示是否找到了匹配的套接字
    bool foundSocket = false;
    // 迭代网络中的所有节点
    for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Node> node = *i;
        Ptr<TcpL4Protocol> tcp = node->GetObject<TcpL4Protocol>();

        //获取该节点上所有的 TCP 套接字对象
        ObjectVectorValue socketVec;
        tcp->GetAttribute("SocketList", socketVec);
        NS_LOG_DEBUG("Node: " << node->GetId() << " TCP socket num: " << socketVec.GetN());

        // 获取套接字的数量
        uint32_t sockNum = socketVec.GetN();
        // 遍历该节点上的每个套接字
        for (uint32_t j = 0; j < sockNum; j++)
        {
            Ptr<Object> sockObj = socketVec.Get(j);
            Ptr<TcpSocketBase> tcpSocket = DynamicCast<TcpSocketBase>(sockObj);
            NS_LOG_DEBUG("Node: " << node->GetId() << " TCP Socket: " << tcpSocket);
            // 检查套接字的拥塞控制算法
            if (!tcpSocket)
            {
                continue;
            }

            // 获取套接字关联的拥塞控制算法
            Ptr<TcpSocketDerived> dtcpSocket = StaticCast<TcpSocketDerived>(tcpSocket);
            Ptr<TcpCongestionOps> ca = dtcpSocket->GetCongestionControlAlgorithm();
            NS_LOG_DEBUG("CA name: " << ca->GetName());
            Ptr<TcpRlBase> rlCa = DynamicCast<TcpRlBase>(ca);
            // 如果找到了使用当前 TcpRlBase 拥塞控制算法的套接字
            if (rlCa == this)
            {
                NS_LOG_DEBUG("Found TcpRl CA!");
                foundSocket = true;
                m_tcpSocket = tcpSocket;// 将找到的套接字存储在成员变量 m_tcpSocket 中
                break;// 找到匹配的套接字后退出循环
            }
        }

        if (foundSocket)
        {
            break;// 找到匹配的套接字后退出节点迭代循环
        }
    }

    NS_ASSERT_MSG(m_tcpSocket, "TCP socket was not found.");

    //如果 m_tcpSocket 不为空，连接套接字的发送和接收回调函数，这两个回调函数分别是 TxPktTrace 和 RxPktTrace，这两个回调函数的实现属于 TcpEnvBase 类。
    if (m_tcpSocket)
    {
        NS_LOG_DEBUG("Found TCP Socket: " << m_tcpSocket);
        // 连接发送回调函数
        m_tcpSocket->TraceConnectWithoutContext(
            "Tx",
            MakeCallback(&TcpEnvBase::TxPktTrace, m_TcpEnvBase));
        // 连接接收回调函数
        m_tcpSocket->TraceConnectWithoutContext(
            "Rx",
            MakeCallback(&TcpEnvBase::RxPktTrace, m_TcpEnvBase));
        NS_LOG_DEBUG("Connect socket callbacks " << m_tcpSocket->GetNode()->GetId());
        m_TcpEnvBase->SetNodeId(m_tcpSocket->GetNode()->GetId());
    }
}
//实现 GetName 函数，返回当前 TcpRlBase 对象的名称。
std::string
TcpRlBase::GetName() const
{
    return "TcpRlBase";
}
//通过这种方式，GetSsThresh() 函数根据传入的 TcpSocketState 对象和飞行中字节数来获取新的慢启动阈值（SsThresh）。它首先检查环境是否已经创建，如果没有，则创建环境。然后，它调用环境对象的 GetSsThresh() 函数来获取慢启动阈值，并返回该值。
uint32_t
TcpRlBase::GetSsThresh(Ptr<const TcpSocketState> state, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << state << bytesInFlight);//该函数使用了 NS_LOG_FUNCTION 宏来输出一条日志消息，其中包含了当前对象的指针 this、传入的参数 state（一个指向 TcpSocketState 对象的指针）和 bytesInFlight（一个表示飞行中字节数的无符号整数）。

    if (!m_TcpEnvBase)//函数检查成员变量 m_TcpEnvBase 是否为空指针。如果是，则调用 CreateGymEnv() 函数来创建环境。
    {
        CreateGymEnv();
    }

    uint32_t newSsThresh = 0;//数声明了一个名为 newSsThresh 的无符号整数变量，并将其初始化为 0。
    if (m_TcpEnvBase) //如果 m_TcpEnvBase 不为空指针，它将调用 m_TcpEnvBase->GetSsThresh() 函数，并将返回值存储在 newSsThresh 变量中。
    {
        newSsThresh = m_TcpEnvBase->GetSsThresh(state, bytesInFlight);
    }

    return newSsThresh;
}

void
TcpRlBase::IncreaseWindow(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
    NS_LOG_FUNCTION(this << tcb << segmentsAcked);

    if (!m_TcpEnvBase)
    {
        CreateGymEnv();
    }

    if (m_TcpEnvBase)
    {
        m_TcpEnvBase->IncreaseWindow(tcb, segmentsAcked);
    }
}
//通过这种方式，IncreaseWindow() 函数根据传入的 TcpSocketState 对象和已确认分段数量来增加窗口大小。它首先检查环境是否已经创建，如果没有，则创建环境。然后，它调用环境对象的 IncreaseWindow() 函数来增加窗口大小，并返回新的慢启动阈值。
void
TcpRlBase::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt)
{
    NS_LOG_FUNCTION(this);

    if (!m_TcpEnvBase)
    {
        CreateGymEnv();
    }

    if (m_TcpEnvBase)
    {
        m_TcpEnvBase->PktsAcked(tcb, segmentsAcked, rtt);
    }
}
//通过这种方式，CongestionStateSet() 函数根据传入的 TcpSocketState 对象和新的状态来设置拥塞状态。它首先检查环境是否已经创建，如果没有，则创建环境。然后，它调用环境对象的 CongestionStateSet() 函数来设置拥塞状态，并返回新的慢启动阈值。
void
TcpRlBase::CongestionStateSet(Ptr<TcpSocketState> tcb,
                              const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this);

    if (!m_TcpEnvBase)
    {
        CreateGymEnv();
    }

    if (m_TcpEnvBase)
    {
        m_TcpEnvBase->CongestionStateSet(tcb, newState);
    }
}
//CwndEvent() 函数根据传入的 TcpSocketState 对象和事件类型来处理窗口事件。它首先检查环境是否已经创建，如果没有，则创建环境。然后，它调用环境对象的 CwndEvent() 函数来处理窗口事件，并返回新的慢启动阈值。
void
TcpRlBase::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this);

    if (!m_TcpEnvBase)
    {
        CreateGymEnv();
    }

    if (m_TcpEnvBase)
    {
        m_TcpEnvBase->CwndEvent(tcb, event);
    }
}
//Fork() 函数创建了一个新的 TcpRlBase 对象，该对象是当前对象的副本
Ptr<TcpCongestionOps>
TcpRlBase::Fork()
{
    return CopyObject<TcpRlBase>(this);
}

NS_OBJECT_ENSURE_REGISTERED(TcpRlTimeBased);

TypeId
TcpRlTimeBased::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpRlTimeBased")
                            .SetParent<TcpRlBase>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpRlTimeBased>();
    return tid;
}

TcpRlTimeBased::TcpRlTimeBased()
    : TcpRlBase()
{
    NS_LOG_FUNCTION(this);
}

TcpRlTimeBased::TcpRlTimeBased(const TcpRlTimeBased& sock)
    : TcpRlBase(sock)
{
    NS_LOG_FUNCTION(this);
}

TcpRlTimeBased::~TcpRlTimeBased()
{
}

std::string
TcpRlTimeBased::GetName() const
{
    return "TcpRlTimeBased"; //该函数返回一个字符串常量 "TcpRlTimeBased"，它表示当前类的名称。
}

void
TcpRlTimeBased::CreateGymEnv()
{
    NS_LOG_FUNCTION(this);
    Ptr<TcpTimeStepEnv> env = CreateObject<TcpTimeStepEnv>();//函数使用了 CreateObject<TcpTimeStepEnv>() 函数来创建一个指向 TcpTimeStepEnv 类对象的指针 env。TcpTimeStepEnv 可能是一个环境类，用于管理时间步长相关的操作。
    env->SetSocketUuid(TcpRlBase::GenerateUuid()); //函数调用了 env->SetSocketUuid() 方法来设置环境对象的 socket UUID。TcpRlBase::GenerateUuid() 函数可能用于生成唯一的 UUID。
    m_TcpEnvBase = env; //函数将 env 指针存储在成员变量 m_TcpEnvBase 中

    ConnectSocketCallbacks(); //并调用 ConnectSocketCallbacks() 函数来连接 socket 回调函数。
}

NS_OBJECT_ENSURE_REGISTERED(TcpRlEventBased);

TypeId
TcpRlEventBased::GetTypeId()
{
    static TypeId tid = TypeId("ns3::TcpRlEventBased")
                            .SetParent<TcpRlBase>()
                            .SetGroupName("Internet")
                            .AddConstructor<TcpRlEventBased>()
                            .AddAttribute("Reward", //"Reward" 属性表示增加拥塞窗口时的奖励值，其类型为 DoubleValue，默认值为 1.0，访问器为 MakeDoubleAccessor(&TcpRlEventBased::m_reward)，检查器为 MakeDoubleChecker<double>()。
                                          "Reward when increasing congestion window.",
                                          DoubleValue(1.0),
                                          MakeDoubleAccessor(&TcpRlEventBased::m_reward),
                                          MakeDoubleChecker<double>())
                            .AddAttribute("Penalty", //"Penalty" 属性表示发生丢包事件后的惩罚值，其类型为 DoubleValue，默认值为 -10.0，访问器为 MakeDoubleAccessor(&TcpRlEventBased::m_penalty)，检查器为 MakeDoubleChecker<double>()。
                                          "Penalty after a loss event.",
                                          DoubleValue(-10.0),
                                          MakeDoubleAccessor(&TcpRlEventBased::m_penalty),
                                          MakeDoubleChecker<double>());
    return tid;
}

TcpRlEventBased::TcpRlEventBased()
    : TcpRlBase()
{
    NS_LOG_FUNCTION(this);
}

TcpRlEventBased::TcpRlEventBased(const TcpRlEventBased& sock)
    : TcpRlBase(sock)
{
    NS_LOG_FUNCTION(this);
}

TcpRlEventBased::~TcpRlEventBased()
{
}

std::string
TcpRlEventBased::GetName() const
{
    return "TcpRlEventBased";
}
//该函数的主要目的是创建基于事件的 Gym 环境，并为其设置相应的参数（UUID、奖励、惩罚）。创建完成后，将 Gym 环境对象赋值给类成员变量 m_TcpEnvBase，并通过调用 ConnectSocketCallbacks 函数连接套接字的回调函数。
void
TcpRlEventBased::CreateGymEnv()
{
    NS_LOG_FUNCTION(this);
    Ptr<TcpEventBasedEnv> env = CreateObject<TcpEventBasedEnv>(); //创建一个基于事件的 Gym 环境对象 (TcpEventBasedEnv)。
    env->SetSocketUuid(TcpRlBase::GenerateUuid()); //为 Gym 环境设置套接字的 UUID，通过调用 TcpRlBase::GenerateUuid() 生成。
    env->SetReward(m_reward);   //为 Gym 环境设置奖励值，使用类成员变量 m_reward 的值。
    env->SetPenalty(m_penalty); //为 Gym 环境设置惩罚值，使用类成员变量 m_penalty 的值。
    m_TcpEnvBase = env;         //将创建的 Gym 环境对象赋值给类成员变量 

    ConnectSocketCallbacks();   //调用 ConnectSocketCallbacks 函数连接套接字的回调函数。
}

} // namespace ns3
