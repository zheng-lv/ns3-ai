/*
 * Copyright (c) 2018 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 * Modify: Muyuan Shen <muyuan_shen@hust.edu.cn>
 *
 */

/*
 * Note: The Gym interface class is only for C++ side. Do not create Python binding
 *       for this interface.
 */

#include "ns3-ai-gym-interface.h"

#include "container.h"
#include "messages.pb.h"
#include "ns3-ai-gym-env.h"
#include "spaces.h"

#include <ns3/config.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("OpenGymInterface");
NS_OBJECT_ENSURE_REGISTERED(OpenGymInterface);

Ptr<OpenGymInterface>
OpenGymInterface::Get()
{
    NS_LOG_FUNCTION_NOARGS();
    return *DoGet();
}

OpenGymInterface::OpenGymInterface()
    : m_simEnd(false),
      m_stopEnvRequested(false),
      m_initSimMsgSent(false)
{
    auto interface = Ns3AiMsgInterface::Get();
    interface->SetIsMemoryCreator(false);
    interface->SetUseVector(false);
    interface->SetHandleFinish(false);
}

OpenGymInterface::~OpenGymInterface()
{
}

TypeId
OpenGymInterface::GetTypeId()
{
    static TypeId tid = TypeId("OpenGymInterface")
                            .SetParent<Object>()
                            .SetGroupName("OpenGym")
                            .AddConstructor<OpenGymInterface>();
    return tid;
}
/*这段代码的功能是初始化 OpenAI Gym 环境。具体注释如下：

初始化 OpenAI Gym 环境。
如果初始化消息已发送，则直接返回，避免重复发送。
获取观测空间和动作空间。
创建用于初始化的消息 simInitMsg。
如果存在观测空间，将其描述信息添加到初始化消息中。
如果存在动作空间，将其描述信息添加到初始化消息中。
获取消息传递接口。
向 Python 发送初始化消息。
从 Python 接收初始化应答消息。
解析初始化应答消息中的标志，如是否完成初始化（done），是否有停止模拟的请求（stopSim）。
如果有停止模拟的请求，则设置停止环境的标志，停止模拟器，并退出程序。
*/
void
OpenGymInterface::Init()
{
    // do not send init msg twice // 避免重复发送初始化消息
    if (m_initSimMsgSent)
    {
        return;
    }
    m_initSimMsgSent = true;

    // 获取观测空间和动作空间
    Ptr<OpenGymSpace> obsSpace = GetObservationSpace();
    Ptr<OpenGymSpace> actionSpace = GetActionSpace();

     // 创建用于初始化的消息，这一行代码是为了创建一个用于构建初始化消息的对象，以便后续代码可以向其中添加相关信息。
    /*这个对象的声明并不仅仅是为了创建一个变量，更重要的是为后续的代码提供一个容器，用于构建并存储将要发送到 Python 端的初始化消息。
    在初始化过程中，会向这个消息对象中添加关于观测空间和动作空间的描述信息，然后将其序列化为字节数组，最终通过消息传递接口发送到 Python 端。*/
    ns3_ai_gym::SimInitMsg simInitMsg;
    // 如果存在观测空间，将其描述信息添加到初始化消息中
    if (obsSpace)
    {
        ns3_ai_gym::SpaceDescription spaceDesc;
        spaceDesc = obsSpace->GetSpaceDescription();
        simInitMsg.mutable_obsspace()->CopyFrom(spaceDesc);
    }
    // 如果存在动作空间，将其描述信息添加到初始化消息中
    if (actionSpace)
    {
        ns3_ai_gym::SpaceDescription spaceDesc;
        spaceDesc = actionSpace->GetSpaceDescription();
        simInitMsg.mutable_actspace()->CopyFrom(spaceDesc);
    }

    // get the interface  这段代码获取了用于处理特定消息类型的消息接口
    Ns3AiMsgInterfaceImpl<Ns3AiGymMsg, Ns3AiGymMsg>* msgInterface =
        Ns3AiMsgInterface::Get()->GetInterface<Ns3AiGymMsg, Ns3AiGymMsg>();

    // send init msg to python // 向 Python 发送初始化消息
    msgInterface->CppSendBegin();//：开始 C++ 到 Python 的消息发送。这表明接下来的操作将把数据发送给 Python。
    msgInterface->GetCpp2PyStruct()->size = simInitMsg.ByteSizeLong();//获取用于 C++ 到 Python 传输的结构体，并设置其大小为 simInitMsg 消息的字节大小。这个结构体存储了要发送到 Python 的数据的相关信息。
    assert(msgInterface->GetCpp2PyStruct()->size <= MSG_BUFFER_SIZE);//断言确保消息的大小不超过预定义的缓冲区大小（MSG_BUFFER_SIZE）。如果超过了，将触发断言失败。
    simInitMsg.SerializeToArray(msgInterface->GetCpp2PyStruct()->buffer,
                                msgInterface->GetCpp2PyStruct()->size);//将 simInitMsg 消息序列化为字节数组，并将其存储在结构体的缓冲区中。这样，消息的内容就准备好发送给 Python。
    msgInterface->CppSendEnd();//结束 C++ 到 Python 的消息发送。这表明消息已准备好发送给 Python，可以执行发送操作。

    // receive init ack msg from python // 从 Python 接收初始化应答消息
    ns3_ai_gym::SimInitAck simInitAck; //GetPy2CppStruct() 获取用于 Python 到 C++ 传输的结构体，ParseFromArray 从字节数组中解析消息内容。
    msgInterface->CppRecvBegin();
    simInitAck.ParseFromArray(msgInterface->GetPy2CppStruct()->buffer, //从接收到的 Python 消息结构体中解析 SimInitAck 消息。
                              msgInterface->GetPy2CppStruct()->size);//GetPy2CppStruct() 获取用于 Python 到 C++ 传输的结构体，ParseFromArray 从字节数组中解析消息内容。
    msgInterface->CppRecvEnd();//束 C++ 接收 Python 消息。这表明消息已成功接收，并完成了接收操作。

    // 解析初始化应答消息
    bool done = simInitAck.done();
    NS_LOG_DEBUG("Sim Init Ack: " << done);
    // 检查是否有停止模拟的请求
    bool stopSim = simInitAck.stopsimreq();
    if (stopSim) // 设置停止环境的标志，停止模拟器，并退出程序
    {
        NS_LOG_DEBUG("---Stop requested: " << stopSim);
        m_stopEnvRequested = true;
        Simulator::Stop();
        Simulator::Destroy();
        std::exit(0);
    }
}
/*
该类是一个名为 OpenGymInterface 的 C++ 类，实现了与 Python 中的 Gym 环境进行交互的功能。具体来说，它的主要职责包括：
初始化和环境状态通知： 通过 Init() 方法进行初始化，包括向 Python 发送初始化消息，并在 NotifyCurrentState() 方法中通知 Python 当前环境的状态。
消息传输： 通过 Ns3AiMsgInterface 实现消息的传输，包括发送环境状态消息和接收动作消息。
动作执行： 根据从 Python 环境接收到的动作消息，调用 ExecuteActions 方法执行相应的动作。
停止模拟： 在接收到停止模拟的请求时，终止模拟并退出程序。
其他功能： 包括获取观察数据、奖励值、判断游戏是否结束等。
总体而言，OpenGymInterface 类在 NS-3 模拟环境中充当了一个与 Python 中 Gym 环境进行通信的接口，负责管理模拟的初始化、状态通知和动作执行等任务。
*/
void
OpenGymInterface::NotifyCurrentState()
{
    // 如果初始化消息未发送，则进行初始化
    if (!m_initSimMsgSent)
    {
        Init();
    }
    // 如果停止环境的请求已经发出，则直接返回
    if (m_stopEnvRequested)
    {
        return;
    }
    // collect current env state 收集当前环境状态信息
    Ptr<OpenGymDataContainer> obsDataContainer = GetObservation();// 获取观察数据
    float reward = GetReward();// 获取奖励值
    bool isGameOver = IsGameOver();// 判断游戏是否结束
    std::string extraInfo = GetExtraInfo();// 获取额外信息
    ns3_ai_gym::EnvStateMsg envStateMsg;  // 创建 EnvStateMsg 消息
    // observation // 处理观察数据
    ns3_ai_gym::DataContainer obsDataContainerPbMsg;
    if (obsDataContainer) //这是一个条件语句，检查是否存在观察数据容器 obsDataContainer。这个容器应该包含当前环境的观察数据。
    {
        obsDataContainerPbMsg = obsDataContainer->GetDataContainerPbMsg();   //如果观察数据容器存在，调用其 GetDataContainerPbMsg 方法，将观察数据转换为 Protocol Buffers 消息类型 DataContainer，并将结果赋给 obsDataContainerPbMsg。
        envStateMsg.mutable_obsdata()->CopyFrom(obsDataContainerPbMsg);//接着，通过 mutable_obsdata() 获取 envStateMsg 对象中观察数据的可变引用，并使用 CopyFrom 方法将 obsDataContainerPbMsg 的内容复制到其中。这样就将观察数据添加到了 envStateMsg 中。
    }
    // 设置奖励值、游戏结束标志和额外信息
    // reward
    envStateMsg.set_reward(reward);
    // game over
    envStateMsg.set_isgameover(false); //设置游戏是否结束的标志为 false，因为下面的条件语句将判断游戏是否结束。
    if (isGameOver) //检查游戏是否结束。如果结束，执行以下操作：
    {
        envStateMsg.set_isgameover(true); //将环境状态消息中的游戏结束标志设置为 true。
        if (m_simEnd)
        {
            envStateMsg.set_reason(ns3_ai_gym::EnvStateMsg::SimulationEnd); //如果是由于模拟结束而导致的游戏结束，则将结束原因设置为 SimulationEnd，否则设置为 GameOver。
        }
        else
        {
            envStateMsg.set_reason(ns3_ai_gym::EnvStateMsg::GameOver);
        }
    }
    // extra info 将额外信息 extraInfo 添加到环境状态消息中。
    envStateMsg.set_info(extraInfo);

    // get the interface  // 获取消息传输接口
    Ns3AiMsgInterfaceImpl<Ns3AiGymMsg, Ns3AiGymMsg>* msgInterface =
        Ns3AiMsgInterface::Get()->GetInterface<Ns3AiGymMsg, Ns3AiGymMsg>();

    // send env state msg to python // 向 Python 发送环境状态消息
    msgInterface->CppSendBegin(); //开始向 Python 发送消息。
    msgInterface->GetCpp2PyStruct()->size = envStateMsg.ByteSizeLong(); //获取用于存储消息大小的缓冲区的大小。
    assert(msgInterface->GetCpp2PyStruct()->size <= MSG_BUFFER_SIZE);
    envStateMsg.SerializeToArray(msgInterface->GetCpp2PyStruct()->buffer,
                                 msgInterface->GetCpp2PyStruct()->size);//将 envStateMsg 序列化为字节数组，并将其拷贝到消息缓冲区中。

    msgInterface->CppSendEnd();//结束消息发送。

    // receive act msg from python // 从 Python 接收动作消息
    ns3_ai_gym::EnvActMsg envActMsg;//解析 Python 发送的消息，将其反序列化为 envActMsg 对象。
    msgInterface->CppRecvBegin();

    envActMsg.ParseFromArray(msgInterface->GetPy2CppStruct()->buffer,
                             msgInterface->GetPy2CppStruct()->size);
    msgInterface->CppRecvEnd();//结束接收消息。

    if (m_simEnd) // 如果模拟结束，则只接收消息并退出
    {
        // if sim end only rx msg and quit
        return;
    }

    // 判断是否有停止模拟的请求
    bool stopSim = envActMsg.stopsimreq();
    if (stopSim) //如果有停止模拟的请求，执行相应的处理：
    {
        NS_LOG_DEBUG("---Stop requested: " << stopSim); 
        m_stopEnvRequested = true;//设置停止环境的标志。
        Simulator::Stop();//停止模拟。
        Simulator::Destroy();//销毁模拟器。
        std::exit(0);//退出程序。
    }

    // first step after reset is called without actions, just to get current state // 在重置后的第一步中，如果没有动作被调用，仅用于获取当前状态
    ns3_ai_gym::DataContainer actDataContainerPbMsg = envActMsg.actdata(); //获取 Python 发送的动作消息中的动作数据。
    Ptr<OpenGymDataContainer> actDataContainer =
        OpenGymDataContainer::CreateFromDataContainerPbMsg(actDataContainerPbMsg);//根据动作数据创建动作数据容器。
    ExecuteActions(actDataContainer);//执行环境中的动作。
}

