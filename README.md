# Bar_control
Force sensor control the robot move in the ROS(Kinetic)    
This package function is that  control the robot by the force sensor in the ros. 

1. Check the robot has installed the following package  
   
   ros-kinetic-serial   
   ros-kinetic-serial-*

2. Put the package in the workspace and catkin_make



3. Make a rule   
  
  3.1 Put the 40-bar_control.rules in the /etc/udev/rules.d    
  3.2 sudo service udev reload    
  3.3 sudo service udev restar


4. You can change some params in the launch file then roslaunch the file.
