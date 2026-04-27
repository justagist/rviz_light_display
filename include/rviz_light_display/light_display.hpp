#ifndef RVIZ_LIGHT_DISPLAY__LIGHT_DISPLAY_HPP_
#define RVIZ_LIGHT_DISPLAY__LIGHT_DISPLAY_HPP_

#include <memory>

#include <Ogre.h>

#include "geometry_msgs/msg/pose.hpp"
#include "rviz_common/properties/color_property.hpp"
#include "rviz_common/properties/enum_property.hpp"
#include "rviz_common/properties/float_property.hpp"
#include "rviz_common/properties/quaternion_property.hpp"
#include "rviz_common/properties/tf_frame_property.hpp"
#include "rviz_common/properties/vector_property.hpp"
#include "rviz_common/ros_topic_display.hpp"
#include "rviz_light_display/msg/light_command.hpp"
#include "rviz_rendering/objects/shape.hpp"

namespace rviz_light_display
{

class LightDisplay : public rviz_common::RosTopicDisplay<rviz_light_display::msg::LightCommand>
{
    Q_OBJECT

public:
    LightDisplay();
    ~LightDisplay() override;

    void onInitialize() override;
    void reset() override;
    void update(float wall_dt, float ros_dt) override;

protected:
    void processMessage(rviz_light_display::msg::LightCommand::ConstSharedPtr msg) override;

private Q_SLOTS:
    void updateVisualProperties();

private:
    geometry_msgs::msg::Pose getConfiguredPose() const;
    rviz_rendering::Shape::Type getConfiguredShape() const;
    Ogre::Quaternion getShapeUprightOrientation() const;
    Ogre::Vector3 getConfiguredScale() const;
    void updateLightShape();
    void updateDimensionProperties();
    void updateScenePose();
    void updateLightMaterial();
    void applyConfiguredColorIfNeeded();

    rviz_common::properties::TfFrameProperty* frame_property_;
    rviz_common::properties::VectorProperty* position_property_;
    rviz_common::properties::QuaternionProperty* orientation_property_;
    rviz_common::properties::EnumProperty* shape_property_;
    rviz_common::properties::FloatProperty* diameter_property_;
    rviz_common::properties::FloatProperty* width_property_;
    rviz_common::properties::FloatProperty* depth_property_;
    rviz_common::properties::FloatProperty* height_property_;
    rviz_common::properties::ColorProperty* default_color_property_;
    rviz_common::properties::FloatProperty* default_alpha_property_;
    rviz_common::properties::FloatProperty* off_alpha_property_;
    rviz_common::properties::ColorProperty* off_color_property_;

    std::unique_ptr<rviz_rendering::Shape> light_shape_;

    Ogre::ColourValue active_color_;
    float brightness_;
    bool enabled_;
    bool has_runtime_color_;
};

} // namespace rviz_light_display

#endif // RVIZ_LIGHT_DISPLAY__LIGHT_DISPLAY_HPP_
