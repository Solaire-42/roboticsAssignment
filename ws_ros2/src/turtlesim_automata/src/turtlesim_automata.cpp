#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

class TurtleController : public rclcpp::Node {
public:
    TurtleController() : Node("turtle_controller") {
        // Create publisher to publish velocity commands
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // Create subscriber to get current pose of the turtle
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>(
            "turtle1/pose",
            10,
            [this](const turtlesim::msg::Pose::SharedPtr pose) {
                // Update the current pose of the turtle
                current_pose_ = *pose;
            });

        // Timer to publish velocity commands periodically
        timer_ = this->create_wall_timer(std::chrono::milliseconds(1000), [this]() {
            // Publish velocity commands to make the turtle move in a square pattern
            geometry_msgs::msg::Twist msg;
            msg.linear.x = 1.0; // Move forward with a linear velocity of 1 m/s
            msg.angular.z = 1.0; // Rotate with an angular velocity of 1 rad/s
            publisher_->publish(msg);
        });
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr subscriber_;
    rclcpp::TimerBase::SharedPtr timer_;
    turtlesim::msg::Pose current_pose_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
