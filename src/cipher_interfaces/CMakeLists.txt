cmake_minimum_required(VERSION 3.8)
project(cipher_interfaces)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/CipherMessage.msg"
  "srv/CipherAnswer.srv"
  DEPENDENCIES std_msgs
)

install(TARGETS 
  DESTINATION lib/${PROJECT_NAME})

ament_export_dependencies(rosidl_default_runtime)


ament_package()