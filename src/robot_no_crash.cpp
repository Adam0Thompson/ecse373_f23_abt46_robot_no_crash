#include "ros/ros.h"
#include "std_msgs/String.h"

#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"

#include <sstream>


ros::Publisher *p_cmd_vel_pub;
/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */


bool stop_moving = 0;
// Create a float variable to hold the parameter
double wall_dist = 0.0;
void laserScanCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
  for (int indx = 45; indx < 225; indx++){
    if(msg->ranges[indx] < wall_dist) {
      stop_moving = 1;
      break;
    }
    else{
      stop_moving = 0;
    }
  }

  if (stop_moving) {
    ROS_INFO_THROTTLE(1, "Stop command sent");
  }

}
geometry_msgs::Twist to_send;
void desVelCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  to_send = *msg;

  if (stop_moving){
    to_send.linear.x = to_send.linear.x > 0? 0 : to_send.linear.x;
  }

  p_cmd_vel_pub->publish(to_send);
  //ROS_INFO_THROTTLE(1.0, "Got a command");
}
int main(int argc, char **argv)
{
  
  
    
  
  
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Subscriber sub = n.subscribe("laser_1", 10, laserScanCallback);
  ros::Subscriber sub_1 = n.subscribe("des_vel", 10, desVelCallback);

  ros::Publisher cmd_vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  p_cmd_vel_pub = &cmd_vel_pub; // Keeps track of address
  ros::Rate loop_rate(10);

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  // int count = 0;

  
  if (ros::param::getCached("wall_dist", wall_dist)){
    ROS_INFO("wall_dist is: [%2.2f]", wall_dist);
  }
  else{
    ROS_ERROR("Failed to get param 'wall_dist'");
  }
  while (ros::ok())
  {
    ros::param::getCached("wall_dist", wall_dist);
    // ROS_INFO_THROTTLE(10, "wall_dist is %2.2f", wall_dist);
    // if (ros::param::getCached("wall_dist", wall_dist)){
    //   ROS_INFO("wall_dist was updated to: [%2.2f]", wall_dist);
    // }
    // /**
    //  * This is a message object. You stuff it with data, and then publish it.
    //  */
    // std_msgs::String msg;

    // std::stringstream ss;
    // ss << "hello world " << count;
    // msg.data = ss.str();

    // ROS_INFO("%s", msg.data.c_str());

    // /**
    //  * The publish() function is how you send messages. The parameter
    //  * is the message object. The type of this object must agree with the type
    //  * given as a template parameter to the advertise<>() call, as was done
    //  * in the constructor above.
    //  */
    // chatter_pub.publish(msg);
    


    ros::spinOnce();

    loop_rate.sleep();
    //++count;
  }


  return 0;
}