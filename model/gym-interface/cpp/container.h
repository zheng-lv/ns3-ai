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

#ifndef OPENGYM_CONTAINER_H
#define OPENGYM_CONTAINER_H

#include "messages.pb.h"

#include <ns3/object.h>
#include <ns3/type-name.h>

namespace ns3
{
/*该类为创建与Gymnasium兼容的环境提供了一个基础框架。子类需要实现纯虚函数，以适应特定的环境数据结构和打印方式。*/
class OpenGymDataContainer : public Object
{
  public:
    OpenGymDataContainer();//构造函数，用于初始化对象的状态。
    ~OpenGymDataContainer() override;//虚析构函数，用于在对象销毁时执行清理操

    static TypeId GetTypeId();//GetTypeId()：这是一个静态函数，通常在NS-3模拟器中用于标识对象类型。在这里，它可能用于在模拟器中注册这个类的类型。

    virtual ns3_ai_gym::DataContainer GetDataContainerPbMsg() = 0;//纯虚函数，子类必须实现它。它返回一个 ns3_ai_gym::DataContainer 对象，可能包含环境数据。
    static Ptr<OpenGymDataContainer> CreateFromDataContainerPbMsg(
        ns3_ai_gym::DataContainer& dataContainer);//静态函数，用于创建一个 OpenGymDataContainer 的实例，通过传入的 ns3_ai_gym::DataContainer 对象初始化。

  //Print函数和运算符重载：
    virtual void Print(std::ostream& where) const = 0;//纯虚函数，子类必须实现。它用于打印对象的信息到给定的输出流。
    /*
    virtual：关键字表示这是一个虚函数，它可以在派生类中被重写。
    void：表示函数的返回类型是空，即不返回任何值。
    Print：函数的名称是 "Print"。
    (std::ostream& where)：表示函数接受一个引用参数，该参数是一个 std::ostream 类型的对象，命名为 where。这个参数是一个输出流，允许将对象的信息输出到流中。
    const：表示这是一个常量成员函数，即在函数内部不能修改对象的成员变量。
    = 0：表示这是一个纯虚函数，它在基类中没有实现，而是在派生类中被强制实现。在这个函数声明中，= 0 的存在使得 Print 函数成为一个纯虚函数。  
    */

  //这是一个友元函数的声明，允许在类外部访问私有成员。它重载了流插入运算符 (<<)，以便可以直接通过 std::cout 或其他输出流打印 OpenGymDataContainer 对象。
    /*
    std::ostream： 是C++标准库中的输出流类。它用于处理输出到各种目标，例如控制台、文件等。std::ostream 可以通过 #include <iostream> 引入。
    operator<<： 是C++中的左移位运算符。在这个上下文中，它被用作插入操作符的重载。通过重载这个运算符，可以自定义类对象在输出流中的打印方式。
    */
    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymDataContainer> container)
    {
        container->Print(os);
        return os;
      /*
      在函数体内，它调用 OpenGymDataContainer 对象的 Print 函数，将其输出到给定的输出流 os 中。
      然后，它返回这个输出流对象，以便支持链式操作（例如 std::cout << container << someOtherData）。
      */
    }//友元函数，用于重载输出流运算符，以便通过 std::cout 打印 OpenGymDataContainer 对象。

  protected: //这是 Object 类中的虚函数，被重写以执行初始化和资源释放操作。
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;
};


//表示离散空间中的数值，并提供了一些操作函数用于设置和获取数值，以及将对象信息打印到输出流。
class OpenGymDiscreteContainer : public OpenGymDataContainer
{
  public:
    OpenGymDiscreteContainer();// 默认构造函数
    OpenGymDiscreteContainer(uint32_t n);// 带参数的构造函数，指定离散空间的大小
    ~OpenGymDiscreteContainer() override;// 析构函数

    static TypeId GetTypeId();// 获取 TypeId，用于类型标识

    ns3_ai_gym::DataContainer GetDataContainerPbMsg() override;// 获取用于序列化的 Protocol Buffers 消息

    void Print(std::ostream& where) const override;// 打印对象信息到输出流

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymDiscreteContainer> container)// 友元函数，重载 << 运算符，用于对象插入到输出流
    {
        container->Print(os);
        return os;
    }

    bool SetValue(uint32_t value);// 设置离散数值
    uint32_t GetValue() const;// 获取离散数值

  protected:
    // Inherited 初始化操作
    void DoInitialize() override;// Inherited，初始化操作
    void DoDispose() override;// Inherited，释放资源操作

    uint32_t m_n;// 离散空间大小
    uint32_t m_value;// 当前数值
};

