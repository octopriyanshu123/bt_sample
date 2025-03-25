#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <cstdlib>  // For std::rand and RAND_MAX
#include <ctime>    // For std::time

class RandomNumberPublisher
{
public:
  RandomNumberPublisher()
  {
    // Initialize the ROS node handle
    ros::NodeHandle nh;

    // Initialize the publisher for the "random_number_topic"
    publisher_ = nh.advertise<std_msgs::Int32>("random_number_topic", 10);

    // Seed the random number generator with the current time
    std::srand(static_cast<unsigned int>(std::time(0)));
  }

  void publishRandomNumber()
  {
    std_msgs::Int32 msg;

    // Generate a random number between 0 and 100 (inclusive)
    msg.data = std::rand() % 101;  // Random number in range [0, 100]

    // Publish the message to the topic
    publisher_.publish(msg);

    // Log the published random number to ROS console
    ROS_INFO("Published random number: %d", msg.data);
  }

private:
  ros::Publisher publisher_;
};

int main(int argc, char** argv)
{
  // Initialize the ROS system
  ros::init(argc, argv, "random_number_publisher");

  // Create an instance of the RandomNumberPublisher class
  RandomNumberPublisher random_number_publisher;

  // Create a ROS loop at a frequency of 1Hz (1 second)
  ros::Rate rate(1);

  // Keep publishing random numbers until ROS is shut down
  while (ros::ok())
  {
    random_number_publisher.publishRandomNumber();
    ros::spinOnce();
    rate.sleep();  // Sleep to maintain the 1Hz loop rate
  }

  return 0;
}
