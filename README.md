Usage:

`rosdep install -i --from-path src --rosdistro humble -y`

`colcon build --packages-select cipher_interfaces`

`colcon build --packages-select cipher_node`

(in new terminal)

`source install/setup.bash`

`cd launch` and `ros2 launch cipher_launch.yaml`
