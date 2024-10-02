cmake_minimum_required(VERSION 3.5)
project(cipher_node)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
# add extra dependencies for node
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cipher_interfaces REQUIRED)

# output executable file for ros2 to run
add_executable(talker src/publisher_member_function.cpp)
ament_target_dependencies(talker rclcpp std_msgs cipher_interfaces)
add_executable(listener src/subscriber_member_function.cpp)
ament_target_dependencies(listener rclcpp std_msgs cipher_interfaces)


# make sure ros2 can find the executable
install(TARGETS
  talker
  listener
  DESTINATION lib/${PROJECT_NAME})

ament_package()