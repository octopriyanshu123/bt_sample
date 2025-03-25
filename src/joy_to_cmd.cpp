// #include <ros/ros.h>
// #include <sensor_msgs/Joy.h>
// #include <geometry_msgs/Twist.h>

// class JoystickControl
// {
// public:
//     JoystickControl()
//     {
//         // Initialize the ROS node
//         ros::NodeHandle nh;

//         // Publisher for cmd_vel (robot velocity control)
//         cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

//         // Subscriber to the joy topic
//         joy_sub = nh.subscribe("/joy", 10, &JoystickControl::joyCallback, this);
//     }

//     // Callback function to process the joystick input
//     void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
//     {
//         // Linear velocity (forward/backward)
//         move_cmd.linear.x = joy->axes[1];  // Forward/backward on the joystick

//         // Angular velocity (rotation)
//         move_cmd.angular.z = joy->axes[0];  // Left/right on the joystick

//         // Optionally, for button presses to control speed, or stop
//         if (joy->buttons[0] == 1)  // Assuming button 0 is used to stop
//         {
//             move_cmd.linear.x = 0;
//             move_cmd.angular.z = 0;
//         }

//         // Publish the movement command
//         cmd_vel_pub.publish(move_cmd);
//     }

// private:
//     ros::Publisher cmd_vel_pub;  // Publisher for cmd_vel topic
//     ros::Subscriber joy_sub;     // Subscriber for joy topic
//     geometry_msgs::Twist move_cmd;  // Message to send to cmd_vel
// };

// int main(int argc, char** argv)
// {
//     ros::init(argc, argv, "joystick_control");  // Initialize the node
//     JoystickControl joystick_control;           // Create the JoystickControl object

//     ros::spin();  // Keep the node running and process callbacks
//     return 0;
// }

// #include <ros/ros.h>
// #include <sensor_msgs/Joy.h>
// #include <geometry_msgs/Twist.h>

// class JoystickControl
// {
// public:
//     JoystickControl()
//     {
//         // Initialize the ROS node
//         ros::NodeHandle nh;

//         // Publisher for cmd_vel (robot velocity control)
//         cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

//         // Subscriber to the joy topic
//         joy_sub = nh.subscribe("/joy", 10, &JoystickControl::joyCallback, this);

//         // Create a timer to periodically publish cmd_vel
//         timer = nh.createTimer(ros::Duration(0.1), &JoystickControl::timerCallback, this);  // 10Hz

//         // Initialize the move command (set it to 0 initially)
//         move_cmd.linear.x = 0;
//         move_cmd.angular.z = 0;
//     }

//     // Callback function to process the joystick input
//     void joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
//     {
//         // Linear velocity (forward/backward)
//         move_cmd.linear.x = joy->axes[1];  // Forward/backward on the joystick

//         // Angular velocity (rotation)
//         move_cmd.angular.z = joy->axes[0];  // Left/right on the joystick

//         // Optionally, for button presses to control speed, or stop
//         if (joy->buttons[0] == 1)  // Assuming button 0 is used to stop
//         {
//             move_cmd.linear.x = 0;
//             move_cmd.angular.z = 0;
//         }
//     }

//     // Timer callback function to periodically publish the move_cmd
//     void timerCallback(const ros::TimerEvent& event)
//     {
//         // Publish the movement command periodically
//         cmd_vel_pub.publish(move_cmd);
//     }

// private:
//     ros::Publisher cmd_vel_pub;  // Publisher for cmd_vel topic
//     ros::Subscriber joy_sub;     // Subscriber for joy topic
//     ros::Timer timer;            // Timer for periodic publishing
//     geometry_msgs::Twist move_cmd;  // Message to send to cmd_vel
// };

// int main(int argc, char** argv)
// {
//     ros::init(argc, argv, "joystick_control");  // Initialize the node
//     JoystickControl joystick_control;           // Create the JoystickControl object

