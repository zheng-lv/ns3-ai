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
 * Based on script: ./examples/tcp/tcp-variants-comparison.cc
 * Modify: Pengyu Liu <eic_lpy@hust.edu.cn>
 *         Hao Yin <haoyin@uw.edu>
 *         Muyuan Shen <muyuan_shen@hust.edu.cn>
 * Topology:
 *
 *   Left Leafs (Clients)                       Right Leafs (Sinks)
 *           |            \                    /        |
 *           |             \    bottleneck    /         |
 *           |              R0--------------R1          |
 *           |             /                  \         |
 *           |   access   /                    \ access |
 *
 */

#include "ns3/ai-module.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/enum.h"
#include "ns3/error-model.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-header.h"
#include "ns3/traffic-control-module.h"

#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("rl-tcp-example");

static std::vector<uint32_t> rxPkts;  //rxPkts 是一个用于记录接收数据包数量的全局变量。

//是一个回调函数，用于统计接收数据包的数量。这个函数可能用于统计每个接收器或目的地接收到的数据包数量。通过递增对应的计数器，可以跟踪每个接收器接收到的数据包数量。
static void
CountRxPkts(uint32_t sinkId, Ptr<const Packet> packet, const Address& srcAddr) //rxPkts 可能是一个向量，其中每个元素表示一个接收数据包的计数器。sinkId 可能是一个标识符，用于唯一标识接收数据包的目的地或接收器。packet 可能是一个指向接收到的数据包的指针，包含了数据包的内容。srcAddr 可能是源地址，用于标识发送数据包的源节点或设备。
{
    rxPkts[sinkId]++; //使用 rxPkts[sinkId] 来访问静态向量 rxPkts 中与 sinkId 对应的元素，并将其值递增 1。
}

//是一个用于打印接收数据包数量的函数。用于在程序运行期间打印接收数据包的计数器，以提供关于接收数据包数量的信息。它可以帮助调试或监控网络通信或数据包处理过程。
static void
PrintRxCount()
{
    uint32_t size = rxPkts.size();  //首先获取静态向量 rxPkts 的大小，并将其存储在变量 size 中。
    NS_LOG_UNCOND("RxPkts:");  //使用 NS_LOG_UNCOND 宏打印出一个标题为 "RxPkts:" 的消息。
    for (uint32_t i = 0; i < size; i++)   //使用一个循环遍历向量 rxPkts 的所有元素。对于每个元素，使用 NS_LOG_UNCOND 宏打印出一个消息，其中包含计数器的索引 i 和对应的值 rxPkts.at(i)。
    {
        NS_LOG_UNCOND("---SinkId: " << i << " RxPkts: " << rxPkts.at(i)); //rxPkts.at(i) 表示向量 rxPkts 中索引为 i 的元素的值。这可能是一个计数器，表示从对应接收器或目的地接收到的数据包数量。
    }
}

