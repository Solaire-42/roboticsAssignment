#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

class TurtleController : public rclcpp::Node {
public:
    int counterEdgeDetection_ = 0;
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
        timer_ = this->create_wall_timer(std::chrono::milliseconds(100), [this]() {
            // Publish velocity commands to make the turtle move in a square pattern
            geometry_msgs::msg::Twist msg;
            
            // Edge detection
            if ((current_pose_.x < 1 || current_pose_.x > 10 || 
	    	current_pose_.y < 1 || current_pose_.y > 10) &&
	    	(counterEdgeDetection_ == 0)) {
            	// Stop linear
            	msg.linear.x = 0;
            	// Rotate clockwise 90 degree per second (3.14/2 [rad/s])
            	msg.angular.z = 15.7;
            	counterEdgeDetection_++;
            } else {
            	// Move forward 1.0 [m/s]
            	msg.linear.x = 2;
            	// Rotate with an angular random velocity between -1 and 1 [rad/s]
            	if (counterEdgeDetection_ > 0) {
            		msg.angular.z = 0;
            		counterEdgeDetection_++;
            		if (counterEdgeDetection_ > 10) { counterEdgeDetection_ = 0; }
            	} else {
            	srand(time(0));
            	msg.angular.z = (rand() % 4) - 2;
            	}
            }
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
