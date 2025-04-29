# Drone Teleop ROS 2
by el jausyan

bismillahirrahmanirrahim, all praise may be to allah and the prophet. today im gonna show you how to fly drone or control drone using computer keyboard like a game, this features called "Teleop" that available on ros2 foxy wiki. in this project i develop the drone teleop script using ros2 workspace, how to make workspace in ros2 you need to open my other repository. in this project i use C++ language to write the code. the flow is you need to install all dependencies that needed to fly drone including mavros, mavlink,and ssh tutorial, so i will give you complate tutorial to run this simulations in order easy to understand to beginners, check it out

<mark>Make sure your electrical is ready to set the drone in alt_hold and ready to safe the drone using remote control in order to safety</mark> <br>

## 1 Scan your raspi IP adress
<mark>make sure your computer and the raspi connected to the same ssid</mark> <br>
```sh
sudo arp-scan --localnet
```
once you scan your local network to find your raspi ip you need to find the list named "raspphberry pi....."
then copy the raspi ip adress
## 2 SSH to Raspi
once you copied the ip adress then insert the ip adress to SSH command
```sh
ssh ubuntu@192.168.1.0
```
replace 192.168.1.0 to your raspi ip adress
after that type your password and than make sure you have been conected to raspi
at least you need 4 terminal to do this script so you need to use terminator to split your terminal

## 3 goto your drone teleop workspace
```sh
cd drone_teleop
```
rebuild the workspace
```sh
colcon build
```
and than source the ros2
```sh
source install/setup.bash
```

## 4 run mavros 
then run mavros to connect and link to the flight controller unit
using default baudrate value and serial name
```sh
ros2 run mavros mavros_node --ros-args -p fcu_url:=serial:///dev/ttyACM0:57600
```
check conectivity beetwen mavros and FCU
```sh
ros2 topic list
```
if you see the topic already looks like 
<mark>/mavros/state</mark> <br>
<mark>/mavros/....</mark> <br>
that means your mavros sucessfully connected to the FCU than you can control your drone using "ros2 topic" or "ros2 service call"
then check drone status via ros2 topic
```sh
ros2 topic echo /mavros/state
```
this command will give you all the drone status including drone mode, drone altitude and the basic drone instrument information

## 5 run the script
before you run the script you need to try the command to 