void
OpenGymInterface::WaitForStop()
{
    NS_LOG_FUNCTION(this);
    //    NS_LOG_UNCOND("Wait for stop message");
    NotifyCurrentState();
}

void
OpenGymInterface::NotifySimulationEnd()
{
    NS_LOG_FUNCTION(this);
    m_simEnd = true;
    if (m_initSimMsgSent)
    {
        WaitForStop();
    }
}

Ptr<OpenGymSpace>
OpenGymInterface::GetActionSpace()
{
    NS_LOG_FUNCTION(this);
    Ptr<OpenGymSpace> actionSpace;
    if (!m_actionSpaceCb.IsNull())
    {
        actionSpace = m_actionSpaceCb();
    }
    return actionSpace;
}

Ptr<OpenGymSpace>
OpenGymInterface::GetObservationSpace()
{
    NS_LOG_FUNCTION(this);
    Ptr<OpenGymSpace> obsSpace;
    if (!m_observationSpaceCb.IsNull())
    {
        obsSpace = m_observationSpaceCb();
    }
    return obsSpace;
}

Ptr<OpenGymDataContainer>
OpenGymInterface::GetObservation()
{
    NS_LOG_FUNCTION(this);
    Ptr<OpenGymDataContainer> obs;
    if (!m_obsCb.IsNull())
    {
        obs = m_obsCb();
    }
    return obs;
}

