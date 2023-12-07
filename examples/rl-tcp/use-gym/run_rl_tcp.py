# Copyright (c) 2020-2023 Huazhong University of Science and Technology
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Author: Pengyu Liu <eic_lpy@hust.edu.cn>
#         Hao Yin <haoyin@uw.edu>
#         Muyuan Shen <muyuan_shen@hust.edu.cn>

import os
import torch
import argparse
import numpy as np
import matplotlib.pyplot as plt
from agents import TcpNewRenoAgent, TcpDeepQAgent, TcpQAgent
import ns3ai_gym_env
import gymnasium as gym
import sys
import traceback

#用于获取 TCP 代理对象
def get_agent(socketUuid, useRl): #get_agent 函数接受两个参数 socketUuid 和 useRl。
    agent = get_agent.tcpAgents.get(socketUuid) #在函数内部，首先尝试从 get_agent.tcpAgents 字典中获取具有给定 socketUuid 键的 TCP 代理对象。
    if agent is None:  #如果代理对象不存在（即 agent 为 None），则根据 useRl 参数和可能的其他参数创建一个新的 TCP 代理对象。
        if useRl:  #根据 useRl 的值，选择不同的 TCP 代理对象类型。如果 useRl 为 True，则创建 Q-learning 或 Deep Q-learning 代理对象；如果为 False，则创建 New Reno 代理对象。
            if args.rl_algo == 'DeepQ':  #如果 args.rl_algo 为 'DeepQ'，则创建一个 TcpDeepQAgent 对象；
                agent = TcpDeepQAgent()
                print("new Deep Q-learning agent, uuid = {}".format(socketUuid))
            else:
                agent = TcpQAgent()
                print("new Q-learning agent, uuid = {}".format(socketUuid))
        else:  #如果 useRl 为 False，函数会创建一个基于传统拥塞控制算法的 TcpNewRenoAgent 对象。
            agent = TcpNewRenoAgent()
            print("new New Reno agent, uuid = {}".format(socketUuid))
        get_agent.tcpAgents[socketUuid] = agent  #新创建的代理对象存储到 get_agent.tcpAgents 字典中，以备将来使用

    return agent  #返回获取到的或新创建的 TCP 代理对象。


# initialize variable
get_agent.tcpAgents = {}   #get_agent.tcpAgents 被初始化为空字典，用于存储 TCP 代理对象

parser = argparse.ArgumentParser()   #使用 argparse 模块创建了一个命令行参数解析器 parser。向解析器中添加了一些命令行参数，包括 seed、sim_seed、duration、show_log、result、result_dir、use_rl 和 rl_algo。
parser.add_argument('--seed', type=int,
                    help='set seed for reproducibility') #用于设置随机数生成器的种子，以确保每次运行时的结果具有可重复性。
parser.add_argument('--sim_seed', type=int,
                    help='set simulation run number') #用于设置模拟运行的编号，以便区分不同的模拟运行。
parser.add_argument('--duration', type=float,
                    help='set simulation duration (seconds)') #用于设置模拟的持续时间（以秒为单位）。
parser.add_argument('--show_log', action='store_true',
                    help='whether show observation and action') #用于指定是否显示观察和动作。
parser.add_argument('--result', action='store_true',
                    help='whether output figures') #用于指定是否输出图形结果。
parser.add_argument('--result_dir', type=str,
                    default='./rl_tcp_results', help='output figures path') #用于指定输出图形结果的路径。
parser.add_argument('--use_rl', action='store_true',
                    help='whether use rl algorithm') #用于指定是否使用强化学习算法。
parser.add_argument('--rl_algo', type=str,
                    default='DeepQ', help='RL Algorithm, Q or DeepQ') #用于指定强化学习算法的类型，默认为 DeepQ。

args = parser.parse_args() #使用 parser.parse_args() 解析命令行参数，并将结果存储在 args 变量中。

#用于设置随机数种子和模拟运行的编号，以及处理强化学习算法和结果输出的相关参数。
my_seed = 42  
if args.seed is not None:
    my_seed = args.seed    #程序通过 args.seed 参数获取用户指定的随机数种子，如果未指定，则使用默认值 42。
print("Python side random seed {}".format(my_seed))
np.random.seed(my_seed)   #然后，使用 np.random.seed() 和 torch.manual_seed() 函数分别为 numpy 和 PyTorch 库设置随机数种子，以确保每次运行时的结果具有可重复性。
torch.manual_seed(my_seed)

