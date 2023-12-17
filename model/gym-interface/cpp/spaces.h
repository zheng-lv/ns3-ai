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
 *这是一个 C++ 头文件，定义了一些与 Gymnasium 环境中的空间相关的类
 OpenGymSpace:
基类，表示 Gym 环境中的通用空间。
包含纯虚函数 GetSpaceDescription()，用于获取空间的描述信息。
包含虚函数 Print()，用于在输出流中打印空间信息。
OpenGymDiscreteSpace:
表示 Gym 中的离散空间，例如离散动作。
包含一个整数属性 m_n，表示离散空间的大小。
实现了基类中的虚函数，提供了获取空间描述信息和打印空间信息的功能。
OpenGymBoxSpace:
表示 Gym 中的连续空间，例如连续动作或状态。
可以表示包括多个维度的 Box 空间。
包含属性 m_low 和 m_high，表示空间的最小和最大值。
包含属性 m_shape，表示空间的形状（维度）。
包含属性 m_dtype，表示空间数据类型。
实现了基类中的虚函数，提供了获取空间描述信息和打印空间信息的功能。
OpenGymTupleSpace:
表示 Gym 中的元组空间，可以包含多个子空间。
包含一个存储子空间的容器 m_tuple。
实现了基类中的虚函数，提供了获取空间描述信息和打印空间信息的功能。
OpenGymDictSpace:
表示 Gym 中的字典空间，可以包含多个键值对，每个值是一个子空间。
包含一个存储键值对的容器 m_dict。
实现了基类中的虚函数，提供了获取空间描述信息和打印空间信息的功能。
 */

#ifndef OPENGYM_SPACES_H
#define OPENGYM_SPACES_H

#include "messages.pb.h"

#include "ns3/object.h"

namespace ns3
{
//表示开放健身房空间，并提供一些基本的功能，如获取空间描述和打印空间信息
class OpenGymSpace : public Object
{
  public:
    OpenGymSpace();//用于创建 OpenGymSpace 对象。
    ~OpenGymSpace() override;//用于销毁 OpenGymSpace 对象。

    static TypeId GetTypeId();//静态函数，返回 OpenGymSpace 类的类型 ID。

    virtual ns3_ai_gym::SpaceDescription GetSpaceDescription() = 0;//纯虚函数，用于获取空间描述信息
    virtual void Print(std::ostream& where) const = 0;//纯虚函数，用于打印空间信息。

  protected:
    // Inherited
    void DoInitialize() override;//虚函数，用于执行初始化操作。
    void DoDispose() override;//虚函数，用于执行销毁操作
};

class OpenGymDiscreteSpace : public OpenGymSpace
{
  public:
    OpenGymDiscreteSpace();//构造函数，用于创建 OpenGymDiscreteSpace 对象。
    OpenGymDiscreteSpace(int n);//构造函数，接受一个整数参数 n。
    ~OpenGymDiscreteSpace() override;//析构函数，用于销毁 OpenGymDiscreteSpace 对象。

    static TypeId GetTypeId();//静态函数，返回 OpenGymDiscreteSpace 类的类型 ID。

    ns3_ai_gym::SpaceDescription GetSpaceDescription() override;//重写的纯虚函数，用于获取空间描述信息。

    int GetN();//获取空间的大小。
    void Print(std::ostream& where) const override;//重写的纯虚函数，用于打印空间信息。

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymDiscreteSpace> space)
    {
        space->Print(os);
        return os;
    }//

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

  private:
    int m_n;
};
//表示一个具有下界、上界、形状和数据类型的盒子空间，并提供一些基本的功能，如获取空间描述和打印空间信息。
class OpenGymBoxSpace : public OpenGymSpace
{
  public:
    OpenGymBoxSpace();
    OpenGymBoxSpace(float low, float high, std::vector<uint32_t> shape, std::string dtype);//构造函数，接受一个浮点数 low、一个浮点数 high、一个整数向量 shape 和一个字符串 dtype。
    OpenGymBoxSpace(std::vector<float> low,
                    std::vector<float> high,
                    std::vector<uint32_t> shape,
                    std::string dtype);
    ~OpenGymBoxSpace() override;

    static TypeId GetTypeId();

    ns3_ai_gym::SpaceDescription GetSpaceDescription() override;

    float GetLow();//获取空间的下界。
    float GetHigh();//获取空间的上界。
    std::vector<uint32_t> GetShape();//获取空间的形状。

    void Print(std::ostream& where) const override;//重写的纯虚函数，用于打印空间信息。

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymBoxSpace> space)
    {
        space->Print(os);
        return os;
    }//元函数，用于将 OpenGymBoxSpace 对象输出到流中。

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

//包含一些私有成员变量和函数：
  private:
    void SetDtype();//私有函数，用于设置数据类型。

    float m_low;//私有成员变量，存储空间的下界。
    float m_high;//私有成员变量，存储空间的上界。
    std::vector<uint32_t> m_shape;//私有成员变量，存储空间的形状。
    std::string m_dtypeName;//私有成员变量，存储数据类型的名称。
    std::vector<float> m_lowVec;//私有成员变量，存储下界的向量表示。
    std::vector<float> m_highVec;//私有成员变量，存储上界的向量表示。

    ns3_ai_gym::Dtype m_dtype;//私有成员变量，存储数据类型的对象。
};
//是在 ns-3 网络模拟器中表示一个包含多个子空间的元组空间，并提供一些基本的功能，如获取空间描述和打印空间信息。
class OpenGymTupleSpace : public OpenGymSpace
{
  public:
    OpenGymTupleSpace();
    ~OpenGymTupleSpace() override;

    static TypeId GetTypeId();

    ns3_ai_gym::SpaceDescription GetSpaceDescription() override;

    bool Add(Ptr<OpenGymSpace> space);//用于向 OpenGymTupleSpace 对象中添加子空间。

    Ptr<OpenGymSpace> Get(uint32_t idx);//用于获取 OpenGymTupleSpace 对象中指定索引位置的子空间。

    void Print(std::ostream& where) const override;

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymTupleSpace> space)
    {
        space->Print(os);
        return os;
    }

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

  private:
    std::vector<Ptr<OpenGymSpace>> m_tuple;//私有成员变量 m_tuple，它是一个指向 OpenGymSpace 对象的向量。
};
/*在 ns-3 网络模拟器中表示一个包含多个子空间的字典空间，并提供一些基本的功能，如获取空间描述和打印空间信息。
*键用于唯一标识字典中的每个项，值是指向 OpenGymSpace 对象的指针，表示对应键所关联的子空间。
*/
class OpenGymDictSpace : public OpenGymSpace
{
  public:
    OpenGymDictSpace();
    ~OpenGymDictSpace() override;

    static TypeId GetTypeId();

    ns3_ai_gym::SpaceDescription GetSpaceDescription() override;

    bool Add(std::string key, Ptr<OpenGymSpace> value);//用于向字典中添加键值对。
    Ptr<OpenGymSpace> Get(std::string key);//用于获取字典中指定键的值。

    void Print(std::ostream& where) const override;

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymDictSpace> space)
    {
        space->Print(os);
        return os;
    }

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

  private:
    std::map<std::string, Ptr<OpenGymSpace>> m_dict;//私有成员变量 m_dict，它是一个 std::map 类型的字典，用于存储键值对。
};

} // end of namespace ns3

#endif /* OPENGYM_SPACES_H */
