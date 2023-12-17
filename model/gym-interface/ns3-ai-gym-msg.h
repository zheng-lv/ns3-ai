/*
 * Copyright (c) 2023 Huazhong University of Science and Technology
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
 * Author:  Muyuan Shen <muyuan_shen@hust.edu.cn>
 */

#ifndef NS3_NS3_AI_GYM_MSG_H
#define NS3_NS3_AI_GYM_MSG_H

#include <stdint.h>

#define MSG_BUFFER_SIZE 1024

struct Ns3AiGymMsg
{
    uint8_t buffer[MSG_BUFFER_SIZE];//包含一个 uint8_t 类型的数组 buffer，用于存储消息数据，
    uint32_t size;//uint32_t 类型的变量 size，用于表示消息的大小。
};

#endif // NS3_NS3_AI_GYM_MSG_H 定义了一个结构体，用于在 ns-3 网络模拟器中发送和接收 gym 消息。这个结构体包含一个用于存储消息数据的缓冲区和一个用于表示消息大小的变量
