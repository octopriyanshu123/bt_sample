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
#include "geometry_msgs/Twist.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <functional>
#include <std_msgs/String.h>
#include <sstream>  
#include <string>




int main(int argc, char ** argv) {
    ros::init(argc,argv,"bt_nav");
    ros::NodeHandle nh_;
    ros::Rate rate(10); // Loop at 10 Hz


    while (ros::ok()) {
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}