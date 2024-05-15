#!/bin/bash

source ~/catkin_ws/devel/setup.bash



roslaunch ydlidar lidar_view.launch &
rosrun lidar scan &
sleep 5


