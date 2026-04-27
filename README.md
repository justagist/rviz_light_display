# rviz_light_display

[![Demo](https://media.githubusercontent.com/media/justagist/_assets/refs/heads/main/rviz_light_display/rviz_light_display.gif)](examples/light_display_demo.py)

`rviz_light_display` is a self-contained rviz2 display plugin for status lights.

Each display instance stores:
- A TF frame
- A pose relative to that frame
- Visual shape, sizing, and off-state appearance
- A default rgba value used before any runtime color is received

Runtime updates only need to publish a `rviz_light_display/msg/LightCommand` message on the configured topic.

## Display properties

When you first add the display in rviz, you can set:
- `Shape` (`Sphere`, `Cylinder`, `Cube`, or `Cone`)
- `Diameter` for spheres, cylinders, and cones
- `Height` for cylinders, cones, and cubes
- `Width` and `Depth` for cubes
- `Orientation`, applied after each shape's upright default orientation
- `Default Color`
- `Default Alpha`

That configured rgba value is used immediately, even before any publisher sets a color.

If a publisher later sends a message with `keep_color = false`, that message color becomes the new active runtime color.
After that, future messages with `keep_color = true` will reuse the last active runtime color.

## Message

```text
std_msgs/ColorRGBA color
float32 brightness
bool keep_color
```

Semantics:
- `color.r`, `color.g`, `color.b` define the light color in the range `[0.0, 1.0]`.
- `color.a` defines the marker transparency in the range `[0.0, 1.0]`.
- `brightness` defines the light intensity in the range `[0.0, 1.0]`.
- `brightness = 0.0` means the light is OFF.
- `brightness > 0.0` means the light is ON, with higher values appearing brighter.
- `keep_color = false` means apply the `color` field from this message and store it as the active runtime color.
- `keep_color = true` means ignore the `color` field and reuse the currently active color.

There is no separate enable flag, so OFF vs dim vs ON is controlled only by `brightness`.

## Typical use

1. Build and source the workspace.
2. Open rviz2 and add `rviz_light_display/LightDisplay`.
3. Set `Topic`, `Light Frame`, `Position`, and optionally `Orientation`, `Shape`, dimensions, `Default Color`, and `Default Alpha`.
4. Publish updates to the chosen topic without resending the pose.

## Example

Start with the color configured in rviz and only send brightness updates:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{brightness: 0.25, keep_color: true}"
```

Set a new runtime color and brightness:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 1.0, b: 0.0, a: 1.0}, brightness: 1.0, keep_color: false}"
```

Make it translucent:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 0.4, b: 1.0, a: 0.35}, brightness: 1.0, keep_color: false}"
```

Turn it off while keeping the current color for the next update:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 0.0, b: 0.0, a: 0.0}, brightness: 0.0, keep_color: true}"
```

## Demo config

This package includes an rviz demo with the supported shapes, a traffic signal, and a four-cylinder beacon light:

```bash
rviz2 -d install/share/rviz_light_display/examples/rviz/light_display_shapes_demo.rviz
ros2 run rviz_light_display light_display_demo.py
```
