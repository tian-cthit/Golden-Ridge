//this file transfers velocity command to velocities of left&right wheels

#include<stdio.h>
#include<ros/ros.h>
#include<sensor_msgs/LaserScan.h>
#include<iostream>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/Twist.h>

int num_readings;  // number of points in laser scan
std_msgs::Bool obstacle_judge;
std_msgs::Bool FLAG_continue_forward;

std::vector<float> raw;


int s_count = 0;       // scan count
int count_buffer[3];		//points count buffer
int obstacle;		//obstacle flag
int p_count;		//obstacle point count
float distance;		//obstacle distance
int p_start;
int p_end;

geometry_msgs::Twist stop_speed;

ros::Subscriber sub;
ros::Publisher obstacle_pub;
ros::Publisher obstacle_distance_pub;
ros::Publisher scan_pub;
ros::Publisher coutiune_forward_pub;
ros::Publisher speed_pub;

void callback(const sensor_msgs::LaserScan & raw_scan)
{
	//num_readings = sizeof(raw_scan.ranges)/sizeof(raw_scan.ranges[0]);

	raw=raw_scan.ranges;
	num_readings = raw.size();

	p_count = 0;
	distance = 0;
	
	p_start = num_readings * 0.375;	//range in front of wheelchair
	p_end = num_readings * 0.625;
	for(unsigned int i = p_start; i < p_end; ++i){   // go througt LaserScan points
		std::cout << num_readings<<std::endl;
      		if(raw[i] < 2000){
			p_count++;				// if distance < 2000mm , count
			distance += raw[i];
		}
	}
	count_buffer[s_count%3] = p_count;		// save into buffer
	distance = distance/p_count;
	s_count ++;

	if(count_buffer[0] > 10 && count_buffer[1] > 10 && count_buffer[2] > 10)	//if obstacle is detected 3 times continuously
		obstacle = 1;
	else
		obstacle = 0;


	std_msgs::Float32 obstacle_distance;
	obstacle_judge.data = obstacle;
	obstacle_distance.data = distance;
	

	
	if(obstacle == 1){
	stop_speed.linear.x = 0;
	stop_speed.linear.y = 0;
	stop_speed.linear.z = 0;  
	stop_speed.angular.z = 0;  
	stop_speed.angular.z = 0;  
	stop_speed.angular.z = 0;
	obstacle_pub.publish(obstacle_judge);   // obstacle_judge flag = 1 when blocked by obstacles
        //scan_pub.publish(raw_scan);	//send scan message to navigation if obstacle nearby,
	speed_pub.publish(stop_speed);}
	else{
	FLAG_continue_forward.data = 1;
	obstacle_pub.publish(obstacle_judge);   // obstacle_judge flag = 0 when no obstacles
	coutiune_forward_pub.publish(FLAG_continue_forward);}  //FLAG_continue_forward flag = 1 when no obstacles
	
	//obstacle_distance_pub.publish(obstacle_distance);
}

int main(int argc, char** argv){
  ros::init(argc, argv, "laser_scan_publisher");

  ros::NodeHandle n;
  sub = n.subscribe("/average_scan", 1000, callback);
  obstacle_pub = n.advertise<std_msgs::Bool>("/FLAG_Obstacle", 50);
  obstacle_distance_pub = n.advertise<std_msgs::Float32>("Obstacle_Distance", 50);
  scan_pub = n.advertise<sensor_msgs::LaserScan>("/nearby_scan", 50);
  coutiune_forward_pub = n.advertise<std_msgs::Bool>("/FLAG_NavigationCompleted", 50);
  speed_pub = n.advertise<geometry_msgs::Twist>("/cmd_navigation", 50);

  ros::Rate loop_rate(50);
  while(ros::ok()){
	ros::spinOnce();




	loop_rate.sleep();
	}
	return 0;
}
