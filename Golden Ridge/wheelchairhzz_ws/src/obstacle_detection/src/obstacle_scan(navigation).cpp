//this file transfers velocity command to velocities of left&right wheels

#include<stdio.h>
#include<ros/ros.h>
#include<sensor_msgs/LaserScan.h>
#include<iostream>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>

int num_readings;  // number of points in laser scan
std_msgs::Bool obstacle_judge;
std::vector<float> raw;


int s_count = 0;       // scan count
int count_buffer[3];		//points count buffer
int obstacle;		//obstacle flag
int p_count;		//obstacle point count
float distance;		//obstacle distance


ros::Subscriber sub;
ros::Publisher obstacle_pub;
ros::Publisher obstacle_distance_pub;
ros::Publisher scan_pub;

void callback(const sensor_msgs::LaserScan & raw_scan)
{
	//num_readings = sizeof(raw_scan.ranges)/sizeof(raw_scan.ranges[0]);

	raw=raw_scan.ranges;
	num_readings = raw.size();

	p_count = 0;
	distance = 0;

	for(unsigned int i = 0; i < num_readings; ++i){   // go througt all LaserScan points
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
	//obstacle_pub.publish(obstacle_judge);

	//send scan message to navigation if obstacle nearby,
	if(obstacle == 1)
        scan_pub.publish(raw_scan);
		//obstacle_distance_pub.publish(obstacle_distance);
}

int main(int argc, char** argv){
  ros::init(argc, argv, "laser_scan_publisher");

  ros::NodeHandle n;
  sub = n.subscribe("/average_scan", 1000, callback);
  obstacle_pub = n.advertise<std_msgs::Bool>("/FLAG_Obstacle", 50);
  obstacle_distance_pub = n.advertise<std_msgs::Float32>("Obstacle_Distance", 50);
  scan_pub = n.advertise<sensor_msgs::LaserScan>("/nearby_scan", 50);



  ros::Rate loop_rate(50);
  while(ros::ok()){
	ros::spinOnce();




	loop_rate.sleep();
	}
	return 0;
}
