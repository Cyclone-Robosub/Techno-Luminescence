#!/usr/bin/env bash
# yup i wrote this by hand
set -euo pipefail

LIB_DIR="${MICROROS_LIB_DIR:-$HOME/Arduino/libraries/micro_ros_arduino}"
META="$LIB_DIR/extras/library_generation/colcon.meta"

if [ ! -d "$LIB_DIR" ]; then
  echo "micro_ros_arduino not found at $LIB_DIR - clone it first:" >&2
  echo "  git clone -b jazzy https://github.com/micro-ROS/micro_ros_arduino.git $LIB_DIR" >&2
  exit 1
fi

if ! grep -q 'RMW_UXRCE_MAX_SUBSCRIPTIONS=10' "$META"; then
  sed -i 's/"-DRMW_UXRCE_MAX_SUBSCRIPTIONS=5"/"-DRMW_UXRCE_MAX_SUBSCRIPTIONS=10"/' "$META"
fi

docker pull microros/micro_ros_static_library_builder:jazzy
docker run --rm \
  -v "$LIB_DIR":/project \
  --env MICROROS_LIBRARY_FOLDER=extras \
  microros/micro_ros_static_library_builder:jazzy -p esp32