//类模板表示一个可以存储任意数据类型（通过模板参数 T）的 Box 空间数据容器。
//其中包含了一些基本的操作，比如添加值、获取值、设置整个数据等。这样的数据容器可以用于表示具有不同形状和数据类型的观测值或动作空间。
template <typename T = float>
class OpenGymBoxContainer : public OpenGymDataContainer
{
  public:
    OpenGymBoxContainer();
    OpenGymBoxContainer(std::vector<uint32_t> shape);
    ~OpenGymBoxContainer() override;

    static TypeId GetTypeId();

    ns3_ai_gym::DataContainer GetDataContainerPbMsg() override;

    void Print(std::ostream& where) const override;

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymBoxContainer> container)
    {
        container->Print(os);
        return os;
    }

    bool AddValue(T value); // 添加一个值到数据容器
    T GetValue(uint32_t idx);// 获取数据容器中指定索引处的值

    bool SetData(std::vector<T> data);// 设置整个数据容器的数据
    std::vector<T> GetData(); // 获取整个数据容器的数据

    std::vector<uint32_t> GetShape();// 获取数据容器的形状

  protected:
    // Inherited
    void DoInitialize() override;// 初始化函数
    void DoDispose() override;// 释放资源函数

  private:
    void SetDtype();// 设置数据类型（Dtype）
    std::vector<uint32_t> m_shape;// 数据容器的形状
    ns3_ai_gym::Dtype m_dtype; // 数据容器的数据类型
    std::vector<T> m_data;// 数据容器的数据
};

template <typename T>
TypeId
OpenGymBoxContainer<T>::GetTypeId()
{
    std::string name = TypeNameGet<T>();
    static TypeId tid = TypeId("ns3::OpenGymBoxContainer<" + name + ">")
                            .SetParent<Object>()
                            .SetGroupName("OpenGym")
                            .template AddConstructor<OpenGymBoxContainer<T>>();
    return tid;
}

template <typename T>
OpenGymBoxContainer<T>::OpenGymBoxContainer()
{
    SetDtype();
}

template <typename T>
OpenGymBoxContainer<T>::OpenGymBoxContainer(std::vector<uint32_t> shape)
    : m_shape(shape)
{
    SetDtype();
}

template <typename T>
OpenGymBoxContainer<T>::~OpenGymBoxContainer()
{
}

template <typename T>
void
OpenGymBoxContainer<T>::SetDtype()
{
    std::string name = TypeNameGet<T>();
    if (name == "int8_t" || name == "int16_t" || name == "int32_t" || name == "int64_t")
    {
        m_dtype = ns3_ai_gym::INT;
    }
    else if (name == "uint8_t" || name == "uint16_t" || name == "uint32_t" || name == "uint64_t")
    {
        m_dtype = ns3_ai_gym::UINT;
    }
    else if (name == "float")
    {
        m_dtype = ns3_ai_gym::FLOAT;
    }
    else if (name == "double")
    {
        m_dtype = ns3_ai_gym::DOUBLE;
    }
    else
    {
        m_dtype = ns3_ai_gym::FLOAT;
    }
}

template <typename T>
void
OpenGymBoxContainer<T>::DoDispose()
{
}

template <typename T>
void
OpenGymBoxContainer<T>::DoInitialize()
{
}

/*
 OpenGymBoxContainer 类的数据转换为 Protocol Buffers 消息，以便进行序列化和在不同平台或语言之间传递。
 函数首先创建 DataContainer 和 BoxDataContainer 对象，然后填充这些对象的字段，最后将 BoxDataContainer 数据打包到 DataContainer 中。
*/
template <typename T>
ns3_ai_gym::DataContainer
OpenGymBoxContainer<T>::GetDataContainerPbMsg()
{
    ns3_ai_gym::DataContainer dataContainerPbMsg; //// 创建 DataContainer 对象，用于存储 BoxDataContainer 数据
    ns3_ai_gym::BoxDataContainer boxContainerPbMsg;//// 创建 BoxDataContainer 对象

    std::vector<uint32_t> shape = GetShape();// 获取容器的形状
    *boxContainerPbMsg.mutable_shape() = {shape.begin(), shape.end()};// 设置 BoxDataContainer 对象的形状，
    //在这里mutable_shape()返回一个指向 google::protobuf::RepeatedField<uint32_t> 类型的指针，而*则用于解引用这个指针，访问这个 RepeatedField<uint32_t> 对象。
    //{shape.begin(), shape.end()}表示从 shape 容器的起始迭代器到结束迭代器的范围。

    boxContainerPbMsg.set_dtype(m_dtype);// 设置 BoxDataContainer 对象的数据类型
    std::vector<T> data = GetData(); // 获取容器的数据

    if (m_dtype == ns3_ai_gym::INT)// 根据数据类型设置 BoxDataContainer 对象的数据字段
    {
        *boxContainerPbMsg.mutable_intdata() = {data.begin(), data.end()};
    }
    else if (m_dtype == ns3_ai_gym::UINT)
    {
        *boxContainerPbMsg.mutable_uintdata() = {data.begin(), data.end()};
    }
    else if (m_dtype == ns3_ai_gym::FLOAT)
    {
        *boxContainerPbMsg.mutable_floatdata() = {data.begin(), data.end()};
    }
    else if (m_dtype == ns3_ai_gym::DOUBLE)
    {
        *boxContainerPbMsg.mutable_doubledata() = {data.begin(), data.end()};
    }
    else
    {
        // 默认使用 FLOAT 类型
        *boxContainerPbMsg.mutable_floatdata() = {data.begin(), data.end()};
    }

  // 设置 DataContainer 对象的类型为 Box，将 BoxDataContainer 数据打包到 DataContainer 中
    dataContainerPbMsg.set_type(ns3_ai_gym::Box);
    dataContainerPbMsg.mutable_data()->PackFrom(boxContainerPbMsg);
    return dataContainerPbMsg;// 返回 DataContainer 对象
}

