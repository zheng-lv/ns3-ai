# Copyright (c) 2023 Huazhong University of Science and Technology
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
# Author: Muyuan Shen <muyuan_shen@hust.edu.cn>


import torch
import numpy as np
import torch.nn as nn
import random


class net(nn.Module):
    def __init__(self):
        super(net, self).__init__()
        self.layers = nn.Sequential(
            nn.Linear(5, 20),
            nn.ReLU(),
            nn.Linear(20, 20),
            nn.ReLU(),
            nn.Linear(20, 4),
        )

    def forward(self, x):
        return self.layers(x)


class DQN(object):
    def __init__(self):
        self.eval_net = net() #eval_net 用于计算当前 Q 值
        self.target_net = net() #其中 target_net 用于计算目标 Q 值，
        self.learn_step = 0 #learn_step 跟踪学习步数，当它达到 target_replace 时，将 target_net 更新为 eval_net。
        self.batchsize = 32 #每次学习时的样本批次大小。
        self.observer_shape = 5 #观察状态的维度。
        self.target_replace = 100
        self.memory_counter = 0 #跟踪记忆库中的存储数量。
        self.memory_capacity = 2000 #记忆库的容量
        self.memory = np.zeros((2000, 2 * 5 + 2))  # s, a, r, s' 存储状态、动作、奖励和下一个状态的记忆库
        self.optimizer = torch.optim.Adam(
            self.eval_net.parameters(), lr=0.0001) #优化器，用于更新神经网络的权重。
        self.loss_func = nn.MSELoss() #用于计算损失的损失函数，这里使用均方误差损失。

    def choose_action(self, x): #根据当前状态选择动作。有一定的概率（0.99 ** self.memory_counter）选择当前估计的最佳动作，否则进行随机探索。
        x = torch.Tensor(x)
        if np.random.uniform() > 0.99 ** self.memory_counter:  # choose best
            action = self.eval_net.forward(x)
            action = torch.argmax(action, 0).numpy()
        else:  # explore
            action = np.random.randint(0, 4)
        return action

    def store_transition(self, s, a, r, s_): #存储状态、动作、奖励和下一个状态到记忆库中。使用循环队列的方式进行存储，以便更好地利用有限的记忆容量。
        index = self.memory_counter % self.memory_capacity
        self.memory[index, :] = np.hstack((s, [a, r], s_))
        self.memory_counter += 1

    def learn(self, ): #执行深度 Q 学习的学习步骤
        self.learn_step += 1
        if self.learn_step % self.target_replace == 0:  #更新 target_net 的频率由 target_replace 决定。
            self.target_net.load_state_dict(self.eval_net.state_dict())
        sample_list = np.random.choice(self.memory_capacity, self.batchsize) #从记忆库中随机采样样本
        # choose a mini batch
        sample = self.memory[sample_list, :]
        s = torch.Tensor(sample[:, :self.observer_shape])
        a = torch.LongTensor(
            sample[:, self.observer_shape:self.observer_shape + 1])
        r = torch.Tensor(
            sample[:, self.observer_shape + 1:self.observer_shape + 2])
        s_ = torch.Tensor(sample[:, self.observer_shape + 2:])
        q_eval = self.eval_net(s).gather(1, a) #计算当前 Q 值 (q_eval) 和下一个状态的目标 Q 值 (q_target)。
        q_next = self.target_net(s_).detach()
        q_target = r + 0.8 * q_next.max(1, True)[0].data

        loss = self.loss_func(q_eval, q_target) #计算损失并执行反向传播

        self.optimizer.zero_grad() #使用优化器更新神经网络的权重。
        loss.backward()
        self.optimizer.step()


