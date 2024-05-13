#! /bin/bash
echo "Create ros2 workspace"
mkdir -p ~/ros2_ws/src

echo "Copy files and directories ..."
cp -r ws_ros2/src/turtlesim_automata ~/ros2_ws/src

#echo "Delete current directory"
#cd..
#rm -r roboticAssignment

echo "Change into workspace directory"
cd ~/ros2_ws

echo "Build with colcon"
colcon build

echo "Source setup.bash"
source install/setup.bash

echo "Launch turtlesim and turtlesim_automata"
ros2 launch turtlesim_automata automata.launch.py

