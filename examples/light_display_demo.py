#!/usr/bin/env python3

from dataclasses import dataclass
from typing import Dict, Tuple

import rclpy
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, HistoryPolicy, QoSProfile, ReliabilityPolicy
from std_msgs.msg import ColorRGBA

from rviz_light_display.msg import LightCommand


Color = Tuple[float, float, float, float]


@dataclass(frozen=True)
class DemoLight:
    topic: str
    color: Color


class LightDisplayDemo(Node):
    def __init__(self):
        super().__init__("rviz_light_display_demo")

        qos = QoSProfile(
            depth=5,
            durability=DurabilityPolicy.TRANSIENT_LOCAL,
            history=HistoryPolicy.KEEP_LAST,
            reliability=ReliabilityPolicy.RELIABLE,
        )

        self._lights: Dict[str, DemoLight] = {
            "sphere": DemoLight("/rviz_light_display_demo/sphere", (1.0, 0.15, 0.15, 1.0)),
            "cube": DemoLight("/rviz_light_display_demo/cube", (0.0, 0.67, 1.0, 1.0)),
            "cylinder": DemoLight("/rviz_light_display_demo/cylinder", (0.2, 0.86, 0.37, 1.0)),
            "cone": DemoLight("/rviz_light_display_demo/cone", (1.0, 0.36, 0.0, 1.0)),
            "traffic_red": DemoLight("/rviz_light_display_demo/traffic/red", (1.0, 0.0, 0.0, 1.0)),
            "traffic_amber": DemoLight("/rviz_light_display_demo/traffic/amber", (1.0, 0.67, 0.0, 1.0)),
            "traffic_green": DemoLight("/rviz_light_display_demo/traffic/green", (0.0, 1.0, 0.0, 1.0)),
            "traffic_housing": DemoLight("/rviz_light_display_demo/traffic/housing", (0.08, 0.08, 0.08, 1.0)),
            "beacon_red": DemoLight("/rviz_light_display_demo/beacon/red", (1.0, 0.0, 0.0, 1.0)),
            "beacon_amber": DemoLight("/rviz_light_display_demo/beacon/amber", (1.0, 0.67, 0.0, 1.0)),
            "beacon_green": DemoLight("/rviz_light_display_demo/beacon/green", (0.0, 1.0, 0.0, 1.0)),
            "beacon_blue": DemoLight("/rviz_light_display_demo/beacon/blue", (0.0, 0.33, 1.0, 1.0)),
        }
        self._publishers = {
            name: self.create_publisher(LightCommand, light.topic, qos)
            for name, light in self._lights.items()
        }

        self._start_time = self.get_clock().now()
        self.create_timer(0.1, self._tick)
        self.get_logger().info("Publishing rviz_light_display demo sequence.")

    def _tick(self):
        elapsed = (self.get_clock().now() - self._start_time).nanoseconds * 1e-9

        shape_names = ["sphere", "cube", "cylinder", "cone"]
        active_shape = shape_names[int(elapsed / 0.6) % len(shape_names)]

        traffic_phase = elapsed % 8.0
        traffic_active = "traffic_red"
        if 3.0 <= traffic_phase < 6.5:
            traffic_active = "traffic_green"
        elif 6.5 <= traffic_phase < 8.0:
            traffic_active = "traffic_amber"

        beacon_names = ["beacon_red", "beacon_amber", "beacon_green", "beacon_blue"]
        beacon_active = beacon_names[int(elapsed / 0.35) % len(beacon_names)]

        for name, light in self._lights.items():
            brightness = 0.0
            if name == active_shape or name == traffic_active or name == beacon_active:
                brightness = 1.0
            elif name == "traffic_housing":
                brightness = 0.25

            self._publishers[name].publish(self._make_command(light.color, brightness))

    @staticmethod
    def _make_command(color: Color, brightness: float) -> LightCommand:
        command = LightCommand()
        command.color = ColorRGBA(r=color[0], g=color[1], b=color[2], a=color[3])
        command.brightness = brightness
        command.keep_color = brightness <= 0.0
        return command


def main():
    rclpy.init()
    node = LightDisplayDemo()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
