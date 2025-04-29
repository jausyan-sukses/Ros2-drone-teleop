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
