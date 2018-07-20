#include "ros/ros.h"
#include "stdlib.h"
#include "math.h"
#include "std_msgs/String.h"
#include "bar_control/serial.h"
#include "bar_control/sensor.h"
#include <serial/serial.h>
#include <std_msgs/Empty.h>


#define sBUFFERSIZE     30


class ChangeData
{  

unsigned char s_buffer[sBUFFERSIZE];
unsigned char data[9];
unsigned char Fdata[8];
unsigned char Fdata1[4];
unsigned char Fdata2[4];
serial::Serial ser;
bar_control::sensor sensor;
int count=0;
int flag=0;
float proc_sensor;

public:  
  ChangeData()  
  {  
    nb.param<float>("proc_sensor", proc_sensor, 100);
    sub = nb.subscribe("listen_from_serial", 1000,  &ChangeData::chatterCallback,this);
    pub = nb.advertise<bar_control::sensor>("force_sensor_data", 1000);
  }

  unsigned char addCheck(unsigned char *Cdata)
  {
    unsigned char i,sum=0;
    for(i=0;i<8;i++)
    sum+=Cdata[i];
    sum = sum+0xAB;
    sum = sum+0xCD;
    return sum;
  }

  struct Force
  {
    float sensor1;
    float sensor2;
  };



   struct Force charToFloat(unsigned char *Ddata)
   {
    int ccount=0;
    struct Force F;
    for(int i=0;i<4;i++)
    Fdata1[i] = Ddata[i];
    for(int i=4;i<8;i++)
    {
        if(ccount<4)
        {
            Fdata2[ccount]=Ddata[i];
            ccount++;
        }
        else
        ccount = 0;
    }
    F.sensor1 = *(float*)(&Fdata1);
    F.sensor2 = *(float*)(&Fdata2);
    //ROS_INFO("force1:[%1f]",F.sensor1);
    //ROS_INFO("force2:[%1f]",F.sensor2);
    return F;
    }

  void chatterCallback(const bar_control::serial& msg)
  {
    float force1,force2;
    for(int i=0;i<msg.serial.size();++i)
    {
     s_buffer[i]=msg.serial[i];
     if(s_buffer[i]==0xAB && flag == 0)
         flag = 1;
         else if(s_buffer[i]==0xCD && flag == 1)
         flag = 2;
         else if(flag == 2)
         {
             data[count] = s_buffer[i];
             count++;
             if(count == 9)
             {
                 count = 0;
		             flag = 0;
                 if(data[8]==addCheck(data))
                 {
                     	for(int n=0;n<8;n++)
		    	        {       
                     		Fdata[n]=data[n];
			            }

                        force1 = charToFloat(Fdata).sensor1;
                        force2 = charToFloat(Fdata).sensor2;
                        if(fabs(force1)<proc_sensor )
                        force1 = 0;
                        if (fabs(force2)<proc_sensor)
                        force2 = 0;
                        if (force1 == 0 && force2 == 0)
                        {
                          sensor.hand = 0;
                          sensor.force_data = 0;
                          sensor.theta_data = 0;
                        }
                        else if(force1==0 && force2>0) //Single rignt push，theta-
                        {
                          sensor.hand = 1;
                          sensor.force_data = force2;
                          sensor.theta_data = force1-force2; 
                        }
                        else if(force1>0 && force2==0)//Single left push，theta+
                        {
                          sensor.hand = 1;
                          sensor.force_data = force1;
                          sensor.theta_data = force1-force2;
                        }
                        else if(force1<0 && force2 == 0) //Single left poll,theta-
                        {
                          sensor.hand = 1;
                          sensor.force_data = force1;
                          sensor.theta_data = force1-force2;
                        }
                        else if(force1==0 && force2<0) //Single right poll,theta+
                        {
                          sensor.hand = 1;
                          sensor.force_data = force2;
                          sensor.theta_data = force1-force2;
                        }
                        else if(force1*force2<0)//Double turn, left push+,right push-
                        {
                          sensor.hand = 2;
                          sensor.force_data = (fabs(force1)+fabs(force2))/2;
                          sensor.theta_data = force1-force2;  
                        }
                        else if((force1>0 && force2>0) || (force1<0 && force2<0)) //Double push or poll,forward+,back-
                        {
                          sensor.hand = 2;
                          sensor.force_data = (force1+force2)/2;
                          sensor.theta_data = 0;
                        }
                        pub.publish(sensor);
                 }
                 
             }
         }
         else
         flag = 0;         
     }
        
    }
  
private:  
  ros::NodeHandle nb;   
  ros::Publisher pub;  
  ros::Subscriber sub;
  
};
  
int main(int argc, char **argv)  
{  
  ros::init(argc, argv, "ChangeData"); 
  ChangeData CDObject;    
  ros::spin();    
  return 0;  
}  
