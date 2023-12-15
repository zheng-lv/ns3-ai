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
 *这个类的主要功能是提供与 OpenAI Gym 接口相关的方法和回调函数，用于初始化仿真环境、获取状态信息、执行动作等。是与 OpenAI Gym 接口相关的一个类
 *各个成员函数和回调函数的具体实现需要查看相应的定义和实现文件。
 */

#ifndef NS3_NS3_AI_GYM_INTERFACE_H
#define NS3_NS3_AI_GYM_INTERFACE_H

#include "../ns3-ai-gym-msg.h"

#include <ns3/ai-module.h>
#include <ns3/callback.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/type-id.h>

namespace ns3
{
// OpenGymSpace 和 OpenGymDataContainer 类的前置声明
class OpenGymSpace;
class OpenGymDataContainer;
class OpenGymEnv;
// OpenGymInterface 类的定义，继承自 ns3 的 Object 类
class OpenGymInterface : public Object
{
  public:
    static Ptr<OpenGymInterface> Get();// 获取 OpenGymInterface 对象的静态方法
    OpenGymInterface();// 构造函数
    ~OpenGymInterface() override;// 析构函数
    static TypeId GetTypeId();// 获取类型标识

    void Init();// 初始化接口
    void NotifyCurrentState();// 通知当前状态
    void WaitForStop();// 等待停止
    void NotifySimulationEnd();// 通知仿真结束

    Ptr<OpenGymSpace> GetActionSpace();// 获取动作空间
    Ptr<OpenGymSpace> GetObservationSpace();// 获取观察空间
    Ptr<OpenGymDataContainer> GetObservation();// 获取观察数据
    float GetReward();// 获取奖励
    bool IsGameOver();// 游戏是否结束
    std::string GetExtraInfo(); // 获取额外信息
    bool ExecuteActions(Ptr<OpenGymDataContainer> action);// 执行动作

    // 设置回调函数
    void SetGetActionSpaceCb(Callback<Ptr<OpenGymSpace>> cb);
    void SetGetObservationSpaceCb(Callback<Ptr<OpenGymSpace>> cb);
    void SetGetObservationCb(Callback<Ptr<OpenGymDataContainer>> cb);
    void SetGetRewardCb(Callback<float> cb);
    void SetGetGameOverCb(Callback<bool> cb);
    void SetGetExtraInfoCb(Callback<std::string> cb);
    void SetExecuteActionsCb(Callback<bool, Ptr<OpenGymDataContainer>> cb);
    // 通知实体状态改变
    void Notify(Ptr<OpenGymEnv> entity);

  protected:
    // Inherited 
    void DoInitialize() override;// 初始化
    void DoDispose() override;// 释放资源

  private:
    static Ptr<OpenGymInterface>* DoGet();// 静态方法，获取 OpenGymInterface 对象的指针
    //    static void Delete();

    bool m_simEnd;// 成员变量，标记仿真是否结束
    bool m_stopEnvRequested;// 成员变量，标记停止仿真的请求
    bool m_initSimMsgSent;// 成员变量，标记仿真消息是否已发送

    Callback<Ptr<OpenGymSpace>> m_actionSpaceCb;// 回调函数，用于获取动作空间
    Callback<Ptr<OpenGymSpace>> m_observationSpaceCb;// 回调函数，用于获取观察空间
    Callback<bool> m_gameOverCb;// 回调函数，用于获取游戏结束标志
    Callback<Ptr<OpenGymDataContainer>> m_obsCb;// 回调函数，用于获取观察数据
    Callback<float> m_rewardCb;// 回调函数，用于获取奖励
    Callback<std::string> m_extraInfoCb;// 回调函数，用于获取额外信息
    Callback<bool, Ptr<OpenGymDataContainer>> m_actionCb;// 回调函数，用于执行动作
};

} // end of namespace ns3
// 头文件结束标志
#endif // NS3_NS3_AI_GYM_INTERFACE_H
