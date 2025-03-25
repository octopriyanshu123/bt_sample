#include "ros/ros.h"
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "std_msgs/Int32.h"
#include <behaviortree_cpp_v3/bt_factory.h>
#include <behaviortree_cpp_v3/loggers/bt_cout_logger.h>
#include <behaviortree_cpp_v3/action_node.h>
#include <actionlib/client/simple_action_client.h>
#include "behaviortree_cpp_v3/behavior_tree.h"
#include "behaviortree_cpp_v3/control_node.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <functional>
#include <std_msgs/String.h>
#include <sstream>
#include <string>


/*   TODO 

This is just the sample node for the Bt
I try to make the all possiable combination of the node 

Done
SyncActionNode   one thread
AsyncActionNode  diff thread
CoroActionNode   async Wait

ConditionNode
sequence
fallback

Repeat           repeat the task


Left



*/


class GenerateRandomNumber : public BT::CoroActionNode
{
public:
    GenerateRandomNumber(const std::string& name, const BT::NodeConfiguration& config)
        : BT::CoroActionNode(name, config)
    {
    }

    // Declare the port for the node
    static BT::PortsList providedPorts()
    {
        return { BT::OutputPort<int>("RandNum") };  // Declare 'RandNum' as an output port
    }

    // Override tick method to generate a random number
    BT::NodeStatus tick() override
    {
        // Generate a random number between 0 and 100
        int random_number = rand() % 100;  // Generates a random number between 0 and 99

        // Print or store the generated number for further use
        std::cout << "Generated Random Number: " << random_number << std::endl;

        // Set the output port value (e.g., `RandNum`)
        setOutput("RandNum", random_number);

        return BT::NodeStatus::SUCCESS;
    }
};




// class GenerateRandomNumber : public BT::SyncActionNode
// {
// public:
//   explicit GenerateRandomNumber(const std::string &name, const BT::NodeConfiguration &config)
//       : BT::SyncActionNode(name, config)
//   {
//     // Initialize ROS NodeHandle
//     ros::NodeHandle nh;
//     // Initialize the subscriber to the /rand_num topic
//     subscriber_ = nh.subscribe("/random_number_topic", 10, &GenerateRandomNumber::randomNumberCallback, this);
//     // Initialize the random_value to a default value
//     random_value_ = 0;
//   }

//   static BT::PortsList providedPorts()
//   {
//     return {BT::OutputPort<int>("random_value")};
//   }

//   BT::NodeStatus tick() override
//   {
//     // Wait for the random value to be received from the subscriber
//     ros::spinOnce(); // Process incoming messages
//     ROS_INFO("Received Random Value: %d", random_value_);
//     // Store the received random value in the blackboard
//     setOutput("random_value", random_value_);
//     // Return SUCCESS as long as we have received a value
//     return BT::NodeStatus::SUCCESS;
//   }

// private:
//   ros::Subscriber subscriber_; // Subscriber to listen to /rand_num topic
//   int random_value_;           // Variable to store the received random number

//   // Callback function to handle the incoming message
//   void randomNumberCallback(const std_msgs::Int32::ConstPtr &msg)
//   {
//     random_value_ = msg->data; // Store the received random number
//   }
// };


class CheckOddOrEven : public BT::ConditionNode
{
public:
    CheckOddOrEven(const std::string& name, const BT::NodeConfiguration& config)
        : BT::ConditionNode(name, config)
    {
    }

    // Declare the port for the node
    static BT::PortsList providedPorts()
    {
        return { BT::InputPort<int>("RandNum") };  // Declare 'RandNum' as an input port
    }

    // Override tick method to check if the number is odd or even
    BT::NodeStatus tick() override
    {
        int rand_num = 0;

        // Get the input value from the 'RandNum' port
        if (!getInput<int>("RandNum", rand_num))
        {
            std::cerr << "Failed to get input 'RandNum'" << std::endl;
            return BT::NodeStatus::FAILURE;
        }

        // Check if the number is even or odd
        if (rand_num % 2 == 0)
        {
            std::cout << rand_num << " is Even." << std::endl;
            return BT::NodeStatus::SUCCESS;  // Return success if even
        }
        else
        {
            std::cout << rand_num << " is Odd." << std::endl;
            return BT::NodeStatus::FAILURE;  // Return failure if odd
        }
    }
};

