#include <rclcpp/rclcpp.hpp>
#include "cipher_interfaces/msg/cipher_message.hpp"
#include "cipher_interfaces/srv/cipher_answer.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

class CaesarCipherNode : public rclcpp::Node
{
public:
    CaesarCipherNode() : Node("caesar_cipher_node"), key_(3) // Default Caesar key is 3
    {
        // Publisher
        cipher_pub_ = this->create_publisher<cipher_interfaces::msg::CipherMessage>("cipher_topic", 10);

        // Service server
        answer_service_ = this->create_service<cipher_interfaces::srv::CipherAnswer>(
            "validate_answer",
            std::bind(&CaesarCipherNode::validate_answer_callback, this, _1, _2));

        // Timer to regularly publish encoded messages
        timer_ = this->create_wall_timer(
            std::chrono::seconds(5),
            std::bind(&CaesarCipherNode::publish_encoded_message, this));
        
        RCLCPP_INFO(this->get_logger(), "Caesar Cipher node has been started.");
    }

private:
    void publish_encoded_message()
    {
        auto msg = cipher_interfaces::msg::CipherMessage();
        msg.original_message = "Hello UNSW";  // Original message to be encoded
        msg.key = key_;
        msg.encoded_message = caesar_cipher_encode(msg.original_message, key_);

        cipher_pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Publishing encoded message: %s", msg.encoded_message.c_str());
    }

    std::string caesar_cipher_encode(const std::string &message, int key)
    {
        std::string encoded = message;
        for (char &c : encoded)
        {
            if (isalpha(c))
            {
                char base = isupper(c) ? 'A' : 'a';
                c = (c - base + key) % 26 + base;
            }
        }
        return encoded;
    }

    void validate_answer_callback(
        const std::shared_ptr<cipher_interfaces::srv::CipherAnswer::Request> request,
        std::shared_ptr<cipher_interfaces::srv::CipherAnswer::Response> response)
    {
        std::string decoded_message = caesar_cipher_encode(request->decoded_message, -key_);
        RCLCPP_INFO(this->get_logger(), "Received decoded message: %s", request->decoded_message.c_str());
        
        response->is_correct = (decoded_message == "Hello UNSW");
        if (response->is_correct)
        {
            RCLCPP_INFO(this->get_logger(), "Decoded message is correct!");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Decoded message is incorrect.");
        }
    }

    // Private members
    rclcpp::Publisher<cipher_interfaces::msg::CipherMessage>::SharedPtr cipher_pub_;
    rclcpp::Service<cipher_interfaces::srv::CipherAnswer>::SharedPtr answer_service_;
    rclcpp::TimerBase::SharedPtr timer_;
    int key_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CaesarCipherNode>());
    rclcpp::shutdown();
    return 0;
}
