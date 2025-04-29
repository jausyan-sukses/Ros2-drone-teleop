#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <mavros_msgs/srv/command_bool.hpp>
#include <mavros_msgs/srv/set_mode.hpp>
#include <mavros_msgs/srv/command_tol.hpp>
#include <termios.h>
#include <unistd.h>
#include <iostream>

class TeleopDrone : public rclcpp::Node
{
public:
    TeleopDrone() : Node("teleop_drone")
    {
        // Publisher untuk mengontrol pergerakan
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/mavros/setpoint_velocity/cmd_vel_unstamped", 10);

        // Clients untuk Arming, Mode, dan Takeoff
        arming_client_ = this->create_client<mavros_msgs::srv::CommandBool>("/mavros/cmd/arming");
        set_mode_client_ = this->create_client<mavros_msgs::srv::SetMode>("/mavros/set_mode");
        takeoff_client_ = this->create_client<mavros_msgs::srv::CommandTOL>("/mavros/cmd/takeoff");

        RCLCPP_INFO(this->get_logger(), "Teleop Drone Node Started! Press 'M' to set GUIDED mode, 'T' to Arm & Takeoff.");
        run();
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Client<mavros_msgs::srv::CommandBool>::SharedPtr arming_client_;
    rclcpp::Client<mavros_msgs::srv::SetMode>::SharedPtr set_mode_client_;
    rclcpp::Client<mavros_msgs::srv::CommandTOL>::SharedPtr takeoff_client_;

    int getKey()
    {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }

    void setModeGuided()
    {
        auto request = std::make_shared<mavros_msgs::srv::SetMode::Request>();
        request->custom_mode = "GUIDED";

        while (!set_mode_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for SetMode service...");
        }

        auto future = set_mode_client_->async_send_request(request);
        RCLCPP_INFO(this->get_logger(), "Setting mode to GUIDED...");
    }

    void armDroneAndTakeoff()
    {
        auto arm_request = std::make_shared<mavros_msgs::srv::CommandBool::Request>();
        arm_request->value = true;

        while (!arming_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Arming service...");
        }

        auto arm_future = arming_client_->async_send_request(arm_request);
        RCLCPP_INFO(this->get_logger(), "Arming drone...");
        rclcpp::sleep_for(std::chrono::seconds(2)); // Tunggu 2 detik untuk arming selesai

        auto takeoff_request = std::make_shared<mavros_msgs::srv::CommandTOL::Request>();
        takeoff_request->altitude = 3.0; // Drone takeoff ke 3 meter

        while (!takeoff_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Takeoff service...");
        }

        auto takeoff_future = takeoff_client_->async_send_request(takeoff_request);
        RCLCPP_INFO(this->get_logger(), "Taking off to 3 meters...");
    }

    void run()
    {
        geometry_msgs::msg::Twist msg;
        while (rclcpp::ok())
        {
            char key = getKey();
            msg.linear.x = 0.0;
            msg.linear.y = 0.0;
            msg.linear.z = 0.0;
            msg.angular.z = 0.0;

            switch (key)
            {
            case 'w':
                msg.linear.x = 1.0;  // Maju
                break;
            case 's':
                msg.linear.x = -1.0; // Mundur
                break;
            case 'a':
                msg.linear.y = 1.0;  // Geser kiri
                break;
            case 'd':
                msg.linear.y = -1.0; // Geser kanan
                break;
            case 'q':
                msg.angular.z = 1.0; // Rotasi kiri
                break;
            case 'e':
                msg.angular.z = -1.0; // Rotasi kanan
                break;
            case 'r':
                msg.linear.z = 1.0;  // Naik
                break;
            case 'f':
                msg.linear.z = -1.0; // Turun
                break;
            case 'm':
                setModeGuided(); // Set mode GUIDED
                break;
            case 't':
                armDroneAndTakeoff(); // Arming dan langsung takeoff
                break;
            case 'x':
                RCLCPP_INFO(this->get_logger(), "Exiting Teleop...");
                return;
            default:
                break;
            }

            cmd_vel_pub_->publish(msg);
            RCLCPP_INFO(this->get_logger(), "Sending command: x=%.2f, y=%.2f, z=%.2f, yaw=%.2f",
                        msg.linear.x, msg.linear.y, msg.linear.z, msg.angular.z);
        }
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<TeleopDrone>());
    rclcpp::shutdown();
    return 0;
}