float
OpenGymInterface::GetReward()
{
    NS_LOG_FUNCTION(this);
    float reward = 0.0;
    if (!m_rewardCb.IsNull())
    {
        reward = m_rewardCb();
    }
    return reward;
}

bool
OpenGymInterface::IsGameOver()
{
    NS_LOG_FUNCTION(this);
    bool gameOver = false;
    if (!m_gameOverCb.IsNull())
    {
        gameOver = m_gameOverCb();
    }
    return (gameOver || m_simEnd);
}

std::string
OpenGymInterface::GetExtraInfo()
{
    NS_LOG_FUNCTION(this);
    std::string info;
    if (!m_extraInfoCb.IsNull())
    {
        info = m_extraInfoCb();
    }
    return info;
}

bool
OpenGymInterface::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    NS_LOG_FUNCTION(this);
    bool reply = false;
    if (!m_actionCb.IsNull())
    {
        reply = m_actionCb(action);
    }
    return reply;
}

void
OpenGymInterface::SetGetActionSpaceCb(Callback<Ptr<OpenGymSpace>> cb)
{
    m_actionSpaceCb = cb;
}

void
OpenGymInterface::SetGetObservationSpaceCb(Callback<Ptr<OpenGymSpace>> cb)
{
    m_observationSpaceCb = cb;
}