class TcpNewRenoAgent:

    def __init__(self): #初始化 new_cWnd 和 new_ssThresh，它们分别表示新的拥塞窗口大小和慢启动阈值。
        self.new_cWnd = 0
        self.new_ssThresh = 0
        pass

    def get_action(self, obs, reward, done, info):
        # current ssThreshold
        ssThresh = obs[4]
        # current contention window size
        cWnd = obs[5]
        # segment size
        segmentSize = obs[6]
        # number of acked segments
        segmentsAcked = obs[9]
        # estimated bytes in flight
        bytesInFlight = obs[7]  #接收环境的观察 (obs)，奖励 (reward)，完成标志 (done)，以及其他信息 (info)。

        self.new_cWnd = 1 #根据观察状态（obs）计算新的拥塞窗口大小 (new_cWnd) 和慢启动阈值 (new_ssThresh)。
        if cWnd < ssThresh:  #根据TCP New Reno的规则，如果当前拥塞窗口小于慢启动阈值，则执行慢启动；否则执行拥塞避免。
            # slow start
            if segmentsAcked >= 1:
                self.new_cWnd = cWnd + segmentSize
        if cWnd >= ssThresh:
            # congestion avoidance
            if segmentsAcked > 0:
                adder = 1.0 * (segmentSize * segmentSize) / cWnd
                adder = int(max(1.0, adder))
                self.new_cWnd = cWnd + adder

        self.new_ssThresh = int(max(2 * segmentSize, bytesInFlight / 2))
        return [self.new_ssThresh, self.new_cWnd] #返回一个动作，其中包含新的慢启动阈值和拥塞窗口大小


class TcpDeepQAgent:

    def __init__(self):
        self.dqn = DQN() #初始化了一个深度强化学习模型 DQN，用于学习和预测TCP代理的动作
        self.new_cWnd = None #初始化了一些变量，包括当前的拥塞窗口大小 (new_cWnd)、慢启动阈值 (new_ssThresh)、当前状态 (s)、上一个动作 (a)、奖励 (r) 和下一个状态 (s_)。
        self.new_ssThresh = None
        self.s = None
        self.a = None
        self.r = None
        self.s_ = None  # next state

    def get_action(self, obs, reward, done, info): #接收环境的观察 (obs)，奖励 (reward)，完成标志 (done)，以及其他信息 (info)。
        # current ssThreshold
        ssThresh = obs[4]
        # current contention window size
        cWnd = obs[5]
        # segment size
        segmentSize = obs[6]
        # number of acked segments
        segmentsAcked = obs[9]
        # estimated bytes in flight
        bytesInFlight = obs[7]

        # update DQN #根据观察状态（obs）和之前的动作、奖励、状态更新深度强化学习模型
        self.s = self.s_
        self.s_ = [ssThresh, cWnd, segmentsAcked, segmentSize, bytesInFlight]
        if self.s is not None:  # not first time
            self.r = segmentsAcked - bytesInFlight - cWnd #奖励的计算是已经被确认的数据段数量减去当前在传输途中的字节数和当前的拥塞窗口大小。这种奖励的设计可能是为了鼓励代理在拥塞窗口管理中更有效地传输数据段，减少在传输途中的字节数，以及在达到拥塞窗口上限时适当减小拥塞窗口的大小。
            self.dqn.store_transition(self.s, self.a, self.r, self.s_)
            if self.dqn.memory_counter > self.dqn.memory_capacity:
                self.dqn.learn()

        # choose action 选择新的动作，根据深度强化学习模型的策略,根据动作的结果更新拥塞窗口大小 (new_cWnd) 和慢启动阈值 (new_ssThresh)。
        self.a = self.dqn.choose_action(self.s_)  #self.a = self.dqn.choose_action(self.s_)：使用深度 Q 学习网络 (self.dqn) 来选择动作，根据当前状态 self.s_。self.a 存储了选择的动作。
        if self.a & 1: #如果 self.a 的最低位为 1，表示 self.a 是奇数，那么 self.new_cWnd 就是当前 cWnd 加上 segmentSize。
            self.new_cWnd = cWnd + segmentSize
        else: #否则，如果 self.a 是偶数，且 cWnd 大于 0，那么 self.new_cWnd 就是当前 cWnd 加上一个值，这个值在一定条件下是 (segmentSize * segmentSize) / cWnd，否则是 1。
            if cWnd > 0:
                self.new_cWnd = cWnd + int(max(1, (segmentSize * segmentSize) / cWnd))
        if self.a < 3: #如果 self.a 小于 3，那么 self.new_ssThresh 就是 2 * segmentSize。
            self.new_ssThresh = 2 * segmentSize
        else: #否则，如果 self.a 大于等于 3，那么 self.new_ssThresh 就是 bytesInFlight / 2 的整数部分。
            self.new_ssThresh = int(bytesInFlight / 2)

        return [self.new_ssThresh, self.new_cWnd] #返回一个动作，其中包含新的慢启动阈值和拥塞窗口大小

 
