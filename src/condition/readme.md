To create the perfect example of both `Condition` and `ScriptCondition` decorators based on the code you provided, let's first review the provided code and then expand it to create examples of how these could be structured.

### Overview of `ConditionNode` Example (`CheckIfOdd`):
The provided code defines a custom Behavior Tree node `CheckIfOdd`, which checks whether a number is odd or not. It uses an input port `is_odd` to determine if the number is odd, returning either `SUCCESS` or `FAILURE` based on the boolean value of `is_odd`. If the required input is missing, it throws an error.

Now, let's break this down and provide both the **Condition** and **ScriptCondition** examples based on this structure.

### 1. **Condition Example (CheckIfOdd)**

In this example, we will use a simple `ConditionNode` to check if a value is odd or even, leveraging the existing code structure you provided. This example will be a "pure condition" node, meaning it will evaluate the condition (odd or even) and return success or failure.

#### Code for `ConditionNode`:
```cpp
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <string>
#include <stdexcept>

class CheckIfOdd : public BT::ConditionNode
{
public:
    explicit CheckIfOdd(const std::string &name, const BT::NodeConfiguration &config)
        : BT::ConditionNode(name, config)
    {
    }

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<bool>("is_odd")}; // Only needs one input port for is_odd
    }

    BT::NodeStatus tick() override
    {
        bool is_odd;
        if (!getInput("is_odd", is_odd))
        {
            throw BT::RuntimeError("Missing required input [is_odd]");
        }

        // Check if the number is odd
        return is_odd ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
    }
};
```

#### Explanation:
- **ConditionNode**: A behavior tree node that checks a condition and returns `SUCCESS` or `FAILURE`.
- **Input Port**: The condition checks the input port `is_odd`, which is a boolean indicating whether the number is odd or not.
- **Tick Function**: In the `tick()` function, the condition is evaluated and the status is returned accordingly:
  - **Success** if the number is odd.
  - **Failure** if the number is even.

This can be used in a behavior tree to conditionally execute other nodes based on whether the value is odd.

---

### 2. **ScriptCondition Example**

The `ScriptCondition` is typically used when you want to execute a condition that involves more complex logic, such as using external data or performing some computation. It could involve logic like interacting with ROS publishers or external services, like in the commented-out code.

In the **ScriptCondition** example, we'll use the `CheckIfOdd` condition with additional logic (e.g., publishing a message to a topic in ROS) if the condition is met.

#### Code for `ScriptConditionNode`:

```cpp
#include <behaviortree_cpp_v3/behavior_tree.h>
#include <std_msgs/String.h>
#include <ros/ros.h>
#include <sstream>

class CheckIfOddWithPublisher : public BT::ConditionNode
{
public:
    explicit CheckIfOddWithPublisher(const std::string &name, const BT::NodeConfiguration &config)
        : BT::ConditionNode(name, config), nh_("~")
    {
        publisher_ = nh_.advertise<std_msgs::String>("odd_or_even_topic", 10);
    }

    static BT::PortsList providedPorts()
    {
        return {BT::InputPort<bool>("is_odd"), BT::InputPort<int>("random_value")};
    }

    BT::NodeStatus tick() override
    {
        bool is_odd;
        int random_value;

        // Check if 'is_odd' input is provided
        if (!getInput("is_odd", is_odd))
        {
            throw BT::RuntimeError("Missing required input [is_odd]");
        }

        // Check if 'random_value' input is provided
        if (!getInput("random_value", random_value))
        {
            throw BT::RuntimeError("Missing required input [random_value]");
        }

        std_msgs::String msg;
        std::stringstream ss;
        ss << random_value;

        if (is_odd)
        {
            msg.data = "The number is odd! Random value: " + ss.str();
            publisher_.publish(msg);
        }
        else
        {
            msg.data = "The number is even! Random value: " + ss.str();
            publisher_.publish(msg);
        }

        // Return success or failure based on the odd/even condition
        return is_odd ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
    }

private:
    ros::NodeHandle nh_;
    ros::Publisher publisher_;
};
```

#### Explanation:
- **ScriptConditionNode**: This extends the condition node with more complex behavior. In this case, after checking whether the number is odd or even, it publishes a message to a ROS topic with the random value.
- **ROS Publisher**: If the number is odd, it publishes a message saying "The number is odd!"; otherwise, it says "The number is even!".
- **Input Ports**: This script node requires two input ports:
  - `is_odd`: A boolean indicating whether the number is odd.
  - `random_value`: An integer value to be included in the message.
  
#### Behavior:
- **Odd Condition**: If the number is odd, the node publishes "The number is odd!" along with the random value.
- **Even Condition**: If the number is even, it publishes "The number is even!" with the random value.
- After publishing, the node returns either `SUCCESS` (for odd numbers) or `FAILURE` (for even numbers).

---

### How to Use These Nodes in a Behavior Tree

#### Example Behavior Tree (XML or Code):

```xml
<root>
    <Sequence name="root_sequence">
        <CheckIfOdd is_odd="true" />
        <CheckIfOddWithPublisher is_odd="true" random_value="7" />
    </Sequence>
</root>
```

- **CheckIfOdd**: This node will simply check if the number is odd and return `SUCCESS` or `FAILURE`.
- **CheckIfOddWithPublisher**: This node checks if the number is odd and also publishes a message to a ROS topic.

#### Example in C++:

```cpp
#include <behaviortree_cpp_v3/behavior_tree.h>

int main()
{
    // Define the behavior tree
    BT::BehaviorTreeFactory factory;

    factory.registerNodeType<CheckIfOdd>("CheckIfOdd");
    factory.registerNodeType<CheckIfOddWithPublisher>("CheckIfOddWithPublisher");

    // Create a tree from the XML file or build the tree programmatically
    BT::Tree tree = factory.createTreeFromFile("behavior_tree.xml");

    // Tick the tree to start execution
    while (true)
    {
        tree.tickRoot();
    }

    return 0;
}
```

This setup shows the basic structure of integrating both simple conditions (`CheckIfOdd`) and more complex script-based conditions (`CheckIfOddWithPublisher`) into a behavior tree. The nodes can be easily expanded or replaced with other conditions, behaviors, or scripts as needed.