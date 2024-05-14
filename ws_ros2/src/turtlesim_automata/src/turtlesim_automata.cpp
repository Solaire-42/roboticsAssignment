#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

class TurtleController : public rclcpp::Node {
public:
    // Send turtle commands periodically with a timer
    const int cycletimeTurtleCommands_ = 100; // [ms]
    // After edge detection we do not want to change direction in the first few cycles for a smoother ride
    int counterEdgeDetection_ = 0;
    const int cyclesOfNoAngularChange_ = 10;
    // Parameters
    const int velocityTurtle_ = 2;
    const int randomAgularVelocityRange_ = 4; // Modulo: x % y = range [0,y-1]



    TurtleController() : Node("turtle_controller") {
        // Create publisher to publish velocity commands
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // Create subscriber to get current pose of the turtle
        subscriber_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, [this](const turtlesim::msg::Pose::SharedPtr pose) {
            // Update the current pose of the turtle
            current_pose_ = *pose;
        });
	
        // Timer to publish velocity commands periodically
        timer_ = this->create_wall_timer(std::chrono::milliseconds(cycletimeTurtleCommands_), [this]() {
            // Publish velocity commands to make the turtle move
            geometry_msgs::msg::Twist msg;
            
            // Edge detection with counter
            if ((current_pose_.x < 0.5 || current_pose_.x > 10.5 || current_pose_.y < 0.5 || current_pose_.y > 10.5) && (counterEdgeDetection_ == 0)) {
            	// Stop linear when we reach the borders
            	msg.linear.x = 0;
            	// Rotate clockwise 90 degree per second (3.14/2 [rad/s])
            	msg.angular.z = (3.14/2)/cycletimeTurtleCommands_*1000; // 90 degree / cycletime
            	counterEdgeDetection_++;
            } else {
            	// Move forward
            	msg.linear.x = velocityTurtle_;
            	// The first few cycles after an edge detection we don not want to rotate and rather move away from the edge
            	if (counterEdgeDetection_ > 0) {
            		msg.angular.z = 0;
            		counterEdgeDetection_++;
            		if (counterEdgeDetection_ > cyclesOfNoAngularChange_) { counterEdgeDetection_ = 0; } // Set counter to 0
            	} else {
                // Otherwise we rotate with an angular random velocity between -2 and 2 [rad/s]
            	srand(time(0));
            	msg.angular.z = (rand() % (randomAgularVelocityRange_+1)) - (randomAgularVelocityRange_/2); // Example: rand() % (4+1) = range[0..4] -> - 4/2 = range[-2..2]
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