// class CheckOddOrEven : public BT::SyncActionNode
// {
// public:
//   explicit CheckOddOrEven(const std::string &name, const BT::NodeConfiguration &config)
//       : BT::SyncActionNode(name, config)
//   {
//   }
//   static BT::PortsList providedPorts()
//   {
//     return {BT::InputPort<int>("random_value"), BT::OutputPort<bool>("is_even"), BT::OutputPort<bool>("is_odd")};
//   }
//   BT::NodeStatus tick() override
//   {
//     int random_value;
//     if (!getInput("random_value", random_value))
//     {
//       ROS_ERROR("Failed to retrieve 'random_value' from the blackboard");
//       return BT::NodeStatus::FAILURE;
//     }
//     bool is_even = (random_value % 2 == 0);
//     bool is_odd = (random_value % 2 != 0);
//     setOutput("is_even", is_even);
//     setOutput("is_odd", is_odd);
//     // ROS_INFO("Number %d is %s", random_value, is_even ? "Even" : "Odd");
//     return BT::NodeStatus::SUCCESS;
//   }
// };

class HandleEvenNumber : public BT::SyncActionNode
{
public:
  explicit HandleEvenNumber(const std::string &name, const BT::NodeConfiguration &config)
      : BT::SyncActionNode(name, config)
  {
    ros::NodeHandle nh;
    publisher_ = nh.advertise<std_msgs::String>("odd_or_even_topic", 10);
  }

  static BT::PortsList providedPorts()
  {
    // No ports are needed for this node
    return {BT::InputPort<int>("random_value")};
  }

  BT::NodeStatus tick() override
  {
    int random_value;
    if (!getInput("random_value", random_value))
    {
      throw BT::RuntimeError("Missing required input [random_value]");
    }
    ros::NodeHandle nh;
    publisher_ = nh.advertise<std_msgs::String>("odd_or_even_topic", 10);
    std_msgs::String msg;
    std::stringstream ss;
    ss << random_value;
    msg.data = "The number is Odd! Random value: " + ss.str();
    publisher_.publish(msg);
    return BT::NodeStatus::SUCCESS;
  }

private:
  ros::Publisher publisher_;
};

class HandleOddNumber : public BT::SyncActionNode
{
public:
  explicit HandleOddNumber(const std::string &name, const BT::NodeConfiguration &config)
      : BT::SyncActionNode(name, config)
  {
  }
  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<int>("random_value")};
  }
  BT::NodeStatus tick() override
  {
    int random_value;
    if (!getInput("random_value", random_value))
    {
      throw BT::RuntimeError("Missing required input [random_value]");
    }
    ros::NodeHandle nh;
    publisher_ = nh.advertise<std_msgs::String>("odd_or_even_topic", 10);
    std_msgs::String msg;
    std::stringstream ss;
    ss << random_value;
    msg.data = "The number is Even! Random value: " + ss.str();
    publisher_.publish(msg);
    return BT::NodeStatus::SUCCESS;
  }

private:
  ros::Publisher publisher_;
};




class CheckIfEven : public BT::ConditionNode
{
public:
  explicit CheckIfEven(const std::string &name, const BT::NodeConfiguration &config)
      : BT::ConditionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<bool>("is_even")}; //, BT::InputPort<int>("random_value")
  }

  BT::NodeStatus tick() override
  {
    bool is_even;
    if (!getInput("is_even", is_even))
    {
      throw BT::RuntimeError("Missing required input [is_even]");
    }
    //  int random_value;
    // if(!getInput("random_value",random_value)){
    //   throw BT::RuntimeError("Missing required input [random_value]");
    // }
    // ros::NodeHandle nh;
    // publisher_ = nh.advertise<std_msgs::String>("odd_or_even_topic", 10);
    // std_msgs::String msg;
    // std::stringstream ss;
    // ss << random_value;

    // if (is_even)
    // {
    // msg.data = "The number is even! Random value: " + ss.str();
    // publisher_.publish(msg);

    // }
    return is_even ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
  }
  // private:
  // ros::Publisher publisher_;
};