my_sim_seed = 0    #程序通过 args.sim_seed 参数获取用户指定的模拟运行编号，如果未指定，则使用默认值 0
if args.sim_seed:
    my_sim_seed = args.sim_seed

my_duration = 1000  #程序通过 args.duration 参数获取用户指定的模拟持续时间，如果未指定，则使用默认值 1000 秒。
if args.duration:
    my_duration = args.duration

if args.use_rl:   #如果 args.use_rl 为 True，则根据 args.rl_algo 参数选择强化学习算法。如果 args.rl_algo 不是 'Q' 或 'DeepQ'，则输出错误信息并退出程序。
    if (args.rl_algo != 'Q') and (args.rl_algo != 'DeepQ'):
        print("Invalid RL Algorithm {}".format(args.rl_algo))
        exit(1)

res_list = ['ssThresh_l', 'cWnd_l', 'segmentsAcked_l',
            'segmentSize_l', 'bytesInFlight_l']
if args.result:   #args.result 为 True，则为 res_list 中的每个结果变量（如 ssThresh_l、cWnd_l、segmentsAcked_l、segmentSize_l 和 bytesInFlight_l）创建一个空列表，用于存储相应的结果。
    for res in res_list:
        globals()[res] = []

#目的是创建一个名为 env 的 OpenAI Gym 环境，该环境使用了自定义的 ns3ai_gym_env 插件。具体步骤如下
stepIdx = 0 #设置了一个名为 stepIdx 的变量，初始值为 0。
#定义了一个名为 ns3Settings 的字典，包含了一些与 ns-3 模拟相关的设置，如传输协议（'TcpRlTimeBased'）、模拟时长（'duration'）和模拟运行编号（'simSeed'）。
ns3Settings = {
    'transport_prot': 'TcpRlTimeBased',
    'duration': my_duration,
    'simSeed': my_sim_seed}
#使用 gym.make 创建了一个名为 env 的 OpenAI Gym 环境，使用了自定义的 ns3ai_gym_env 插件。传递给环境的参数包括 targetName、ns3Path 和 ns3Settings。其中，targetName 是 ns3ai_rltcp_gym，ns3Path 是 "../../../../../"，而 ns3Settings 则是上一步中定义的字典。
env = gym.make("ns3ai_gym_env/Ns3-v0", targetName="ns3ai_rltcp_gym",
               ns3Path="../../../../../", ns3Settings=ns3Settings)
#打印了观察空间（ob_space）和动作空间（ac_space）的信息，包括它们的类型（dtype）。
ob_space = env.observation_space
ac_space = env.action_space
print("Observation space: ", ob_space, ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)

