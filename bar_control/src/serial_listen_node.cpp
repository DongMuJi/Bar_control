/***
 * This example expects the serial port has a loopback on it.
 *
 * Alternatively, you could use an Arduino:
 *
 * <pre>
 *  void setup() {
 *    Serial.begin(<insert your baudrate here>);
 *  }
 *
 *  void loop() {
 *    if (Serial.available()) {
 *      Serial.write(Serial.read());
 *    }
 *  }
 * </pre>
 */

#include <ros/ros.h>
#include <serial/serial.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include "bar_control/serial.h"

#define rBUFFERSIZE  30
unsigned char r_buffer[rBUFFERSIZE];
serial::Serial ser;
/*void write_callback(const std_msgs::String::ConstPtr& msg){
    ROS_INFO_STREAM("Writing to serial port" << msg->data);
    ser.write(msg->data);
}*/

/*std::vector<unsigned char> cov(unsigned char* rbu)
                         {
                           std::vector<unsigned char>  m;
                           for(int l=0;l<rBUFFERSIZE;l++)
                                      {
                                      m.push_back(rbu[l]);
                                       }
                             return m;
                           }*/
int main (int argc, char** argv){
    ros::init(argc, argv, "serial_listen_node");
    ros::NodeHandle nh;

   // ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback);
    ros::Publisher msg_pub = nh.advertise<bar_control::serial>("listen_from_serial", 1000);

    try
    {
        ser.setPort("/dev/bar");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    if(ser.isOpen()){
        ROS_INFO_STREAM("Serial Port initialized");
    }else{
        return -1;
    }

    ros::Rate loop_rate(50);
    while(ros::ok()){
        bar_control::serial msg;
        ros::spinOnce();
          if(ser.available()){
                             ROS_INFO_STREAM("Reading from serial port");
                             ser.read(r_buffer,rBUFFERSIZE);
			     for(int i=0;i<rBUFFERSIZE;i++)
				{
                                 ROS_INFO("[0x%02x]",r_buffer[i]);
                                 }
			 ROS_INFO_STREAM("End reading from serial port");        
                             for(int l=0;l<rBUFFERSIZE;l++)
                                {
                                    msg.serial.push_back(r_buffer[l]);
                                 }
                                 msg_pub.publish(msg);
                                /*  for(int j=0;j<rBUFFERSIZE;j++)
                                   ROS_INFO("[0x%02x]",msg.serial[j]); */   
                          
                       
                          
                   /*   
                        std::cout << "msg.serial[0]=" << msg.serial[j] << std::endl; 
                        std::cout << "msg.serial.size=" << msg.serial.size() << std::endl;
                       //std_msgs::String s( reinterpret_cast<char const*>(r_buffer) );
                      ser.write(r_buffer,rBUFFERSIZE); */ 
                      
                             } 
      /*  此处代码为读取字符串类型
               if(ser.available()){
            ROS_INFO_STREAM("Reading from serial port");
            std_msgs::String result;
            result.data=ser.read(ser.available());
           ROS_INFO_STREAM("Read: " << result.data);
           read_pub.publish(result);
        } */
                     loop_rate.sleep();

    }
}

