#include "rviz_light_display/light_display.hpp"

#include <algorithm>
#include <string>

#include <OgreMaterial.h>
#include <OgrePass.h>
#include <OgreTechnique.h>
#include <pluginlib/class_list_macros.hpp>

#include "rviz_common/display_context.hpp"
#include "rviz_common/frame_manager_iface.hpp"
#include "rviz_common/properties/status_property.hpp"

namespace rviz_light_display
{

namespace
{

float clampUnit(float value) { return std::clamp(value, 0.0f, 1.0f); }

} // namespace

LightDisplay::LightDisplay()
    : frame_property_(new rviz_common::properties::TfFrameProperty(
          "Light Frame", "", "TF frame that the configured light pose is relative to.", this, nullptr, false,
          SLOT(updateVisualProperties()), this
      )),
      position_property_(new rviz_common::properties::VectorProperty(
          "Position", Ogre::Vector3::ZERO, "Light position relative to the selected frame.", this,
          SLOT(updateVisualProperties()), this
      )),
      orientation_property_(new rviz_common::properties::QuaternionProperty(
          "Orientation", Ogre::Quaternion::IDENTITY, "Light orientation relative to the selected frame.", this,
          SLOT(updateVisualProperties()), this
      )),
      diameter_property_(new rviz_common::properties::FloatProperty(
          "Diameter", 0.05f, "Rendered diameter of the light in meters.", this, SLOT(updateVisualProperties()), this
      )),
      off_alpha_property_(new rviz_common::properties::FloatProperty(
          "Off Alpha", 0.15f, "Transparency used when the light is off.", this, SLOT(updateVisualProperties()), this
      )),
      off_color_property_(new rviz_common::properties::ColorProperty(
          "Off Color", QColor(40, 40, 40), "Fallback color shown while the light is off.", this,
          SLOT(updateVisualProperties()), this
      )),
      active_color_(1.0f, 0.0f, 0.0f, 1.0f), brightness_(0.0f), enabled_(false)
{
    diameter_property_->setMin(0.001f);
    off_alpha_property_->setMin(0.0f);
    off_alpha_property_->setMax(1.0f);
}

LightDisplay::~LightDisplay() = default;

void LightDisplay::onInitialize()
{
    RTDClass::onInitialize();
    frame_property_->setFrameManager(context_->getFrameManager());

    light_shape_ = std::make_unique<rviz_rendering::Shape>(
        rviz_rendering::Shape::Sphere, context_->getSceneManager(), scene_node_
    );

    updateVisualProperties();
}

void LightDisplay::reset()
{
    RTDClass::reset();
    brightness_ = 0.0f;
    enabled_ = false;
    updateVisualProperties();
}

void LightDisplay::update(float wall_dt, float ros_dt)
{
    RTDClass::update(wall_dt, ros_dt);
    updateScenePose();
}

void LightDisplay::processMessage(rviz_light_display::msg::LightCommand::ConstSharedPtr msg)
{
    active_color_ = Ogre::ColourValue(
        clampUnit(msg->color.r), clampUnit(msg->color.g), clampUnit(msg->color.b), clampUnit(msg->color.a)
    );
    brightness_ = clampUnit(msg->brightness);
    enabled_ = brightness_ > 0.0f;

    updateLightMaterial();
    queueRender();
}

void LightDisplay::updateVisualProperties()
{
    updateScenePose();
    updateLightMaterial();
    queueRender();
}

geometry_msgs::msg::Pose LightDisplay::getConfiguredPose() const
{
    geometry_msgs::msg::Pose pose;
    const Ogre::Vector3 position = position_property_->getVector();
    const Ogre::Quaternion orientation = orientation_property_->getQuaternion();

    pose.position.x = position.x;
    pose.position.y = position.y;
    pose.position.z = position.z;
    pose.orientation.x = orientation.x;
    pose.orientation.y = orientation.y;
    pose.orientation.z = orientation.z;
    pose.orientation.w = orientation.w;
    return pose;
}

void LightDisplay::updateScenePose()
{
    if (!light_shape_)
    {
        return;
    }

    const std::string frame = frame_property_->getFrameStd();
    if (frame.empty())
    {
        setStatus(
            rviz_common::properties::StatusProperty::Warn, "Transform",
            "Set Light Frame to place the light in the scene."
        );
        light_shape_->getRootNode()->setVisible(false);
        return;
    }

    Ogre::Vector3 position;
    Ogre::Quaternion orientation;
    if (!context_->getFrameManager()->transform(
            frame, rclcpp::Time(0, 0, RCL_ROS_TIME), getConfiguredPose(), position, orientation
        ))
    {
        setMissingTransformToFixedFrame(frame, "light pose");
        light_shape_->getRootNode()->setVisible(false);
        return;
    }

    light_shape_->setPosition(position);
    light_shape_->setOrientation(orientation);

    const float diameter = std::max(diameter_property_->getFloat(), 0.001f);
    light_shape_->setScale(Ogre::Vector3(diameter, diameter, diameter));
    light_shape_->getRootNode()->setVisible(true);
    setTransformOk();
}

void LightDisplay::updateLightMaterial()
{
    if (!light_shape_)
    {
        return;
    }

    Ogre::ColourValue display_color =
        enabled_ ? Ogre::ColourValue(
                       active_color_.r * brightness_, active_color_.g * brightness_, active_color_.b * brightness_,
                       active_color_.a
                   )
                 : Ogre::ColourValue(
                       off_color_property_->getOgreColor().r, off_color_property_->getOgreColor().g,
                       off_color_property_->getOgreColor().b, clampUnit(off_alpha_property_->getFloat())
                   );

    light_shape_->setColor(display_color);

    Ogre::MaterialPtr material = light_shape_->getMaterial();
    if (!material || material->getNumTechniques() == 0)
    {
        return;
    }

    Ogre::Technique* technique = material->getTechnique(0);
    if (technique == nullptr || technique->getNumPasses() == 0)
    {
        return;
    }

    Ogre::Pass* pass = technique->getPass(0);
    if (enabled_)
    {
        pass->setSelfIllumination(
            active_color_.r * brightness_, active_color_.g * brightness_, active_color_.b * brightness_
        );
    }
    else
    {
        pass->setSelfIllumination(0.0f, 0.0f, 0.0f);
    }
}

} // namespace rviz_light_display

PLUGINLIB_EXPORT_CLASS(rviz_light_display::LightDisplay, rviz_common::Display)