//     // Use a MultiThreadedSpinner to allow multithreading
//     ros::MultiThreadedSpinner spinner(2);  // Use 2 threads
//     spinner.spin();  // Start the spinner and allow callbacks to be processed in parallel

//     return 0;
// }

// #include <ros/ros.h>
// #include <geometry_msgs/Twist.h>
// #include <turtlesim/Pose.h>
// #include <cmath>

// double Kp_linear = 0.5;
// double Ki_linear = 0.0;
// double Kd_linear = 0.2;

// double Kp_angular = 2.0;
// double Ki_angular = 0.0;
// double Kd_angular = 1.0;

// double goal_x = 1.0;
// double goal_y = 10.0;

// turtlesim::Pose current_pose;

// double prev_error_linear = 0.0;
// double integral_linear = 0.0;

// double prev_error_angular = 0.0;
// double integral_angular = 0.0;

// void poseCallback(const turtlesim::Pose::ConstPtr& msg)
// {
//     current_pose = *msg;
// }

// geometry_msgs::Twist computePID(double goal_x, double goal_y)
// {
//     geometry_msgs::Twist cmd_vel;

//     double dx = goal_x - current_pose.x;
//     double dy = goal_y - current_pose.y;
//     double distance = sqrt(dx * dx + dy * dy);

//     double desired_angle = atan2(dy, dx);
//     double angle_error = desired_angle - current_pose.theta;

//     while (angle_error > M_PI) angle_error -= 2 * M_PI;
//     while (angle_error < -M_PI) angle_error += 2 * M_PI;

//     double p_linear = Kp_linear * distance;
//     integral_linear += Ki_linear * distance;
//     double d_linear = Kd_linear * (distance - prev_error_linear);

//     double p_angular = Kp_angular * angle_error;
//     integral_angular += Ki_angular * angle_error;
//     double d_angular = Kd_angular * (angle_error - prev_error_angular);

//     cmd_vel.linear.x = p_linear + integral_linear + d_linear;
//     cmd_vel.angular.z = p_angular + integral_angular + d_angular;

//     prev_error_linear = distance;
//     prev_error_angular = angle_error;

//     return cmd_vel;
// }

// int main(int argc, char** argv)
// {
//     ros::init(argc, argv, "turtle_pid_controller");
//     ros::NodeHandle nh;

//     ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);

//     ros::Subscriber pose_sub = nh.subscribe("/turtle1/pose", 10, poseCallback);

//     ros::Rate loop_rate(10);

//     while (ros::ok())
//     {
//         geometry_msgs::Twist cmd_vel = computePID(goal_x, goal_y);

//         cmd_vel_pub.publish(cmd_vel);

//         ros::spinOnce();
//         loop_rate.sleep();
//     }

//     return 0;
// }
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include <std_msgs/Bool.h>
#include <cmath>

double Kp_linear = 0.5;
double Ki_linear = 0.0;
double Kd_linear = 0.2;

double Kp_angular = 2.0;
double Ki_angular = 0.0;
double Kd_angular = 1.0;

turtlesim::Pose current_pose;
turtlesim::Pose goal_pose;

double prev_error_linear = 0.0;
double integral_linear = 0.0;

double prev_error_angular = 0.0;
double integral_angular = 0.0;

void poseCallback(const turtlesim::Pose::ConstPtr& msg)
{
    current_pose = *msg;
}

void goalPoseCallback(const turtlesim::Pose::ConstPtr& msg)
{
    goal_pose = *msg;
}

geometry_msgs::Twist computePID()
{
    geometry_msgs::Twist cmd_vel;

    double dx = goal_pose.x - current_pose.x;
    double dy = goal_pose.y - current_pose.y;
    double distance = sqrt(dx * dx + dy * dy);

    double desired_angle = atan2(dy, dx);
    double angle_error = desired_angle - current_pose.theta;

    while (angle_error > M_PI) angle_error -= 2 * M_PI;
    while (angle_error < -M_PI) angle_error += 2 * M_PI;

    double p_linear = Kp_linear * distance;
    integral_linear += Ki_linear * distance;
    double d_linear = Kd_linear * (distance - prev_error_linear);

    double p_angular = Kp_angular * angle_error;
    integral_angular += Ki_angular * angle_error;
    double d_angular = Kd_angular * (angle_error - prev_error_angular);

    cmd_vel.linear.x = p_linear + integral_linear + d_linear;
    cmd_vel.angular.z = p_angular + integral_angular + d_angular;

    prev_error_linear = distance;
    prev_error_angular = angle_error;

    return cmd_vel;
}

