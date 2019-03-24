#include<stdio.h>
#include<ros/ros.h>
#include<tf/transform_broadcaster.h>
#include<nav_msgs/Odometry.h>
#include<geometry_msgs/Twist.h>
#include<iostream>

const float wheel_distance = 0.5534;
float v_l;
float v_r;

void callback(const geometry_msgs::Twist& vel)
{
/*
	ROS_INFO("Received a /cmd_vel message!");
	ROS_INFO("Linear Components:[%f,%f,%f]",mybot_cmd.linear.x,mybot_cmd.linear.y,mybot_cmd.linear.z);
	ROS_INFO("Angular Components:[%f,%f,%f]",mybot_cmd.angular.x,mybot_cmd.angular.y,mybot_cmd.angular.z);
*/

   v_l = vel.linear.x+vel.angular.z*wheel_distance/2;
   v_r = vel.linear.x-vel.angular.z*wheel_distance/2;  

    // Then set wheel speeds 
//    wheel_left.set_speed(v_l)  
//    wheel_right.set_speed(v_r) 
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "diff_trans");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("/mybot/cmd", 1000, callback);
	//ros::spin();


	ros::Rate loop_rate(10);
	while(ros::ok() ) 
	{
		// printf("left wheel speed = %f\n",v_l);
   		 ros::spinOnce();
		loop_rate.sleep();
	}

/*
	ros::Rate loop_rate(10);
	while( n.ok() ) 
	{
   		 ros::spin();
	}
*/
	return 0;
}