template <typename T>
bool
OpenGymBoxContainer<T>::AddValue(T value)
{
    m_data.push_back(value);
    return true;
}

template <typename T>
T
OpenGymBoxContainer<T>::GetValue(uint32_t idx)
{
    T data = 0;
    if (idx < m_data.size())
    {
        data = m_data.at(idx);
    }
    return data;
}

template <typename T>
bool
OpenGymBoxContainer<T>::SetData(std::vector<T> data)
{
    m_data = data;
    return true;
}

template <typename T>
std::vector<uint32_t>
OpenGymBoxContainer<T>::GetShape()
{
    return m_shape;
}

template <typename T>
std::vector<T>
OpenGymBoxContainer<T>::GetData()
{
    return m_data;
}

template <typename T>
void
OpenGymBoxContainer<T>::Print(std::ostream& where) const
{
    where << "[";// 打印数组开始的左方括号
    for (auto i = m_data.begin(); i != m_data.end(); ++i)// 遍历数据容器中的元素
    {
        where << std::to_string(*i); // 打印当前元素的字符串表示形式
        auto i2 = i;// 获取下一个迭代器
        i2++;
        if (i2 != m_data.end())// 如果下一个元素存在，打印逗号和空格，否则打印右方括号
        {
            where << ", ";
        }
    }
    where << "]";
}
//这个类 OpenGymTupleContainer 的作用是表示一个元组（Tuple）容器，其中可以存储多个子容器。每个子容器可以是不同类型的 OpenGymDataContainer。
class OpenGymTupleContainer : public OpenGymDataContainer
{
  public:
    OpenGymTupleContainer();
    ~OpenGymTupleContainer() override;

    static TypeId GetTypeId();

    ns3_ai_gym::DataContainer GetDataContainerPbMsg() override;

    void Print(std::ostream& where) const override;

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymTupleContainer> container)
    {
        container->Print(os);
        return os;
    }

    bool Add(Ptr<OpenGymDataContainer> space); // 向元组容器中添加子容器
    Ptr<OpenGymDataContainer> Get(uint32_t idx);// 获取元组容器中指定索引位置的子容器

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

    std::vector<Ptr<OpenGymDataContainer>> m_tuple;// 存储子容器的数组
};

class OpenGymDictContainer : public OpenGymDataContainer
{
  public:
    OpenGymDictContainer();
    ~OpenGymDictContainer() override;

    static TypeId GetTypeId();

    ns3_ai_gym::DataContainer GetDataContainerPbMsg() override;

    void Print(std::ostream& where) const override;

    friend std::ostream& operator<<(std::ostream& os, const Ptr<OpenGymDictContainer> container)
    {
        container->Print(os);
        return os;
    }

    bool Add(std::string key, Ptr<OpenGymDataContainer> value);// 向字典中添加键值对的函数
    Ptr<OpenGymDataContainer> Get(std::string key); // 根据键获取值的函数

  protected:
    // Inherited
    void DoInitialize() override;
    void DoDispose() override;

  // 内部成员变量，用于存储字典
    std::map<std::string, Ptr<OpenGymDataContainer>> m_dict;//类的内部成员变量 m_dict 是一个 std::map 类型的字典，用于存储键值对。
};

} // end of namespace ns3

#endif /* OPENGYM_CONTAINER_H */
