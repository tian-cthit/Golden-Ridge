//this file transfers velocity command to velocities of left&right wheels

#include<stdio.h>
#include<ros/ros.h>
#include<sensor_msgs/LaserScan.h>
#include<iostream>
#include <std_msgs/Int32.h>
#include <std_msgs/Float32.h>

int num_readings;  // number of points in laser scan
//std::vector<float> raw;
std::vector<float> aver_scan;
std::vector<float> empty_scan(1280*4, 20.0);      // empty scan to clear costmap
//sensor_msgs::LaserScan scan;
std::vector<float> buffer[10];			//buffer to save last 10 times data
int buffer_count = 0;		//count buffer
double laser_frequency = 30;
sensor_msgs::LaserScan filtered_scan;

int state_buffer[2];
int state_buffer_count = 0;


ros::Subscriber state_sub;
ros::Subscriber scan_sub;
ros::Publisher aver_scan_pub;

int obs_state;

void state_callback(const sensor_msgs::LaserScan & raw_scan)
{
	obs_state = 1;
}

void scan_callback(const sensor_msgs::LaserScan & raw_scan)
{
	state_buffer[buffer_count%2] = obs_state;	//save the last 2 states(obstacle avoidance == 1 or 0) in buffer

	//std::cout << state<<std::endl;
	num_readings = raw_scan.ranges.size();
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
	filtered_scan.angle_increment = raw_scan.angle_increment;//3.14 / num_readings;
	filtered_scan.time_increment = 0.0; //(1 / laser_frequency) / (num_readings);
	filtered_scan.range_min = raw_scan.range_min;//0.1;
	filtered_scan.range_max = raw_scan.range_max;//3.0;
	filtered_scan.ranges.resize(num_readings);
	filtered_scan.ranges = aver_scan;
	filtered_scan.intensities = raw_scan.intensities;


	// generate a empty scan msg to clear costmap
	if(state_buffer[1] == 0 && state_buffer[0] == 1){
		filtered_scan.angle_min = 0.0;
		filtered_scan.angle_max = 6.28000020981;
		filtered_scan.ranges.resize(num_readings*4);
		filtered_scan.ranges = empty_scan;
		for(unsigned int v = 0; v < 10; ++v){
			aver_scan_pub.publish(filtered_scan);
			ros::Duration d(0.05);
		}
	}

	//for(unsigned int i = 0; i < num_readings; ++i){

	//}


	aver_scan_pub.publish(filtered_scan);
	}
	buffer_count ++;

}

int main(int argc, char** argv){
  ros::init(argc, argv, "filtered_scan_publisher");

  ros::NodeHandle n;
	state_sub = n.subscribe("mybot/scan", 1000, state_callback);
  scan_sub = n.subscribe("mybot/scan", 1000, scan_callback);
  aver_scan_pub = n.advertise<sensor_msgs::LaserScan>("average_scan", 10);


  ros::Rate loop_rate(50);
  while(ros::ok()){
	ros::spinOnce();



	loop_rate.sleep();
	}
	return 0;
}