void
OpenGymInterface::SetGetGameOverCb(Callback<bool> cb)
{
    m_gameOverCb = cb;
}

void
OpenGymInterface::SetGetObservationCb(Callback<Ptr<OpenGymDataContainer>> cb)
{
    m_obsCb = cb;
}

void
OpenGymInterface::SetGetRewardCb(Callback<float> cb)
{
    m_rewardCb = cb;
}

void
OpenGymInterface::SetGetExtraInfoCb(Callback<std::string> cb)
{
    m_extraInfoCb = cb;
}

void
OpenGymInterface::SetExecuteActionsCb(Callback<bool, Ptr<OpenGymDataContainer>> cb)
{
    m_actionCb = cb;
}

void
OpenGymInterface::DoInitialize()
{
    NS_LOG_FUNCTION(this);
}

void
OpenGymInterface::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

void
OpenGymInterface::Notify(Ptr<OpenGymEnv> entity)
{
    NS_LOG_FUNCTION(this);

    SetGetGameOverCb(MakeCallback(&OpenGymEnv::GetGameOver, entity));
    SetGetObservationCb(MakeCallback(&OpenGymEnv::GetObservation, entity));
    SetGetRewardCb(MakeCallback(&OpenGymEnv::GetReward, entity));
    SetGetExtraInfoCb(MakeCallback(&OpenGymEnv::GetExtraInfo, entity));
    SetExecuteActionsCb(MakeCallback(&OpenGymEnv::ExecuteActions, entity));

    NotifyCurrentState();
}

Ptr<OpenGymInterface>*
OpenGymInterface::DoGet()
{
    static Ptr<OpenGymInterface> ptr = CreateObject<OpenGymInterface>();
    return &ptr;
}

} // namespace ns3
