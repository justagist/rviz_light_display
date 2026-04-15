# rviz_light_display

`rviz_light_display` is a self-contained rviz2 display plugin for status lights.

Each display instance stores:
- A TF frame
- A pose relative to that frame
- Visual sizing and off-state appearance

Runtime updates only need to publish a `rviz_light_display/msg/LightCommand` message on the configured topic.

## Message

```text
std_msgs/ColorRGBA color
float32 brightness
```

Semantics:
- `color.r`, `color.g`, `color.b` define the light color in the range `[0.0, 1.0]`.
- `color.a` defines the marker transparency in the range `[0.0, 1.0]`.
- `brightness` defines the light intensity in the range `[0.0, 1.0]`.
- `brightness = 0.0` means the light is OFF.
- `brightness > 0.0` means the light is ON, with higher values appearing brighter.

There is no separate enable flag, so OFF vs dim vs ON is controlled only by `brightness`.

## Typical use

1. Build and source the workspace.
2. Open rviz2 and add `rviz_light_display/LightDisplay`.
3. Set `Topic`, `Light Frame`, `Position`, and optionally `Orientation` and `Diameter`.
4. Publish updates to the chosen topic without resending the pose.

## Example

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 1.0, b: 0.0, a: 1.0}, brightness: 1.0}"
```

Dim the light:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 1.0, g: 0.5, b: 0.0, a: 1.0}, brightness: 0.25}"
```

Make it translucent:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 0.4, b: 1.0, a: 0.35}, brightness: 1.0}"
```

Turn it off:

```bash
ros2 topic pub /status_light rviz_light_display/msg/LightCommand \
  "{color: {r: 0.0, g: 1.0, b: 0.0, a: 1.0}, brightness: 0.0}"
```
