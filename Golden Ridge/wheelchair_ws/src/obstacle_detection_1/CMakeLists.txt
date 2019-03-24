cmake_minimum_required(VERSION 2.8.3)
project(obstacle_detection)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
)


catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES obstacle_detection
#  CATKIN_DEPENDS roscpp rospy sensor_msgs std_msgs
#  DEPENDS system_lib
)

include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)

add_executable(obstacle_scan src/obstacle_scan.cpp)
target_link_libraries(obstacle_scan
    #${Boost_LIBRARIES}
    ${catkin_LIBRARIES}
    )

add_executable(scan_filter src/scan_filter.cpp)
target_link_libraries(scan_filter
    #${Boost_LIBRARIES}
    ${catkin_LIBRARIES}
    )
