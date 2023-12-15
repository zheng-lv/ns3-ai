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

#include "ns3-ai-gym-env.h"

#include "ns3-ai-gym-interface.h"

#include <ns3/log.h>
#include <ns3/object.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(OpenGymEnv);
NS_LOG_COMPONENT_DEFINE("OpenGymEnv");

OpenGymEnv::OpenGymEnv()
{
    NS_LOG_FUNCTION(this);
}

OpenGymEnv::~OpenGymEnv()
{
    NS_LOG_FUNCTION(this);
}

TypeId
OpenGymEnv::GetTypeId()
{
    static TypeId tid = TypeId("ns3::OpenGymEnv").SetParent<Object>().SetGroupName("OpenGym");
    return tid;
}
/*将 OpenGymEnv 类的成员函数与 Python 端的 OpenGymInterface 进行关联，以实现在 Python 端请求环境信息时的回调。
*这种关联使得在Python端请求相关信息时，能够调用 OpenGymEnv 中定义的对应成员函数，从而实现与Python端的通信。
*/
void
OpenGymEnv::SetOpenGymInterface(Ptr<OpenGymInterface> openGymInterface)
{
    NS_LOG_FUNCTION(this);//这是一个NS-3网络模拟器中的日志宏，用于记录函数调用。在这里，它用于记录 SetOpenGymInterface 函数的调用。
    m_openGymInterface = openGymInterface;//将成员变量 m_openGymInterface 设置为传入的 openGymInterface 智能指针。这个指针用于与Python端通信。
    //使用 MakeCallback 函数为 openGymInterface 设置各种回调函数，以便在 Python 端请求相关信息时，能够调用 OpenGymEnv 对应的成员函数。
    openGymInterface->SetGetActionSpaceCb(MakeCallback(&OpenGymEnv::GetActionSpace, this));
    openGymInterface->SetGetObservationSpaceCb(
        MakeCallback(&OpenGymEnv::GetObservationSpace, this));
    openGymInterface->SetGetGameOverCb(MakeCallback(&OpenGymEnv::GetGameOver, this));
    openGymInterface->SetGetObservationCb(MakeCallback(&OpenGymEnv::GetObservation, this));
    openGymInterface->SetGetRewardCb(MakeCallback(&OpenGymEnv::GetReward, this));
    openGymInterface->SetGetExtraInfoCb(MakeCallback(&OpenGymEnv::GetExtraInfo, this));
    openGymInterface->SetExecuteActionsCb(MakeCallback(&OpenGymEnv::ExecuteActions, this));
}

void
OpenGymEnv::Notify()
{
    NS_LOG_FUNCTION(this);
    if (m_openGymInterface)
    {
        m_openGymInterface->Notify(this);
    }
}

void
OpenGymEnv::NotifySimulationEnd()
{
    NS_LOG_FUNCTION(this);
    if (m_openGymInterface)
    {
        m_openGymInterface->NotifySimulationEnd();
    }
}

void
OpenGymEnv::DoInitialize()
{
    NS_LOG_FUNCTION(this);
}

void
OpenGymEnv::DoDispose()
{
    NS_LOG_FUNCTION(this);
}

} // namespace ns3