int
main(int argc, char* argv[])
{
    double tcpEnvTimeStep = 0.1;  //表示时间步长，值为 0.1，表示每隔 0.1 秒进行一次模拟更新。
    uint32_t nLeaf = 1;   //表示网络中的叶子节点数量，值为 1。
    std::string transport_prot = "TcpRlTimeBased";  //表示传输协议，值为 "TcpRlTimeBased"，可能是一种基于时间的 TCP 协议。
    double error_p = 0.0;  //表示错误概率，值为 0.0，表示错误发生的概率为 0。
    std::string bottleneck_bandwidth = "2Mbps";  //表示瓶颈带宽，值为 "2Mbps"，表示瓶颈链路的带宽为 2 兆位每秒。
    std::string bottleneck_delay = "0.01ms";    //：表示瓶颈延迟，值为 "0.01ms"，表示瓶颈链路的延迟为 0.01 毫秒。
    std::string access_bandwidth = "10Mbps";   //：表示接入带宽，值为 "10Mbps"，表示接入链路的带宽为 10 兆位每秒。
    std::string access_delay = "20ms";         //表示接入延迟，值为 "20ms"，表示接入链路的延迟为 20 毫秒。
    std::string prefix_file_name = "TcpVariantsComparison";  //表示前缀文件名，值为 "TcpVariantsComparison"，可能是用于保存模拟结果的文件名的前缀。
    uint64_t data_mbytes = 0;  //表示数据大小，值为 0，表示没有指定数据大小。
    uint32_t mtu_bytes = 400;  //表示最大传输单元 (MTU) 的大小，值为 400，表示 MTU 大小为 400 字节。
    double duration = 1000.0;  //表示模拟持续时间，值为 1000.0，表示模拟持续时间为 1000 秒。
    uint32_t run = 0;         //表示运行次数，值为 0，表示没有指定运行次数。
    bool flow_monitor = false;  //表示是否启用流量监测，值为 false，表示不启用流量监测。
    bool sack = true;           //表示是否启用选择性确认 (SACK)，值为 true，表示启用 SACK。
    std::string queue_disc_type = "ns3::PfifoFastQueueDisc";   //表示队列调度类型，值为 "ns3::PfifoFastQueueDisc"，表示使用 ns3 中提供的先进先出快速队列调度器。
    std::string recovery = "ns3::TcpClassicRecovery";          //表示恢复策略，值为 "ns3::TcpClassicRecovery"，表示使用 ns3 中提供的经典 TCP 恢复策略。

    CommandLine cmd; //定义了一个命令行解析器 CommandLine 对象 cmd，并使用 AddValue 方法添加了一些命令行参数。
    // seed related 用于指定随机数生成器的种子。默认值为 0。
    cmd.AddValue("simSeed", "Seed for random generator. Default: 0", run);
    // other
    cmd.AddValue("envTimeStep",
                 "Time step interval for TcpRlTimeBased. Default: 0.1s",
                 tcpEnvTimeStep);    //指定 TcpRlTimeBased 协议的时间步长。默认值为 0.1 秒。
    cmd.AddValue("nLeaf", "Number of left and right side leaf nodes", nLeaf); //指定左右两侧的叶子节点数量。
    cmd.AddValue("transport_prot",
                 "Transport protocol to use: TcpNewReno, TcpHybla, TcpHighSpeed, TcpHtcp, "
                 "TcpVegas, TcpScalable, TcpVeno, TcpBic, TcpYeah, TcpIllinois, TcpWestwood, "
                 "TcpWestwoodPlus, TcpLedbat, TcpLp, TcpRlTimeBased, TcpRlEventBased",
                 transport_prot); //指定要使用的传输协议，可以选择 "TcpNewReno", "TcpHybla", "TcpHighSpeed", "TcpHtcp", "TcpVegas", "TcpScalable", "TcpVeno", "TcpBic", "TcpYeah", "TcpIllinois", "TcpWestwood", "TcpWestwoodPlus", "TcpLedbat", "TcpLp", "TcpRlTimeBased", "TcpRlEventBased" 中的一个。
    cmd.AddValue("error_p", "Packet error rate", error_p);  //指定包错误率。
    cmd.AddValue("bottleneck_bandwidth", "Bottleneck bandwidth", bottleneck_bandwidth); //指定瓶颈带宽。
    cmd.AddValue("bottleneck_delay", "Bottleneck delay", bottleneck_delay);  //指定瓶颈延迟。
    cmd.AddValue("access_bandwidth", "Access link bandwidth", access_bandwidth); //指定接入带宽。
    cmd.AddValue("access_delay", "Access link delay", access_delay);             //指定接入延迟
    cmd.AddValue("prefix_name", "Prefix of output trace file", prefix_file_name);//指定输出跟踪文件的前缀。
    cmd.AddValue("data", "Number of Megabytes of data to transmit", data_mbytes);//指定要传输的数据大小（以兆字节为单位）。
    cmd.AddValue("mtu", "Size of IP packets to send in bytes", mtu_bytes);       //指定要发送的 IP 数据包的大小（以字节为单位）。
    cmd.AddValue("duration", "Time to allow flows to run in seconds", duration); //指定允许流量运行的时间（以秒为单位）。
    cmd.AddValue("flow_monitor", "Enable flow monitor", flow_monitor);           //启用或禁用流量监测。
    cmd.AddValue("queue_disc_type",
                 "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)",
                 queue_disc_type);                                              //指定网关的队列调度类型（例如，"ns3::CoDelQueueDisc"）。
    cmd.AddValue("sack", "Enable or disable SACK option", sack);                //启用或禁用 SACK 选项。
    cmd.AddValue("recovery", "Recovery algorithm type to use (e.g., ns3::TcpPrrRecovery", recovery); //指定要使用的恢复算法类型（例如，"ns3::TcpPrrRecovery"）。
    cmd.Parse(argc, argv);  //使用 Parse 方法解析命令行参数。

    // There are two kinds of Tcp congestion control algorithm using RL: 
    // 1. TcpRlTimeBased
    // 2. TcpRlEventBased
    // The only difference is when interaction occurs (at fixed interval or at event).
    //这段代码定义了两种基于强化学习的 Tcp 拥塞控制算法：TcpRlTimeBased 和 TcpRlEventBased。它们的唯一区别在于交互发生的时间（在固定间隔或在事件发生时）。
    if (transport_prot == "TcpRlTimeBased") //如果所选择的传输协议是 "TcpRlTimeBased"，则通过 Config::SetDefault 设置 ns3::TcpTimeStepEnv::StepTime 为给定的时间间隔 tcpEnvTimeStep。
    {
        Config::SetDefault("ns3::TcpTimeStepEnv::StepTime", TimeValue(Seconds(tcpEnvTimeStep)));
    }

    transport_prot = std::string("ns3::") + transport_prot; //将传输协议类型字符串 transport_prot 添加前缀 "ns3::"
    Config::SetDefault("ns3::TcpL4Protocol::SocketType",     //然后通过 Config::SetDefault 设置 ns3::TcpL4Protocol::SocketType 为所得类型。这里是为了告诉仿真器使用哪一种 TCP 协议。
                       TypeIdValue(TypeId::LookupByName(transport_prot)));  //TypeId::LookupByName 方法接受一个字符串参数 transport_prot，它表示要查找的类型名称。该方法将在类型注册库中查找与指定名称匹配的类型，并返回相应的 TypeIdValue。

    // OpenGym Env --- has to be created before any other thing  创建 OpenGym 环境接口：
    Ptr<OpenGymInterface> openGymInterface;
    if (transport_prot == "ns3::TcpRlTimeBased" or transport_prot == "ns3::TcpRlEventBased")
    {
        openGymInterface = OpenGymInterface::Get();
    }  //如果所选择的传输协议是 "ns3::TcpRlTimeBased" 或 "ns3::TcpRlEventBased"，则创建一个 OpenGymInterface 的指针。这是为了与 OpenGym 进行交互，可能是用于强化学习中的环境交互。

    SeedManager::SetSeed(1); //使用 SeedManager::SetSeed 设置随机数种子。
    SeedManager::SetRun(run);

    NS_LOG_UNCOND("C++ side random seed: " << run);  //打印 C++ 侧的随机种子和选择的 Tcp 版本。
    NS_LOG_UNCOND("Tcp version: " << transport_prot);

    // Calculate the ADU size 用于计算 IP 和 Tcp 头部的大小，并计算 Tcp 应用数据单元（ADU）的大小。
    Header* temp_header = new Ipv4Header(); //创建一个 Ipv4Header 对象 temp_header
    uint32_t ip_header = temp_header->GetSerializedSize(); //使用 GetSerializedSize 方法获取 IP 头部的大小，将大小存储在变量 ip_header 中
    NS_LOG_LOGIC("IP Header size is: " << ip_header);      //使用 NS_LOG_LOGIC 打印出 IP 头部的大小。
    delete temp_header;                                    //删除 temp_header
    temp_header = new TcpHeader();                         //创建一个 TcpHeader 对象 temp_header
    uint32_t tcp_header = temp_header->GetSerializedSize();//用 GetSerializedSize 方法获取 Tcp 头部的大小，将大小存储在变量 tcp_header 中
    NS_LOG_LOGIC("TCP Header size is: " << tcp_header);    //使用 NS_LOG_LOGIC 打印出 Tcp 头部的大小。
    delete temp_header;                                    //删除 temp_header
    uint32_t tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header); //计算 Tcp ADU 的大小。使用 mtu_bytes（表示 IP 数据包的最大传输单元）减去 20（表示 IP 头部的最小大小）和 ip_header + tcp_header（表示 IP 和 Tcp 头部的总大小），得到 Tcp ADU 的大小
    NS_LOG_LOGIC("TCP ADU size is: " << tcp_adu_size);    //使用 NS_LOG_LOGIC 打印出 Tcp ADU 的大小

    // Set the simulation start and stop time  设置模拟的开始时间和停止时间。
    double start_time = 0.1;   //定义了一个双精度浮点数变量 start_time，并将其初始化为 0.1，表示模拟的开始时间。
    double stop_time = start_time + duration;  //使用加法运算符将 start_time 和 duration 相加，得到模拟的停止时间。将结果存储在变量 stop_time 中。

    // 4 MB of TCP buffer 设置一些 TCP 相关的默认配置参数
    Config::SetDefault("ns3::TcpSocket::RcvBufSize", UintegerValue(1 << 21));
    Config::SetDefault("ns3::TcpSocket::SndBufSize", UintegerValue(1 << 21)); //TCP 缓冲区大小：ns3::TcpSocket::RcvBufSize 和 ns3::TcpSocket::SndBufSize 被设置为 4 MB（1 << 21 字节）。这是 TCP 接收和发送缓冲区的大小。
    Config::SetDefault("ns3::TcpSocketBase::Sack", BooleanValue(sack));     //选择性确认（SACK）：ns3::TcpSocketBase::Sack 被设置为 sack 的布尔值。SACK 是一种 TCP 选项，用于选择性地确认已收到的数据块。
    Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(2));   //延迟确认计数（Delayed ACK）：ns3::TcpSocket::DelAckCount 被设置为 2。表示 TCP 使用延迟确认，每收到两个数据包时发送一个确认。
    Config::SetDefault("ns3::TcpL4Protocol::RecoveryType",
                       TypeIdValue(TypeId::LookupByName(recovery))); //拥塞控制算法的恢复类型：ns3::TcpL4Protocol::RecoveryType 被设置为由 recovery 字符串表示的拥塞控制算法的类型。这个值将被用于设置仿真中使用的 TCP 拥塞控制算法。

    // Configure the error model  用于配置错误模型。
    // Here we use RateErrorModel with packet error rate
    Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>(); //使用 CreateObject 方法创建一个 UniformRandomVariable 对象 uv。UniformRandomVariable 是一种随机变量类型，用于生成均匀分布的随机数。
    uv->SetStream(50);                                           //使用 SetStream 方法将 uv 的流大小设置为 50。流大小表示在一次模拟中生成的随机数序列的长度。
    RateErrorModel error_model;                                  //创建一个 RateErrorModel 对象 error_model，用于表示错误模型
    error_model.SetRandomVariable(uv);                           //使用 SetRandomVariable 方法将 uv 作为随机变量设置到 error_model 中。
    error_model.SetUnit(RateErrorModel::ERROR_UNIT_PACKET);      //使用 SetUnit 方法将错误模型的单位设置为 RateErrorModel::ERROR_UNIT_PACKET，表示错误率是以包为单位计算的。
    error_model.SetRate(error_p);                                //使用 SetRate 方法将错误率设置为 error_p。错误率是指每个包发生错误的概率。

    // Create the point-to-point link helpers  创建点对点链路助手。
    PointToPointHelper bottleNeckLink;       // 创建了一个 PointToPointHelper 对象 bottleNeckLink，用于表示瓶颈链路
    bottleNeckLink.SetDeviceAttribute("DataRate", StringValue(bottleneck_bandwidth));    //使用 SetDeviceAttribute 方法将瓶颈链路的带宽设置为 bottleneck_bandwidth，
    bottleNeckLink.SetChannelAttribute("Delay", StringValue(bottleneck_delay));          //通道，延迟设置为 bottleneck_delay。
    // bottleNeckLink.SetDeviceAttribute  ("ReceiveErrorModel", PointerValue (&error_model));

    PointToPointHelper pointToPointLeaf;     //创建了另一个 PointToPointHelper 对象 pointToPointLeaf，用于表示叶子链路。
    pointToPointLeaf.SetDeviceAttribute("DataRate", StringValue(access_bandwidth));  //使用 SetDeviceAttribute 方法将叶子链路的带宽设置为 access_bandwidth
    pointToPointLeaf.SetChannelAttribute("Delay", StringValue(access_delay));       //通道，延迟设置为 access_delay。

    PointToPointDumbbellHelper d(nLeaf, pointToPointLeaf, nLeaf, pointToPointLeaf, bottleNeckLink);  //使用 PointToPointDumbbellHelper 类创建了一个哑铃型网络助手 d，其中包含 nLeaf 个叶子节点和一个瓶颈链路。哑铃型网络助手将瓶颈链路和叶子链路连接起来，形成一个完整的网络结构。
            //PointToPointDumbbellHelper 类是一个网络助手类，用于帮助构建哑铃型网络结构。它接受五个参数：
                //nLeaf：叶子节点的数量。
                //pointToPointLeaf：叶子节点之间的点对点链路助手。
                //nLeaf：第二个叶子节点的数量，与第一个参数相同。
                //pointToPointLeaf：第二个叶子节点之间的点对点链路助手，与第二个参数相同。
                //bottleNeckLink：瓶颈链路助手。

    // Install IP stack   安装 IP 堆栈后，节点就可以参与网络通信，例如发送和接收 IP 数据包、进行网络地址转换等。
    InternetStackHelper stack;  //InternetStackHelper 是一个网络助手类，用于帮助安装 IP 堆栈
    stack.InstallAll();         //InstallAll 方法是 InternetStackHelper 类的一个成员函数，用于在节点上安装所有的 IP 堆栈模块。
                                //通过调用 stack.InstallAll()，程序会在节点上安装所有的 IP 堆栈模块，包括 IP 协议、ICMP 协议、UDP 协议和 TCP 协议等。这些协议栈模块是网络通信所必需的，它们提供了在网络上发送和接收数据的基本功能。



    
    // Traffic Control   用于配置流量控制（Traffic Control）的相关参数
    //TrafficControlHelper 类用于配置流量控制，创建了两个 TrafficControlHelper 对象：tchPfifo 和 tchCoDel。
    //配置先进先出（FIFO）队列和主动队列管理（CoDel）队列。
    //使用 SetRootQueueDisc 方法将队列类型设置为指定的队列类型。对于 FIFO 队列，使用 ns3::PfifoFastQueueDisc；对于 CoDel 队列，使用 ns3::CoDelQueueDisc。
    TrafficControlHelper tchPfifo;  
    tchPfifo.SetRootQueueDisc("ns3::PfifoFastQueueDisc");

    TrafficControlHelper tchCoDel;
    tchCoDel.SetRootQueueDisc("ns3::CoDelQueueDisc");

    //定义访问链路和瓶颈链路的带宽和时延：使用 DataRate 对象 access_b 和 bottle_b 分别表示接入带宽和瓶颈带宽。使用 Time 对象 access_d 和 bottle_d 分别表示接入延迟和瓶颈延迟。
    DataRate access_b(access_bandwidth);  //定义访问链路和瓶颈链路的带宽
    DataRate bottle_b(bottleneck_bandwidth);
    Time access_d(access_delay);         //定义访问链路和瓶颈链路的时延。
    Time bottle_d(bottleneck_delay);

    //计算队列的最大大小：通过比较访问链路和瓶颈链路的带宽，以及它们的时延，计算了队列的最大大小。这里的计算涉及到流量的字节大小和链路的时延。
    uint32_t size = static_cast<uint32_t>((std::min(access_b, bottle_b).GetBitRate() / 8) *
                                          ((access_d + bottle_d + access_d) * 2).GetSeconds());

    //设置队列的最大大小：使用 Config::SetDefault 方法设置 PFIFO 和 CoDel 队列的最大大小，分别基于队列长度和队列字节大小。
    Config::SetDefault("ns3::PfifoFastQueueDisc::MaxSize",
                       QueueSizeValue(QueueSize(QueueSizeUnit::PACKETS, size / mtu_bytes)));
    Config::SetDefault("ns3::CoDelQueueDisc::MaxSize",
                       QueueSizeValue(QueueSize(QueueSizeUnit::BYTES, size)));

    //据指定的队列类型（ns3::PfifoFastQueueDisc 或 ns3::CoDelQueueDisc），使用 Install 方法在哑铃型网络助手 d 的左右节点的设备 1 上安装流量控制模块。
    if (queue_disc_type == "ns3::PfifoFastQueueDisc")
    {
        tchPfifo.Install(d.GetLeft()->GetDevice(1));
        tchPfifo.Install(d.GetRight()->GetDevice(1));
    }
    else if (queue_disc_type == "ns3::CoDelQueueDisc")
    {
        tchCoDel.Install(d.GetLeft()->GetDevice(1));
        tchCoDel.Install(d.GetRight()->GetDevice(1));
    }
    else //错误处理：如果用户提供的队列调度算法不是 PFIFO 或 CoDel，程序将抛出错误。
    {
        NS_FATAL_ERROR("Queue not recognized. Allowed values are ns3::CoDelQueueDisc or "
                       "ns3::PfifoFastQueueDisc");
    }



     
    // Assign IP Addresses  分配 IP 地址：使用 AssignIpv4Addresses 方法为仿真中的各个节点分配 IPv4 地址。这里为左侧、右侧和底层节点分别分配了 IP 地址段。
    d.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0", "255.255.255.0"),
                          Ipv4AddressHelper("10.2.1.0", "255.255.255.0"),
                          Ipv4AddressHelper("10.3.1.0", "255.255.255.0"));

    NS_LOG_INFO("Initialize Global Routing."); //使用 NS_LOG_INFO 宏打印一条消息，表明正在初始化全局路由表。
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();//使用 Ipv4GlobalRoutingHelper::PopulateRoutingTables 方法填充全局路由表。

    // Install apps in left and right nodes
    uint16_t port = 50000;    //定义了一个端口号 port，值为 50000，表示接收器应用程序将监听该端口。
    Address sinkLocalAddress(InetSocketAddress(Ipv4Address::GetAny(), port));
    PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", sinkLocalAddress);   //使用 PacketSinkHelper 类在哑铃型网络的右侧节点上安装一个简单的数据包接收器应用程序。
    ApplicationContainer sinkApps;
    for (uint32_t i = 0; i < d.RightCount(); ++i)
    {
        sinkHelper.SetAttribute("Protocol", TypeIdValue(TcpSocketFactory::GetTypeId())); //该对象使用 TcpSocketFactory 创建一个 TcpSocket，并将其绑定到指定的端口。
        sinkApps.Add(sinkHelper.Install(d.GetRight(i)));  //使用 ApplicationContainer::Add 方法将接收器应用程序安装到哑铃型网络的右侧节点上。安装的接收器应用程序数量由 d.RightCount() 函数返回的值确定。
    }
    sinkApps.Start(Seconds(0.0));  //使用 ApplicationContainer::Start 和 ApplicationContainer::Stop 方法启动和停止接收器应用程序。接收器应用程序将在启动后立即开始监听指定的端口，并在停止时停止监听。停止时间由 stop_time 变量指定。
    sinkApps.Stop(Seconds(stop_time));

    for (uint32_t i = 0; i < d.LeftCount(); ++i)  //为每个左侧节点创建一个 TCP 客户端应用，并使用 BulkSendHelper 来模拟 TCP 流量的发送。 循环遍历左侧节点：
    {
        // Create an on/off app sending packets to the left side
        AddressValue remoteAddress(InetSocketAddress(d.GetRightIpv4Address(i), port)); //创建发送端地址：根据右侧节点的 IPv4 地址和指定的端口创建发送端地址
        Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcp_adu_size)); //配置 TCP Socket 的一些属性： 设置 TCP Socket 的段大小属性
        BulkSendHelper ftp("ns3::TcpSocketFactory", Address()); //创建 BulkSendHelper：使用 BulkSendHelper 创建一个发送器，指定 TCP Socket 工厂和发送器的地址（这里为空地址，表示系统会自动选择可用的地址）。
        ftp.SetAttribute("Remote", remoteAddress);//设置 BulkSendHelper 的属性：设置 BulkSendHelper 的远程地址属性，即发送数据的目标地址。
        ftp.SetAttribute("SendSize", UintegerValue(tcp_adu_size));   //设置发送的数据包大小。
        ftp.SetAttribute("MaxBytes", UintegerValue(data_mbytes * 1000000)); //设置发送的最大字节数。

        ApplicationContainer clientApp = ftp.Install(d.GetLeft(i));  //安装并配置应用： 安装 BulkSendHelper 应用到左侧节点。
        clientApp.Start(Seconds(start_time * i)); // Start after sink 设置应用开始时间，确保在 Sink 应用之后开始发送数据。
        clientApp.Stop(Seconds(stop_time - 3));   // Stop before the sink 设置应用结束时间，确保在 Sink 应用之前停止发送数据。
    }

    // Flow monitor 用于配置和安装流量监视器（Flow Monitor）流量监视器允许您收集和分析仿真期间的网络流量信息，如数据包传输、时延、吞吐量等。
    FlowMonitorHelper flowHelper; //创建 FlowMonitorHelper 实例：创建一个名为 flowHelper 的 FlowMonitorHelper 对象。
    if (flow_monitor)  //检查是否启用流量监视器：
    {
        flowHelper.InstallAll(); //安装流量监视器：如果流量监视器已启用，则调用 InstallAll() 方法安装流量监视器。这将使监视器开始跟踪仿真期间的流量信息。
    }

    // Count RX packets 计算接收（RX）的数据包数量
    //使用一个循环（for 循环），遍历右侧节点的 PacketSink 应用程序，为每个节点初始化一个计数器（rxPkts），并通过回调函数连接到 Rx TraceSource。这样，每次数据包到达时都会触发回调函数，用于增加相应节点的接收数据包计数。
    for (uint32_t i = 0; i < d.RightCount(); ++i)
    {
        rxPkts.push_back(0);
        Ptr<PacketSink> pktSink = DynamicCast<PacketSink>(sinkApps.Get(i));
        pktSink->TraceConnectWithoutContext("Rx", MakeBoundCallback(&CountRxPkts, i));
    }

    //设置仿真停止时间，确保仿真在达到指定的 stop_time 后停止运行。
    Simulator::Stop(Seconds(stop_time));
    Simulator::Run();//运行仿真：直到达到设定的停止时间。

    //序列化流量监视器信息到 XML 文件：
    //如果启用了流量监视器 (flow_monitor 为真)，则使用 flowHelper.SerializeToXmlFile 将流量监视器的信息保存到 XML 文件中。这包括有关仿真期间数据包流量的详细信息。
    if (flow_monitor)
    {
        flowHelper.SerializeToXmlFile(prefix_file_name + ".flowmonitor", true, true);
    }

    //通知 OpenGym 接口仿真结束：
    //如果使用了基于时间或事件的 RL TCP 实现，则通过 OpenGym 接口通知仿真结束。这是为了使 RL 算法知道仿真已经完成，可以进行最后的总结和学习。
    if (transport_prot == "ns3::TcpRlTimeBased" or transport_prot == "ns3::TcpRlEventBased")
    {
        openGymInterface->NotifySimulationEnd();
    }

    PrintRxCount();//打印接收数据包数量：调用一个函数，该函数可能用于打印每个节点接收到的数据包数量的信息。
    Simulator::Destroy();//销毁仿真：释放仿真资源，确保正确结束仿真。
    return 0;
}