try:
    obs, info = env.reset()  #环境重置： 使用 env.reset() 重置环境，并获取初始观察值 obs、信息 info。
    reward = 0   #初始化变量： 初始化奖励 reward 为0，done 标志为False。
    done = False

    # get existing agent or create new TCP agent if needed 获取或创建代理： 使用 get_agent 函数获取现有的代理或创建新的 TCP 代理，具体取决于是否使用强化学习。
    tcpAgent = get_agent(obs[0], args.use_rl)

    while True: #主循环： 在一个无限循环中，代理根据当前观察值选择动作，并将其应用于环境。随后，观察新状态、奖励和完成标志。
        #接下来，程序通过一个循环不断执行以下操作：
            #获取当前观测值，并打印出来。
            #根据强化学习算法选择一个动作，并打印出来。
            #执行动作，并获取下一个观测值、奖励和结束标志。
            #如果结束标志为 True，则退出循环。
        # current ssThreshold 示拥塞窗口的阈值（Slow Start Threshold）。
        ssThresh = obs[4]
        # current contention window size 当前的拥塞窗口大小（Congestion Window）。
        cWnd = obs[5]
        # segment size 表示每个数据段的大小。
        segmentSize = obs[6]
        # number of acked segments 表示已经确认的分段数量。
        segmentsAcked = obs[9]
        # estimated bytes in flight 表示估计的飞行中的字节数量。
        bytesInFlight = obs[7]

        cur_obs = [ssThresh, cWnd, segmentsAcked, segmentSize, bytesInFlight]   #这个列表可能是在一个强化学习算法中用于表示当前的观测状态，这些元素分别表示拥塞窗口的阈值（Slow Start Threshold）、当前的拥塞窗口大小（Congestion Window）、已经确认的分段数量、每个数据段的大小以及估计的飞行中的字节数量。
        if args.show_log:  #记录观察值和动作（if args.show_log:）：如果 args.show_log 为 True，则打印接收到的观察值 cur_obs。这可以用于调试和监控环境的运行情况。
            print("Recv obs:", cur_obs)

        if args.result:  #记录结果（if args.result:）：如果 args.result 为 True，则记录结果。对于 res_list 中的每个结果变量 res，将其添加到相应的全局变量列表中。例如，如果结果变量为 ssThresh_l，则将其添加到 globals()['ssThresh_l'] 列表中。这可以用于分析和可视化学习过程中的结果变化。
            for res in res_list:
                globals()[res].append(globals()[res[:-2]])

        action = tcpAgent.get_action(obs, reward, done, info)  #获取动作：使用 tcpAgent.get_action() 方法根据当前的观察值 obs、奖励 reward、结束标志 done 和其他信息 info 来选择动作。这个方法是由强化学习代理 tcpAgent 实现的，它根据强化学习算法和策略来决定下一步的动作。

        if args.show_log:    #打印步骤信息（if args.show_log:）：如果 args.show_log 为 True，则打印当前的步骤索引 stepIdx，并将其增加 1。然后打印发送的动作 action。
            print("Step:", stepIdx)
            stepIdx += 1
            print("Send act:", action)

        obs, reward, done, _, info = env.step(action)  #执行环境步骤（obs, reward, done, _, info = env.step(action)）：根据发送的动作 action，执行环境的一步操作，并获取观测值 obs、奖励 reward、结束标志 done、其他信息 _ 和额外信息 info。

        if done:    #检查是否结束（if done:）：如果结束标志 done 为 True，则打印 "Simulation ended"，并退出循环。
            print("Simulation ended")
            break

        # get existing agent of create new TCP agent if needed 获取或创建 TCP 代理（tcpAgent = get_agent(obs[0], args.use_rl)）：根据观测值 obs[0] 和参数 args.use_rl，获取现有的 TCP 代理（如果已经存在），或者创建一个新的 TCP 代理（如果需要使用强化学习）。这个 get_agent() 函数的实现细节取决于具体的强化学习框架和代码结构。
        tcpAgent = get_agent(obs[0], args.use_rl)

except Exception as e:
    exc_type, exc_value, exc_traceback = sys.exc_info()
    print("Exception occurred: {}".format(e))
    print("Traceback:")
    traceback.print_tb(exc_traceback)
    exit(1)

else:  #绘制图表（可选）： 如果指定了 args.result 和 args.result_dir，则将结果绘制成图表并保存在指定的目录中。
    if args.result:
        if args.result_dir:
            if not os.path.exists(args.result_dir): 
                os.mkdir(args.result_dir)    #创建结果目录（if args.result_dir:）：如果指定了结果目录参数 args.result_dir，但该目录不存在，则使用 os.mkdir() 函数创建该目录。
        for res in res_list:  #遍历结果列表（for res in res_list:）：对于每个结果变量 res，获取其全局变量列表 globals()[res]。
            y = globals()[res]
            x = range(len(y))   #获取 x 和 y 轴数据（x = range(len(y))）：使用 range() 函数生成一个包含 y 轴数据长度的数字序列，并将其赋值给 x 轴。
            plt.clf()
            plt.plot(x, y, label=res[:-2], linewidth=1, color='r')  #绘制图表（plt.plot(x, y, label=res[:-2], linewidth=1, color='r')）：使用 plt.plot() 函数绘制 x 轴和 y 轴的数据，并设置标签为结果变量的前两个部分（res[:-2]），线宽为 1，颜色为红色。
            plt.xlabel('Step Number')
            plt.title('Information of {}'.format(res[:-2]))  #设置 x 轴和图表标题（plt.xlabel('Step Number'), plt.title('Information of {}'.format(res[:-2]))）：使用 plt.xlabel() 和 plt.title() 函数分别设置 x 轴标签和图表标题，其中标题为结果变量的前两个部分（res[:-2]）。
            plt.savefig('{}.png'.format(os.path.join(args.result_dir, res[:-2])))  #保存图表（plt.savefig('{}.png'.format(os.path.join(args.result_dir, res[:-2])))）：使用 plt.savefig() 函数将绘制的图表保存为 PNG 格式的图像文件，并使用 os.path.join() 函数将结果目录和结果变量的前两个部分拼接在一起作为文件名。
finally: 
    print("Finally exiting...")
    env.close()   #环境关闭： 最终，通过调用 env.close() 关闭环境。
