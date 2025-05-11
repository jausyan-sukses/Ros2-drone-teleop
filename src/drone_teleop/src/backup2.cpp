#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <mavros_msgs/srv/command_bool.hpp>
#include <mavros_msgs/srv/set_mode.hpp>
#include <mavros_msgs/srv/command_tol.hpp>
#include <mavros_msgs/msg/state.hpp>
#include <termios.h>
#include <unistd.h>
#include <iostream>

class TeleopDrone : public rclcpp::Node
{
public:
    TeleopDrone() : Node("teleop_drone")
    {
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/mavros/setpoint_velocity/cmd_vel_unstamped", 10);

        arming_client_ = this->create_client<mavros_msgs::srv::CommandBool>("/mavros/cmd/arming");
        set_mode_client_ = this->create_client<mavros_msgs::srv::SetMode>("/mavros/set_mode");
        takeoff_client_ = this->create_client<mavros_msgs::srv::CommandTOL>("/mavros/cmd/takeoff");
        land_client_ = this->create_client<mavros_msgs::srv::CommandTOL>("/mavros/cmd/land");

        state_sub_ = this->create_subscription<mavros_msgs::msg::State>(
            "/mavros/state", 10, std::bind(&TeleopDrone::stateCallback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "Teleop Drone Node Started! Press 'M' to set GUIDED, 'T' to Arm & Takeoff, 'L' to Land, 'Z' to Disarm.");
        run();
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Client<mavros_msgs::srv::CommandBool>::SharedPtr arming_client_;
    rclcpp::Client<mavros_msgs::srv::SetMode>::SharedPtr set_mode_client_;
    rclcpp::Client<mavros_msgs::srv::CommandTOL>::SharedPtr takeoff_client_;
    rclcpp::Client<mavros_msgs::srv::CommandTOL>::SharedPtr land_client_;
    rclcpp::Subscription<mavros_msgs::msg::State>::SharedPtr state_sub_;
    mavros_msgs::msg::State current_state_;

    void stateCallback(const mavros_msgs::msg::State::SharedPtr msg)
    {
        current_state_ = *msg;
    }

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
        if (current_state_.mode == "GUIDED") {
            RCLCPP_INFO(this->get_logger(), "Already in GUIDED mode");
            return;
        }

        auto request = std::make_shared<mavros_msgs::srv::SetMode::Request>();
        request->custom_mode = "GUIDED";

        while (!set_mode_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for SetMode service...");
        }

        auto future = set_mode_client_->async_send_request(request);
        RCLCPP_INFO(this->get_logger(), "Setting mode to GUIDED...");
    }

    void armDrone()
    {
        if (current_state_.armed) {
            RCLCPP_INFO(this->get_logger(), "Drone already armed");
            return;
        }

        auto arm_request = std::make_shared<mavros_msgs::srv::CommandBool::Request>();
        arm_request->value = true;

        while (!arming_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Arming service...");
        }

        auto future = arming_client_->async_send_request(arm_request);
        RCLCPP_INFO(this->get_logger(), "Arming drone...");
    }

    void disarmDrone()
    {
        if (!current_state_.armed) {
            RCLCPP_INFO(this->get_logger(), "Drone already disarmed");
            return;
        }

        auto disarm_request = std::make_shared<mavros_msgs::srv::CommandBool::Request>();
        disarm_request->value = false;

        while (!arming_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Arming service...");
        }

        auto future = arming_client_->async_send_request(disarm_request);
        RCLCPP_INFO(this->get_logger(), "Disarming drone...");
    }

    void takeoffDrone()
    {
        auto takeoff_request = std::make_shared<mavros_msgs::srv::CommandTOL::Request>();
        takeoff_request->altitude = 3.0; // Takeoff ke 3 meter

        while (!takeoff_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Takeoff service...");
        }

        auto future = takeoff_client_->async_send_request(takeoff_request);
        RCLCPP_INFO(this->get_logger(), "Taking off to 3 meters...");
    }

    void landDrone()
    {
        auto land_request = std::make_shared<mavros_msgs::srv::CommandTOL::Request>();

        while (!land_client_->wait_for_service(std::chrono::seconds(2)))
        {
            RCLCPP_WARN(this->get_logger(), "Waiting for Land service...");
        }

        auto future = land_client_->async_send_request(land_request);
        RCLCPP_INFO(this->get_logger(), "Landing drone...");
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
                msg.linear.x = 0.1;  
                break;
            case 's':
                msg.linear.x = -0.1; 
                break;
            case 'a':
                msg.linear.y = 0.5;  
                break;
            case 'd':
                msg.linear.y = -0.5; 
                break;
            case 'q':
                msg.angular.z = 0.5;
                break;
            case 'e':
                msg.angular.z = -0.5; 
                break;
            case 'r':
                msg.linear.z = 0.5;  
                break;
            case 'f':
                msg.linear.z = -0.5; 
                break;
            case 'm':
                setModeGuided(); 
                break;
            case 't':
                armDrone();
                rclcpp::sleep_for(std::chrono::seconds(3)); 
                takeoffDrone(); 
                break;
            case 'l':
                landDrone(); 
                break;
            case 'z':
                disarmDrone(); 
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