bool checkIfOnGoal()
{
    double dx = goal_pose.x - current_pose.x;
    double dy = goal_pose.y - current_pose.y;
    double distance = sqrt(dx * dx + dy * dy);

    double angle_error = goal_pose.theta - current_pose.theta;
    while (angle_error > M_PI) angle_error -= 2 * M_PI;
    while (angle_error < -M_PI) angle_error += 2 * M_PI;

    if (distance < 0.1 && fabs(angle_error) < 0.1)// && fabs(angle_error) < 0.1
    {
        return true; // Turtle is close enough to the goal pose
    }

    return false; // Turtle has not reached the goal pose yet
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "turtle_pid_controller");
    ros::NodeHandle nh;

    ros::Publisher cmd_vel_pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    ros::Publisher goal_reached_pub = nh.advertise<std_msgs::Bool>("/turtle1/is_on_goal_pose", 10);

    ros::Subscriber pose_sub = nh.subscribe("/turtle1/pose", 10, poseCallback);
    ros::Subscriber goal_pose_sub = nh.subscribe("/goal_pose", 10, goalPoseCallback);

    ros::Rate loop_rate(10);

    while (ros::ok())
    {
        geometry_msgs::Twist cmd_vel = computePID();
        cmd_vel_pub.publish(cmd_vel);

        std_msgs::Bool is_on_goal;
        is_on_goal.data = checkIfOnGoal();
        goal_reached_pub.publish(is_on_goal);

        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}


// #include "ros/ros.h"
// #include "turtlesim/Pose.h"  // Include the Pose message type
// #include <cstdlib>  // For rand() and srand()
// #include <ctime>    // For time()

// // Function to generate a random float in a given range [min, max]
// float getRandomFloat(float min, float max) {
//     float scale = rand() / (float) RAND_MAX; // Get a random float between 0 and 1
//     return min + scale * (max - min);  // Scale it to the desired range
// }

// // Function to publish random pose to a topic
// void publishRandomPose(ros::Publisher& pub) {
//     turtlesim::Pose random_pose;

//     // Generate random values for x and y between 0 and 10
//     random_pose.x = getRandomFloat(0.0, 10.0);
//     random_pose.y = getRandomFloat(0.0, 10.0);
//     random_pose.theta = getRandomFloat(0.0, 6.28);  // Random angle between 0 and 2*pi

//     // Publish the random pose message
//     pub.publish(random_pose);
//     ROS_INFO("Published random pose: x = %f, y = %f, theta = %f", random_pose.x, random_pose.y, random_pose.theta);
// }

// int main(int argc, char **argv) {
//     // Initialize ROS
//     ros::init(argc, argv, "random_pose_publisher");

//     // Create a NodeHandle
//     ros::NodeHandle nh;

//     // Create a publisher for publishing random poses to /turtle1/pose
//     ros::Publisher pose_pub = nh.advertise<turtlesim::Pose>("/turtle1/pose", 10);

//     // Seed the random number generator
//     srand(time(0));

//     // Set loop rate for publishing (e.g., 1 Hz)
//     ros::Rate loop_rate(1);

//     // Loop to continuously publish random poses
//     while (ros::ok()) {
//         publishRandomPose(pose_pub);  // Call function to publish a random pose
//         ros::spinOnce();  // Handle callbacks (if any)
//         loop_rate.sleep();  // Sleep for the remaining time to maintain the loop rate
//     }

//     return 0;
// }
