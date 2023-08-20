# A Plus B example

## Introduction

In this example (`ns3ai_apb`), C++ side starts by setting 2 random numbers between 0 and 10 in shared memory. Then, Python side gets the
numbers and sets the sum of the numbers in shared memory (in another region). Finally, C++ gets the sum that Python set.
The procedure is analogous to C++ passing RL states to Python and Python passing RL actions back to C++, and is
repeated many times.

We created this example to demonstrate the basic usage of ns3-ai's two interfaces: Gym interface and message interface.

## Quick start guide on how to use ns3-ai

### 1. Gym interface

#### C++ side

The Gym interface turns ns3-ai into a RL playground and implements [Gymnasium](https://gymnasium.farama.org/index.html)
APIs such as `reset`, `step` and `close`.

To begin, simply inherit from `OpenGymEnv` to create our own environment. Environment is an RL terminology, here it
refers to the ns-3 program that Python interact with. Thus, the Python program will be the agent; the numbers that C++ 
set, `m_a` and `m_b`, is the state; the sum Python calculated, `m_sum`, is the action.

```c++
class ApbEnv : public OpenGymEnv
{
  public:
    ApbEnv();
    ~ApbEnv() override;
    static TypeId GetTypeId();
    void DoDispose() override;

    uint32_t GetAPlusB();
    
    // OpenGym interfaces:
    Ptr<OpenGymSpace> GetActionSpace() override;
    Ptr<OpenGymSpace> GetObservationSpace() override;
    bool GetGameOver() override;
    Ptr<OpenGymDataContainer> GetObservation() override;
    float GetReward() override;
    std::string GetExtraInfo() override;
    bool ExecuteActions(Ptr<OpenGymDataContainer> action) override;
    
    uint32_t m_a;
    uint32_t m_b;
  private:
    uint32_t m_sum;
};
```

As mentioned, C++ side sets the numbers and gets their sum from Python. This is done by `GetAPlusB()`:

```c++
uint32_t
ApbEnv::GetAPlusB()
{
    Notify();
    return m_sum;
}
```

The `Notify()` function, defined in base class, is the core of C++-Python interaction. It registers essential callbacks,
collects state and send it to Python, receives the action, and executes it. 
In the A Plus B example, the execution of action is simply storing the sum in `m_sum`. So, after `Notify()`, `m_sum` becomes the sum 
of `m_a` and `m_b`.

In order for `Notify()` to work well, some functions (use as callbacks) must be implemented:
1. `GetActionSpace`: Called when initialing Gym interface. It defines the action space of environment. In this example, action space
contains one integer between 0 and 20, so box space is applied:
```c++
Ptr<OpenGymSpace>
ApbEnv::GetActionSpace()
{
    std::vector<uint32_t> shape = {1};
    std::string dtype = TypeNameGet<uint32_t>();
    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace>(0, 20, shape, dtype);
    return box;
}
```
2. `GetObservationSpace`: Similar to `GetActionSpace`, it defines the observation space of environment, which contains two integer between 0 and 10:
```c++
Ptr<OpenGymSpace>
ApbEnv::GetObservationSpace()
{
    std::vector<uint32_t> shape = {2};
    std::string dtype = TypeNameGet<uint32_t>();
    Ptr<OpenGymBoxSpace> box = CreateObject<OpenGymBoxSpace>(0, 10, shape, dtype);
    return box;
}
```
3. `GetGameOver`: In Gym interface an environment have two ways to stop: game over or simulation end. In this example we 
prefer the latter to stop the environment. Therefore, the return value is always `false`:
```c++
bool
ApbEnv::GetGameOver()
{
    return false;
}
```
4. `GetObservation`: Function to collect observation (state) from environment. In this example, `m_a` and `m_b` are collected. 
Note that `OpenGymBoxContainer` rather than `OpenGymBoxSpace` (in space definition) is used.
```c++
Ptr<OpenGymDataContainer>
ApbEnv::GetObservation()
{
    std::vector<uint32_t> shape = {2};
    Ptr<OpenGymBoxContainer<uint32_t>> box = CreateObject<OpenGymBoxContainer<uint32_t>>(shape);

    box->AddValue(m_a);
    box->AddValue(m_b);

    return box;
}
```
5. `GetReward`: Function that define the reward (`float` type). Reward is unused in A Plus B, can be arbitrary:
```c++
float
ApbEnv::GetReward()
{
    return 0.0;
}
```
6. `GetExtraInfo`: Function that pass additional info to Python. Info is also unused, thus it is an empty value:
```c++
std::string
ApbEnv::GetExtraInfo()
{
    return "";
}
```
7. `ExecuteActions`: Function that executes the action according to the information in container. In this example, we get the 
sum of a + b from the zeroth (and the only) item of action:
```c++
bool
ApbEnv::ExecuteActions(Ptr<OpenGymDataContainer> action)
{
    Ptr<OpenGymBoxContainer<uint32_t>> box = DynamicCast<OpenGymBoxContainer<uint32_t>>(action);
    m_sum = box->GetValue(0);
    return true;
}
```

After those functions defined, the `ApbEnv` class can be created and used.

```c++
using namespace ns3;
Ptr<ApbEnv> apb = CreateObject<ApbEnv>();
```

And the interaction is seamless:

```c++
std::cout << "set: " << apb->m_a << "," << apb->m_b << ";";
std::cout << "\n";

sum = apb->GetAPlusB();

std::cout << "get: " << sum << ";";
std::cout << "\n";
```

Remember to call `NotifySimulationEnd` before exit, to properly destroy the interface:

```c++
apb->NotifySimulationEnd();
```

#### Python side

Start by importing essential modules:

```python
import ns3ai_gym_env
import gymnasium as gym
```

`import ns3ai_gym_env` registers the `ns3ai_gym_env/Ns3-v0` environment in gym.

Define the agent:

```python
class ApbAgent:

    def __init__(self):
        pass

    def get_action(self, obs, reward, done, info):

        a = obs[0]
        b = obs[1]
        act = a + b

        return [act]
```

The `get_action` is used for summing a & b (observation), and returns an array containing the sum (action).

Create the environment and initial setup:

```python
env = gym.make("ns3ai_gym_env/Ns3-v0")
ob_space = env.observation_space
ac_space = env.action_space
print("Observation space: ", ob_space, ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)
obs, info = env.reset()
reward = 0
done = False
agent = ApbAgent()
```

Interact with Python:

```python
while True:

    action = agent.get_action(obs, reward, info, done)
    # print("---action: ", action)

    obs, reward, done, _, info = env.step(action)
    # print("---obs, reward, done, info: ", obs, reward, done, info)

    if done:
        break
```

When C++ side calls `NotifySimulationEnd`, `done` becomes true and Python exits the loop.

Remember to close the environment:

```python
env.close()
```

#### Running the example

1. [Setup ns3-ai](../../install.md)
2. Run Python script first

Python side should run first, because Python script is the shared memory creator.

```bash
cd contrib/ns3-ai/examples/a_plus_b/use_gym
python apb.py
```
3. When you see the message `Created message interface, waiting for C++ side to send initial environment...`, open
   another terminal and run C++ side.

```bash
./ns3 run ns3ai_apb_gym
```

#### Output

The output is at C++ side, repeatedly printing two random numbers generated by C++ and their sum calculated by Python:

```text
set: 4,10;
get: 14;
set: 10,8;
get: 18;
set: 5,1;
get: 6;
set: 10,6;
get: 16;
......
```

### 2. Message interface (struct version)

Message interface is more low-level compared to Gym interface. In fact, interprocess communication of Gym interface is based on message interface. 
It has send and receive functions like a typical message queue. 

When there is existing `struct` or `std::vector` to be 
shared with Python, it's easier to apply message interface then Gym interface, because you don't need to convert the data to Gym spaces.
Also, both C++ and Python side can access the data directly, which save the serialization/deserialization time especially 
for sharing large amount of data or having frequent interaction.

With **struct** version of message interface, the scenario is the same with Gym interface: C++ 
sends two numbers to Python, and Python sends one sum to C++. 

With **vector** version, the scenario is different: C++ sends a vector of two numbers to Python, and Python sends a 
vector of sums to C++.

*Note: the interface may be enhanced in the future, such as inheriting from `ns3::Singleton` to avoid using local
variable, and adding attributes for easier configuration.*

#### C++ side

We have two structs for the two numbers, `env_a` and `env_b`, and their sum, `act_c`:

```c++
struct EnvStruct
{
    uint32_t env_a;
    uint32_t env_b;
}

struct ActStruct
{
    uint32_t act_c;
}
```

Start by declaring a `Ns3AiMsgInterface` (parameters will be explained later):

```c++
Ns3AiMsgInterface<EnvStruct, ActStruct> cpp_side(false, false, true);
``` 

Then, interact with Python (some initialization code is skipped). The interface is simple and intuitive. To set `temp_a` and `temp_b` 
into shared memory, just write them into `m_single_cpp2py_msg`. To get the sum, just read from `m_single_py2cpp_msg`.

```c++
for (int i = 0; i < NUM_ENV; ++i) {
    cpp_side.cpp_send_begin();
    std::cout << "set: ";
    uint32_t temp_a = distrib(gen);
    uint32_t temp_b = distrib(gen);
    std::cout << temp_a << "," << temp_b << ";";
    cpp_side.m_single_cpp2py_msg->env_a = temp_a;
    cpp_side.m_single_cpp2py_msg->env_b = temp_b;
    std::cout << "\n";
    cpp_side.cpp_send_end();

    cpp_side.cpp_recv_begin();
    std::cout << "get: ";
    std::cout << cpp_side.m_single_py2cpp_msg->act_c;
    std::cout << "\n";
    cpp_side.cpp_recv_end();
}
```

Note that before and after read or write, several **synchronization** functions are needed. `cpp_send_begin` and `cpp_send_end` 
are for sending to Python, and `cpp_recv_begin` and `cpp_recv_end` are for receiving from Python. There are similar 
functions for Python side. These functions are based on semaphore, which requires strict sequence. If C++ side is receiving 
at the beginning, then Python side must be sending at the beginning. 
[The OSTEP book](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf) has a good introduction of semaphores.

##### Parameters of `Ns3AiMsgInterface`

Function prototype:

```c++
Ns3AiMsgInterface(bool is_memory_creator,
                  bool use_vector,
                  bool handle_finish,
                  uint32_t size = 4096,
                  const char* segment_name = "My Seg",
                  const char* cpp2py_msg_name = "My Cpp to Python Msg",
                  const char* py2cpp_msg_name = "My Python to Cpp Msg",
                  const char* lockable_name = "My Lockable");
```

- `is_memory_creator`: Controls whether this side initializes the interface. Because the shared memory is a named region, 
after the creator initializes the segment with a unique name, the other side can access the segment with that name.
- `use_vector`: Controls whether to use `std::vector` or not. 
  - If `true`, the vectors (`m_cpp2py_msg` and `m_py2cpp_msg`) must be initialized (resize to required length) before 
  use. See the Python side of vector version of this example for details.
  - If `false`, use the structs (`m_single_cpp2py_msg` and `m_single_py2cpp_msg`) for data exchange, and no initialization is required.
- `handle_finish`: Enables a simple protocol, which notifies Python side when C++ side interface is destroyed (possibly 
due to ns-3 program exit). This is useful for all applications using the message interface, because Python side is
always unaware of simulation ending. For Gym interface on top of msg interface, this function should not be enabled 
because Gym interface has its own protocol dealing with simulation ending.
- `size`: Size of Boost's `managed_shared_memory`. The default value is enough for most applications.
- `segment_name`: Unique name of the shared memory segment.
- `cpp2py_msg_name`: Unique name of the C++ to Python message inside the memory segment.
- `py2cpp_msg_name`: Unique name of the Python to C++ message inside the memory segment.
- `lockable_name`: Unique name of the lockable (contains semaphores) inside the memory segment.

#### Python side

Python side interface is a **binding** of the C++ side interface. Python binding means the Python side reuses the C++
code to have the same capabilities as C++. Because the C++ code is template-based, Python binding varies with the data
structure that is being shared, and every example is unique.

The bindings in ns3-ai is created with pybind11. For detailed instructions on writing Python bindings, check out [pybind11 documentation](https://pybind11.readthedocs.io/en/stable/index.html). 
However, most bindings have similar style and fixed components. A Plus B's binding code is a good boilerplate, 
you can just modify a few lines and apply to another program.

Basically, you can follow these steps:

1. Declare the Python module to create. In this example, `ns3ai_apb_py_not_vec` will be the import name of the binding module.

```c++
PYBIND11_MODULE(ns3ai_apb_py_not_vec, m) {
    ...
}
```

2. Bind the structures. Every item that Python may access (not necessarily all items) need to be mentioned in the code.
The names in Python can be different from that in C++. For instance, `a` in Python is `env_a` in C++.

```c++
py::class_<EnvStruct>(m, "PyEnvStruct")
    .def(py::init<>())
    .def_readwrite("a", &EnvStruct::env_a)
    .def_readwrite("b", &EnvStruct::env_b);

py::class_<ActStruct>(m, "PyActStruct")
    .def(py::init<>())
    .def_readwrite("c", &ActStruct::act_c);
```

3. Bind the C++ class. Every function or member that Python may use (not necessarily all) need to be mentioned in the binding code. 
In this binding, C++-only methods such as `cpp_send_begin` is excluded because Python side never use it.

```c++
py::class_<ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>>(m, "Ns3AiMsgInterface")
    .def(py::init<bool, bool, bool, uint32_t, const char*, const char*, const char*, const char*>())
    .def("py_recv_begin", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::py_recv_begin)
    .def("py_recv_end", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::py_recv_end)
    .def("py_send_begin", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::py_send_begin)
    .def("py_send_end", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::py_send_end)
    .def("py_get_finished", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::py_get_finished)
    .def_readwrite("m_single_cpp2py_msg", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::m_single_cpp2py_msg)
    .def_readwrite("m_single_py2cpp_msg", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::m_single_py2cpp_msg)
    ;
```

The binding module is configure by CMake to generate shared library at the source directory. Therefore, the Python script
importing the binding must be also in the source directory.

Import the binding module and create message interface:

```python
import ns3ai_apb_py_not_vec as apb      # note: the code must be in the same directory with binding

rl = apb.Ns3AiMsgInterface(True, False, True, 4096, "My Seg", "My Cpp to Python Msg", "My Python to Cpp Msg", "My Lockable")
```

Interaction with C++:

```python
while True:
    rl.py_recv_begin()
    if rl.py_get_finished():
        break
    temp = rl.m_single_cpp2py_msg.a + rl.m_single_cpp2py_msg.b
    rl.py_recv_end()

    rl.py_send_begin()
    rl.m_single_py2cpp_msg.c = temp
    rl.py_send_end()
```

Remember to destroy the interface before exit:

```python
del rl
```

#### Running the example

1. [Setup ns3-ai](../../install.md)
2. Run Python script first

Python side should run first, because Python script is the shared memory creator.

```bash
cd contrib/ns3-ai/examples/a_plus_b/use_msg_stru
python apb.py
```
3. When you see the message `Created message interface, waiting for C++ side to send initial environment...`, open
   another terminal and run C++ side.

```bash
./ns3 run ns3ai_apb_msg_stru
```

#### Output

The output is same with Gym interface.

### 3. Message interface (vector version)

This vector version A Plus B example demonstrates how C++ sends a vector of two numbers to Python, and Python sends a
vector of sums to C++.

#### C++ side

Start by declaring a `Ns3AiMsgInterface`, with `use_vector` (second parameter) set to `true`:

```c++
Ns3AiMsgInterface<EnvStruct, ActStruct> cpp_side(false, true, true);
```

Then interact with Python. Instead of `m_single_cpp2py_msg` and `m_single_py2cpp_msg` in struct version of msg interface, 
in vector version `m_cpp2py_msg` and `m_py2cpp_msg` are used, which are both `std::vector`s. The code below assumes the 
vector's size is `APB_SIZE`, and writes random numbers to the whole vector in the inner `for` loop.

```c++
for (int i = 0; i < NUM_ENV; ++i) {
    cpp_side.cpp_send_begin();
    std::cout << "set: ";
    for (int j = 0; j < APB_SIZE; ++j) {
        uint32_t temp_a = distrib(gen);
        uint32_t temp_b = distrib(gen);
        std::cout << temp_a << "," << temp_b << ";";
        cpp_side.m_cpp2py_msg->at(j).env_a = temp_a;
        cpp_side.m_cpp2py_msg->at(j).env_b = temp_b;
    }
    std::cout << "\n";
    cpp_side.cpp_send_end();

    cpp_side.cpp_recv_begin();
    std::cout << "get: ";
    for (ActStruct j: *cpp_side.m_py2cpp_msg) {
        std::cout << j.act_c << ";";
    }
    std::cout << "\n";
    cpp_side.cpp_recv_end();
}
```

The synchronization method of the vector version is the same as that of struct version.

#### Python side

Binding vector is different from binding structs, because essential methods such as `resize`, `__len__` and `__getitem__` needs 
to be implemented. The binding code is again a boilerplate and can be easily reused in other projects.

```c++
py::class_<ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector>(m, "PyEnvVector")
    .def("resize", static_cast
         <void (ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector::*)
              (ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector::size_type)>
         (&ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector::resize))
    .def("__len__", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector::size)
    .def("__getitem__", [](ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Cpp2PyMsgVector&vec, uint32_t i) -> EnvStruct & {
        if (i >= vec.size()) {
            std::cerr << "Invalid index " << i << " for vector, whose size is " << vec.size() << std::endl;
            exit(1);
        }
        return vec.at(i);
    }, py::return_value_policy::reference)
    ;

py::class_<ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector>(m, "PyActVector")
    .def("resize", static_cast
         <void (ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector::*)
              (ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector::size_type)>
         (&ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector::resize))
    .def("__len__", &ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector::size)
    .def("__getitem__", [](ns3::Ns3AiMsgInterface<EnvStruct, ActStruct>::Py2CppMsgVector&vec, uint32_t i) -> ActStruct & {
        if (i >= vec.size()) {
            std::cerr << "Invalid index " << i << " for vector, whose size is " << vec.size() << std::endl;
            exit(1);
        }
        return vec.at(i);
    }, py::return_value_policy::reference)
    ;
```

In this vector version example, the two vectors `m_cpp2py_msg` and `m_py2cpp_msg` are both initialized on Python side, 
using the `resize` method:

```python
import ns3ai_apb_py as apb
APB_SIZE = 3

rl = apb.Ns3AiMsgInterface(True, True, True, 4096, "My Seg", "My Cpp to Python Msg", "My Python to Cpp Msg", "My Lockable")
assert len(rl.m_py2cpp_msg) == 0
rl.m_py2cpp_msg.resize(APB_SIZE)
assert len(rl.m_cpp2py_msg) == 0
rl.m_cpp2py_msg.resize(APB_SIZE)
```

Interact with C++:

```python
while True:
    rl.py_recv_begin()
    rl.py_send_begin()
    if rl.py_get_finished():
        break
    for i in range(len(rl.m_cpp2py_msg)):
        rl.m_py2cpp_msg[i].c = rl.m_cpp2py_msg[i].a + rl.m_cpp2py_msg[i].b
    rl.py_recv_end()
    rl.py_send_end()
```

In the above code, `py_send_begin` is called before `py_recv_end`. This won't cause errors because C++ is not posting on 
the semaphore `m_py2cpp_empty_count` which `py_send_begin` is waiting until `py_send_end` completes.

#### Running the example

1. [Setup ns3-ai](../../install.md)
2. Run Python script first

Python side should run first, because Python script is the shared memory creator.

```bash
cd contrib/ns3-ai/examples/a_plus_b/use_msg_vec
python apb.py
```
3. When you see the message `Created message interface, waiting for C++ side to send initial environment...`, open
   another terminal and run C++ side.

```bash
./ns3 run ns3ai_apb_msg_vec
```

#### Output

The output is at C++ side, repeatedly printing vectors of two random numbers generated by C++ and their sum calculated 
by Python (default vector size is 3):

```text
set: 1,1;2,4;1,4;
get: 2;6;5;
set: 6,10;10,2;1,2;
get: 16;12;3;
set: 4,4;7,10;6,4;
get: 8;17;10;
set: 10,3;5,8;10,9;
get: 13;13;19;
......
```