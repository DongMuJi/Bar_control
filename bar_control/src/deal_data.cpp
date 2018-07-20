#include "ros/ros.h"
#include "stdlib.h"
#include "math.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/Empty.h>
#include "bar_control/sensor.h"



class BarControl 
{  
float linear_limit,angular_limit;
float proc_x_two_hands,proc_y_two_hands,proc_x_one_hand,proc_y_one_hand;
std::string topic_cmd;

public:  
  BarControl()  
  {  
    n_.param<float>("linear_limit", linear_limit, 1.0);
    n_.param<float>("angular_limit", angular_limit, 1.0);
    n_.param<float>("proc_x_two_hands", proc_x_two_hands, 0);
    n_.param<float>("proc_y_two_hands", proc_y_two_hands,0.1);
    n_.param<float>("proc_x_one_hand", proc_x_one_hand, 0);
    n_.param<float>("proc_y_one_hand", proc_y_one_hand,0.1);
    n_.param<std::string>("topic_cmd", topic_cmd, "/cmd_vel_mux/input/teleop");
    sub_ = n_.subscribe("force_sensor_data", 1000,  &BarControl::chatterCallback,this);
    pub_ = n_.advertise<geometry_msgs::Twist>(topic_cmd, 1000);
  }



  float SpeedChangeOne(float a,float b)
  {
     float speed1;
     if(a>0)
     {
       if(b<0) //single  push turn left
       speed1 = a*(angular_limit/5000);
       else if(b>0) //single push turn right
       speed1 = a*((-angular_limit)/5000);
     }
     else if(a<0)
     {
       if(b>0) //single poll turn left
       speed1 = a*(angular_limit/5000);
       else if(b<0) //single poll turn right
       speed1 = a*((-angular_limit)/5000);
     }
     return speed1;    
  }

  float SpeedChangeTwo(float a,float b)
  {
      float speed2;
      if(b>0)//double turn right
      speed2 = a*((-angular_limit)/5000);
      else if(b<0)//double turn left
      speed2 = a*(angular_limit/5000);
      else
      speed2 = a*(linear_limit/5000);// double go forward or back
      return speed2;
  }



  void chatterCallback(const bar_control::sensor& msg)
  {
    int hand;
    float force_data,theta_data,f_speed;
    geometry_msgs::Twist cmd;
    hand = msg.hand;
    force_data = msg.force_data;
    theta_data = msg.theta_data;
    if (hand == 0)
    {
        cmd.linear.x = 0;
        cmd.linear.y = 0;
        cmd.linear.z = 0;
        cmd.angular.x = 0;
        cmd.angular.y = 0;
        cmd.angular.z = 0;
     }
     else if(hand == 1)
     {
        f_speed = SpeedChangeOne(force_data,theta_data);
        if(force_data>0)
        {
          cmd.linear.x = proc_x_one_hand*fabs(f_speed);
          cmd.linear.y = proc_y_one_hand*f_speed;
          cmd.angular.z = f_speed;
        }
        else if(force_data<0)
        {
          cmd.linear.x = proc_x_one_hand*(-fabs(f_speed));
          cmd.linear.y = proc_y_one_hand*(-f_speed);
          cmd.angular.z = f_speed;
        }
        
     }
     else if(hand == 2)
     {
        f_speed = SpeedChangeTwo(force_data,theta_data);
        if(theta_data != 0)
        {
            cmd.linear.x = proc_x_two_hands*fabs(f_speed);
            cmd.linear.y = proc_y_two_hands*f_speed;
            cmd.angular.z = f_speed;
        }
        else
        {
            cmd.linear.x = f_speed;
            cmd.linear.y = 0;
            cmd.angular.z = 0;
        }

      }
       pub_.publish(cmd);
   }
private:  
  ros::NodeHandle n_;   
  ros::Publisher pub_;  
  ros::Subscriber sub_;
  
};
  
int main(int argc, char **argv)  
{  
  ros::init(argc, argv, "subscribe_and_publish"); 
  BarControl BCObject;    
  ros::spin();    
  return 0;  
}  


















