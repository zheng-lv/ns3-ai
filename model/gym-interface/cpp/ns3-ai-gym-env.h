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

#ifndef OPENGYM_ENV_H
#define OPENGYM_ENV_H

#include <ns3/object.h>

namespace ns3
{

class OpenGymSpace;
class OpenGymDataContainer;
class OpenGymInterface;

/**
 * \brief Base class to create Gymnasium-compatible environments in C++ side.定义了一个名为 OpenGymEnv 的类，作为创建在C++中与Gymnasium兼容的环境的基类。
 */
class OpenGymEnv : public Object
{
  public:
    OpenGymEnv();//构造函数 OpenGymEnv() 用于初始化对象的状态。
    ~OpenGymEnv() override;//用于清理对象的资源。

    // static TypeId GetTypeId();/GetTypeId 是用于获取类型标识的静态成员函数，用于识别类的类型。

    /**
     * Get action space (Box, Dict, ...) from simulation 获取动作空间，例如 Box、Dict 等，从仿真中获取。
     */
    virtual Ptr<OpenGymSpace> GetActionSpace() = 0;

    /**
     * Get observation space (Box, Dict, ...) from simulation 获取观察空间，例如 Box、Dict 等，从仿真中获取。
     */
    virtual Ptr<OpenGymSpace> GetObservationSpace() = 0;

    /**
     * Get whether game is over (simulation is finished).
     * Normally, this should always return false. 取游戏是否结束（仿真是否完成）的标志。通常应始终返回false。
     */
    virtual bool GetGameOver() = 0;

    /**
     * Get observation (stored in container) 获取存储在容器中的观察数据。
     */
    virtual Ptr<OpenGymDataContainer> GetObservation() = 0;

    /**
     * Get reward 获取奖励。
     */
    virtual float GetReward() = 0;

    /**
     * Get extra information 获取额外信息。
     */
    virtual std::string GetExtraInfo() = 0;

    /**
     * Execute actions. E.g., modify the contention window in TCP. 执行动作，例如修改TCP中的拥塞窗口。
     */
    virtual bool ExecuteActions(Ptr<OpenGymDataContainer> action) = 0;

    /**
     * Sets the lower level gym interface (shared memory)
     * associated to the environment 设置与环境关联的低级Gym接口（共享内存）
     */
    void SetOpenGymInterface(Ptr<OpenGymInterface> openGymInterface);

    /**
     * Notify Python side about the states, and execute the actions  通知Python端关于状态，并执行动作。这是一个重要的函数，用于将环境的状态和动作信息发送到Python端。。
     */
    void Notify();

    /**
     * Notify Python side that the simulation has ended. 通知Python端仿真已经结束。
     */
    void NotifySimulationEnd();

  protected:
    // Inherited 这两个函数用于初始化和处理资源。它们是从基类继承的虚拟函数。
    void DoInitialize() override;
    void DoDispose() override;

    Ptr<OpenGymInterface> m_openGymInterface; //m_openGymInterface 是指向 OpenGymInterface 对象的智能指针，用于与Python端通信。

  private:
};

} // end of namespace ns3

#endif /* OPENGYM_ENV_H */
