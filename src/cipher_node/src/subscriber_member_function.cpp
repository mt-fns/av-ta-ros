// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


// run:
// colcon build
// . install/setup.bash
// ros2 run cipher_node listener
// or use the launch file, ros2 launch cipher_launch.yaml

// resources:
// https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Publisher-And-Subscriber.html
// https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Custom-ROS2-Interfaces.html
// https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Service-And-Client.html

#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "cipher_interfaces/msg/cipher_message.hpp"
#include "cipher_interfaces/srv/cipher_answer.hpp"

using namespace std;

using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
public:
  MinimalSubscriber()
  : Node("minimal_subscriber")
  {
    // subscribe to a topic called 'topic'
    subscription_ = this->create_subscription<cipher_interfaces::msg::CipherMessage>(
      "topic", 10, std::bind(&MinimalSubscriber::topic_callback, this, _1));

    // create client for service call from this node
    client_ = this->create_client<cipher_interfaces::srv::CipherAnswer>("cipher_answer");
  }

private:
  rclcpp::Subscription<cipher_interfaces::msg::CipherMessage>::SharedPtr subscription_;
  rclcpp::Client<cipher_interfaces::srv::CipherAnswer>::SharedPtr client_;

  string caesar_decipher(string text, int key) const {
    key = key % 26;
    
    for (int i = 0; i < text.length(); i++) {
      char deciphered = text[i] - key;
      
      // loop back to Z (uppercase)
      if(isupper(text[i]) && (65 - deciphered) > 0) {
        deciphered = 90 - (65 - deciphered - 1);
        text[i] =  deciphered;
      }
      
      // loop back to z (lowercase)
      else if (islower(text[i]) && (97 - deciphered) > 0) {
        deciphered = 122 - (97 - deciphered - 1);
        text[i] = deciphered;
      }
    }
    
    return text;
  }

  // subscriber callback function, receive from publiher then send service call using client
  void topic_callback(const cipher_interfaces::msg::CipherMessage & msg) const
  {
    string encoded_msg = msg.message.c_str();
    int key = msg.key;

    RCLCPP_INFO_STREAM(this->get_logger(), "Cipher received: '" << encoded_msg << "' and key: " << key); 
    // prepare service call
    auto request = std::make_shared<cipher_interfaces::srv::CipherAnswer::Request>();
    string decoded_msg = caesar_decipher(encoded_msg, key);
    request->answer = decoded_msg;

    // make request header (double check this implementation)
    // https://answers.ros.org/question/347241/how-to-check-if-a-message-has-a-header-c/
    // https://answers.ros.org/question/287946/ros-2-time-handling/
    // https://answers.ros.org/question/356352/query-on-get_clock-now/
    request->header.stamp = rclcpp::Clock().now();
    request->header.frame_id = "frame_1";

    RCLCPP_INFO_STREAM(this->get_logger(), "Decoded msg: '" << decoded_msg << "'"); 


    // try to find a service or wait until a service is found
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
        return;
      }
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
    }

    // send decoded msg to service and log result in service callback
    auto result = client_->async_send_request(request, std::bind(&MinimalSubscriber::service_callback, this, std::placeholders::_1));
  }

  // example service callback function implementation: https://answers.ros.org/question/402688/how-to-access-the-response-of-async-request-from-a-client/
  void service_callback(rclcpp::Client<cipher_interfaces::srv::CipherAnswer>::SharedFuture future) const {
    auto response = future.get();

    RCLCPP_INFO(this->get_logger(), "Cipher received: '%d'", response->result); 

    // example of shutting down node inside of callback: https://answers.ros.org/question/329716/close-a-node-from-inside-node-class/
    // shutdown node after service call
    rclcpp::shutdown();
    return;
  }
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}
