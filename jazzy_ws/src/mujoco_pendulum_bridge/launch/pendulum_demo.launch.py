# Copyright 2026 Bryant Olmos
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    pkg_dir = get_package_share_directory('mujoco_pendulum_bridge')
    config_file = os.path.join(pkg_dir, 'config', 'params.yaml')

    bridge_node = Node(
        package='mujoco_pendulum_bridge',
        executable='bridge_node',
        name='bridge_node',
        output='screen',
        parameters=[config_file],
        emulate_tty=True
    )

    controller_node = Node(
        package='mujoco_pendulum_bridge',
        executable='controller_node',
        name='controller_node',
        output='screen',
        parameters=[config_file],
        emulate_tty=True
    )

    return LaunchDescription([
        bridge_node,
        controller_node,
    ])