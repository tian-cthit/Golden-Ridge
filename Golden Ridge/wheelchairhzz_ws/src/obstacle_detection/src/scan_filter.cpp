//this file transfers velocity command to velocities of left&right wheels

#include<stdio.h>
#include<ros/ros.h>
#include<sensor_msgs/LaserScan.h>
#include<iostream>
#include <std_msgs/Int32.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>

int num_readings;  // number of points in laser scan
//std::vector<float> raw;
std::vector<float> aver_scan;
std::vector<float> empty_scan(1280*4, 15);      // empty scan to clear costmap,
//sensor_msgs::LaserScan scan;
std::vector<float> buffer[10];			//buffer to save last 10 times data
int buffer_count = 0;		//count buffer
double laser_frequency = 30;
sensor_msgs::LaserScan filtered_scan;
sensor_msgs::LaserScan clear_scan;

int32_t state_buffer = 0;
int state_buffer_count = 0;


ros::Subscriber state_sub;
ros::Subscriber scan_sub;
ros::Publisher aver_scan_pub;

bool obs_state = 0;
int clear_count = 0;
int clear_flag = 0;

void state_callback(const std_msgs::Bool & OA_Bool)
{

	obs_state = OA_Bool.data;
	//std::cout << obs_state<<std::endl;
}

void scan_callback(const sensor_msgs::LaserScan & raw_scan)
{

	//std::cout << "obs_state: "<<obs_state <<std::endl;
	//std::cout << "buffer_count: "<<buffer_count <<std::endl;
	//std::cout << state_buffer[0] <<std::endl;


	num_readings = raw_scan.ranges.size()*0.64*2/3.1415926;
	aver_scan.resize(num_readings);
	for (unsigned int k = 0; k < num_readings; ++k)
		aver_scan[k] = 0;

	for (unsigned int u = 0; u < 10; ++u){
		buffer[u].resize(num_readings);
	}


	buffer[buffer_count%10] = raw_scan.ranges;
	//std::cout << buffer_count%10<<std::endl;
	if (buffer_count > 10){
		for(unsigned int i = 0; i < num_readings; ++i){	  // for each pixel
			for(unsigned int j = 0; j < 10; ++j){
				//if(buffer[j][i] != inf)
				aver_scan[i] += buffer[j][i];	// sum of last 10 data of this pixel

			}

			aver_scan[i] /= 10;
		}




	filtered_scan.header.frame_id = raw_scan.header.frame_id;//"camera_depth_frame";
	filtered_scan.header.stamp = ros::Time::now();

	filtered_scan.angle_min = raw_scan.angle_min;
	filtered_scan.angle_max = raw_scan.angle_max;

	//filtered_scan.angle_min = -0.32;
	//filtered_scan.angle_max = 0.32;
	filtered_scan.angle_increment = raw_scan.angle_increment;//3.14 / num_readings;
	filtered_scan.time_increment = 0.0; //(1 / laser_frequency) / (num_readings);


	filtered_scan.range_min = raw_scan.range_min;//0.1;
	filtered_scan.range_max = raw_scan.range_max;//3.0;

	filtered_scan.ranges.resize(num_readings);
	filtered_scan.ranges = aver_scan;
	filtered_scan.intensities = raw_scan.intensities;


	//def empty scan to clear costmap
	clear_scan.header.frame_id = raw_scan.header.frame_id;//"camera_depth_frame";
	clear_scan.header.stamp = ros::Time::now();
	clear_scan.angle_increment = raw_scan.angle_increment;//3.14 / num_readings;
	clear_scan.time_increment = 0.0; //(1 / laser_frequency) / (num_readings);
	clear_scan.intensities = raw_scan.intensities;


		if(state_buffer == 1 && obs_state == 0){
		  clear_flag = 1;
		}


		if(clear_flag == 1){
			clear_scan.angle_min = 0.0;
			clear_scan.angle_max = 6.28000020981;
			clear_scan.range_min = 0.05;
			clear_scan.range_max = 20;
			clear_scan.ranges.resize(num_readings*4);
			clear_scan.ranges = empty_scan;

			std::cout << "Clearing costmap" <<std::endl;
			aver_scan_pub.publish(clear_scan);

			clear_count ++;
			if(clear_count == 500){
				clear_flag = 0;
				clear_count = 0;
			}

		}
		else{
			aver_scan_pub.publish(filtered_scan);
		}


	}
	buffer_count ++;
	state_buffer = obs_state;	//save the last  state(obstacle avoidance == 1 or 0) in buffer
	std::cout << obs_state <<std::endl;
}

int main(int argc, char** argv){
  ros::init(argc, argv, "filtered_scan_publisher");

  ros::NodeHandle n;
  state_sub = n.subscribe("/FLAG_ObstacleAvoidanceMode", 1000, state_callback);
  scan_sub = n.subscribe("/mybot/scan", 1000, scan_callback);
  aver_scan_pub = n.advertise<sensor_msgs::LaserScan>("/average_scan", 10);


  ros::Rate loop_rate(20);
  while(ros::ok()){
	ros::spinOnce();



	loop_rate.sleep();
	}
	return 0;
}