class CheckIfOdd : public BT::ConditionNode
{
public:
  explicit CheckIfOdd(const std::string &name, const BT::NodeConfiguration &config)
      : BT::ConditionNode(name, config)
  {
  }

  static BT::PortsList providedPorts()
  {
    return {BT::InputPort<bool>("is_odd")}; // , BT::InputPort<int>("random_value")
  }

  BT::NodeStatus tick() override
  {
    bool is_odd;
    if (!getInput("is_odd", is_odd))
    {
      throw BT::RuntimeError("Missing required input [is_odd]");
    }
    // int random_value;
    // if(!getInput("random_value",random_value)){
    //   throw BT::RuntimeError("Missing required input [random_value]");
    // }
    // ros::NodeHandle nh;
    // publisher_ = nh.advertise<std_msgs::String>("odd_or_even_topic", 10);
    // std_msgs::String msg;
    // std::stringstream ss;
    // ss << random_value;

    // if (is_odd)
    // {
    // msg.data = "The number is odd! Random value: " + ss.str();
    //     publisher_.publish(msg);
    //     }
    return is_odd ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
  }
};

int main(int argc, char **argv)
{
  // Initialize ROS node
  ros::init(argc, argv, "bt_ros_node");
  ros::NodeHandle nh;

  // Create BehaviorTree factory
  BT::BehaviorTreeFactory factory;

  // Register the custom nodes with the factory

  factory.registerNodeType<GenerateRandomNumber>("GenerateRandomNumber");
  factory.registerNodeType<CheckOddOrEven>("CheckOddOrEven");
  factory.registerNodeType<HandleEvenNumber>("HandleEvenNumber");
  factory.registerNodeType<HandleOddNumber>("HandleOddNumber");
  factory.registerNodeType<CheckIfEven>("CheckIfEven");
  factory.registerNodeType<CheckIfOdd>("CheckIfOdd");

  // factory.registerNodeType<ApproachObject>("ApproachObject");
  // factory.registerSimpleCondition("CheckBattery", std::bind(CheckBattery));
  // factory.registerNodeType<CheckBattery>("CheckBattery");

  // GripperInterface gripper;

  // factory.registerSimpleAction(
  //     "OpenGripper",
  //     std::bind(&GripperInterface::open, &gripper));

  // factory.registerSimpleAction(
  //     "CloseGripper",
  //     std::bind(&GripperInterface::close, &gripper));

  // Create a blackboard for storing data between nodes
  BT::Blackboard::Ptr blackboard = BT::Blackboard::create();

  // Create Behavior Tree with blackboard
//   std::string xml_file = "/home/octo/catkin_ws/src/bt_sample/bt_xml/bt_sample.xml"; // Provide the correct path
//   auto tree = factory.createTreeFromFile(xml_file);

std::string xml_text = R"(
    <root BTCPP_format="3">
<!--  //////////  -->
    <BehaviorTree ID="Untitled">
    <Repeat num_cycles="15">
    <Sequence>
    <Action ID="GenerateRandomNumber" RandNum="0"/>
    <Condition ID="CheckOddOrEven" RandNum="0"/>
    </Sequence>
    </Repeat>
    </BehaviorTree>
    <!--  Description of Node Models (used by Groot)  -->
    <TreeNodesModel>
    <Condition ID="CheckOddOrEven" editable="true">
    <input_port name="RandNum" default="0">Random Num Generated By Action Node</input_port>
    </Condition>
    <Action ID="GenerateRandomNumber" editable="true">
    <output_port name="RandNum" default="0">Random Num Output</output_port>
    </Action>
    </TreeNodesModel>

    </root>)";

    std::cout << "Loaded XML:\n" << xml_text << std::endl;

    auto tree = factory.createTreeFromText(xml_text, blackboard);


  // Set the blackboard for the tree root
//    tree.rootNode()->setBlackboard(blackboard);  // The blackboard must be set individually for each node when needed.
  BT::StdCoutLogger logger(tree);

  // Main ROS loop to execute the Behavior Tree
  ros::Rate rate(10); // Loop at 10 Hz
//   while (ros::ok())
//   {
    tree.tickRoot(); // Execute the root node (which is a Sequence)
    ros::spinOnce(); // Process ROS callbacks if there are any
    rate.sleep();    // Sleep to maintain the loop rate
                     // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //}
  return 0;
}