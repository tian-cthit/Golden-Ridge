#include <ros/ros.h> 
#include <serial/serial.h>  
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 

#include<stdio.h>
#include<tf/transform_broadcaster.h>
#include<nav_msgs/Odometry.h>
#include<geometry_msgs/Twist.h>
#include<iostream>

const float wheel_distance = 0.5534;
float v_l;
float v_r;

#define	sBUFFERSIZE	15//send buffer size 串口发送缓存长度
#define	rBUFFERSIZE	27//receive buffer size 串口接收缓存长度
unsigned char s_buffer[sBUFFERSIZE];//发送缓存
unsigned char r_buffer[rBUFFERSIZE];//接收缓存


serial::Serial ros_ser;   

void callback(const geometry_msgs::Twist& vel)
//void callback(const std_msgs::String::ConstPtr& msg)
{
     //ROS_INFO_STREAM("Write to serial port" << msg->data);
     //l & r wheel vel in m/s
     v_l = vel.linear.x+vel.angular.z*wheel_distance/2;
     v_r = vel.linear.x-vel.angular.z*wheel_distance/2;
     ros_ser.write("12",2);
 }
int main (int argc, char** argv){
     ros::init(argc, argv, "my_serial_node");
     ros::NodeHandle n;

     ros::Subscriber command_sub = n.subscribe("command", 1000, callback);
     //ros::Publisher sensor_pub = n.advertise<std_msgs::String>("sensor", 1000);
     try
     {
// port, baudrate, timeout in milliseconds

         //serial::Serial my_serial("/dev/ttyUSB0", 115200, serial::Timeout::simpleTimeout(1000));

         ros_ser.setPort("/dev/ttyUSB0");
         ros_ser.setBaudrate(115200);
         serial::Timeout to = serial::Timeout::simpleTimeout(1000);
         ros_ser.setTimeout(to);

         ros_ser.open();
     }

     catch (serial::IOException& e)
     {
         ROS_ERROR_STREAM("Unable to open port ");
         return -1;
     }
     if(ros_ser.isOpen()){
         ROS_INFO_STREAM("Serial Port opened");
     }else{
         return -1;
     }

     ros::Rate loop_rate(1);
     while(ros::ok()){
         ros::spinOnce();
         if(ros_ser.available()){
             ROS_INFO_STREAM("Reading from serial port");
             std_msgs::String serial_data;
             //获取串口数据
             serial_data.data = ros_ser.read(ros_ser.available());
             ROS_INFO_STREAM("Read: " << serial_data.data);
             //将串口数据发布到主题sensor
             //sensor_pub.publish(serial_data);
         }
         loop_rate.sleep();
     }
 }
