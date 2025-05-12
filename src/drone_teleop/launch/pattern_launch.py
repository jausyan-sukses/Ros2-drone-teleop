from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='drone_teleop',
            executable='teleop_keyboard',
            name='teleop_drone_node',
            output='screen'
        ),
        Node(
            package='drone_teleop',
            executable='teleop_sequencer_node',
            name='autocmd_node',
            output='screen'
        )
    ])
