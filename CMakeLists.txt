cmake_minimum_required(VERSION 3.20)
project(rviz_light_display)

include($ENV{WORKSPACE}/src/utils/cmake/project_defaults.cmake)
setup_project()

set(CMAKE_AUTOMOC ON)
set(LIGHT_DISPLAY_LIBRARY ${PROJECT_NAME}_plugin)

find_package(ament_cmake REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(pluginlib REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rviz_common REQUIRED)
find_package(rviz_rendering REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/LightCommand.msg"
  DEPENDENCIES std_msgs
)

add_library(${LIGHT_DISPLAY_LIBRARY} SHARED
  include/rviz_light_display/light_display.hpp
  src/light_display.cpp
)

set_target_properties(${LIGHT_DISPLAY_LIBRARY} PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME}
)

target_include_directories(${LIGHT_DISPLAY_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

ament_target_dependencies(${LIGHT_DISPLAY_LIBRARY}
  geometry_msgs
  pluginlib
  rclcpp
  rviz_common
  rviz_rendering
  std_msgs
)

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(${LIGHT_DISPLAY_LIBRARY}
  "${cpp_typesupport_target}"
)

add_dependencies(${LIGHT_DISPLAY_LIBRARY} ${PROJECT_NAME}__rosidl_generator_cpp)

pluginlib_export_plugin_description_file(rviz_common plugin_description.xml)

install(TARGETS ${LIGHT_DISPLAY_LIBRARY}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY msg/
  DESTINATION share/${PROJECT_NAME}/msg
)

install(FILES
  plugin_description.xml
  README.md
  DESTINATION share/${PROJECT_NAME}
)

ament_export_dependencies(
  geometry_msgs
  pluginlib
  rclcpp
  rviz_common
  rviz_rendering
  rosidl_default_runtime
  std_msgs
)

ament_package()