class TcpQAgent: #该代理使用一个Q表来根据环境的观察状态做出关于下一步动作的决策。

    def discretize(self, metric, minval, maxval): #将连续的度量指标离散化为指定范围内的离散值。
        metric = max(metric, minval)
        metric = min(metric, maxval)
        return int((metric - minval) * (self.discrete_level - 1) / (maxval - minval))

    def __init__(self):
        self.update_times = 0 
        self.learning_rate = None  #学习速率，决定Q表中新信息覆盖旧信息的程度。
        self.discount_rate = 0.5 #未来奖励的折扣因子。
        self.discrete_level = 15 #用于表示Q表中每个状态变量的离散级别数量
        self.epsilon = 0.1  # exploration rate 探索率，确定以Q表建议的动作而不是随机动作的概率。
        self.state_size = 3 #状态变量的数量
        self.action_size = 1 #动作的数量
        self.action_num = 4 # 可用的离散动作的总数
        self.actions = np.arange(self.action_num, dtype=int) #表示可用动作的数组。
        self.q_table = np.zeros((*((self.discrete_level, ) * self.state_size), self.action_num)) #Q表，初始值为零
        # print(self.q_table.shape)
        self.new_cWnd = None
        self.new_ssThresh = None
        self.s = None
        self.a = np.zeros(self.action_size, dtype=int)
        self.r = None
        self.s_ = None  # next state

    def get_action(self, obs, reward, done, info):
        # current ssThreshold
        # ssThresh = obs[4]
        # current contention window size
        cWnd = obs[5]
        # segment size
        segmentSize = obs[6]
        # number of acked segments
        segmentsAcked = obs[9]
        # estimated bytes in flight
        bytesInFlight = obs[7]

        #将连续状态变量 (cWnd、segmentsAcked、bytesInFlight) 离散化以获得离散状态值。
        cWnd_d = self.discretize(cWnd, 0., 50000.)
        segmentsAcked_d = self.discretize(segmentsAcked, 0., 64.)
        bytesInFlight_d = self.discretize(bytesInFlight, 0., 1000000.)

        #基于观察到的奖励和下一个状态更新Q表
        self.s = self.s_
        self.s_ = [cWnd_d, segmentsAcked_d, bytesInFlight_d]
        if self.s:  # not first time
            # update Q-table
            self.learning_rate = 0.3 * (0.995 ** (self.update_times // 10))
            self.r = segmentsAcked - bytesInFlight - cWnd
            self.q_table[tuple(self.s)][tuple(self.a)] = (
                    (1 - self.learning_rate) * self.q_table[tuple(self.s)][tuple(self.a)] +
                    self.learning_rate * (self.r + self.discount_rate * np.max(self.q_table[tuple(self.s_)]))
            )
            self.update_times += 1

        #使用 epsilon-greedy 策略选择下一个动作，平衡探索和利用。
        # epsilon-greedy
        if random.uniform(0, 1) < 0.1:
            self.a[0] = np.random.choice(self.actions)
        else:
            self.a[0] = np.argmax(self.q_table[tuple(self.s_)])

        #将选择的动作映射到相应的 cWnd 和 ssThresh 值。
        # map action to cwnd and ssthresh
        if self.a[0] & 1:
            self.new_cWnd = cWnd + segmentSize
        else:
            if cWnd > 0:
                self.new_cWnd = cWnd + int(max(1, (segmentSize * segmentSize) / cWnd))
        if self.a[0] < 3:
            self.new_ssThresh = 2 * segmentSize
        else:
            self.new_ssThresh = int(bytesInFlight / 2)

        return [self.new_ssThresh, self.new_cWnd]
