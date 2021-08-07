#!/bin/bash
BUILD_DIR="cmake-build-debug"
if [ ! -d "$BUILD_DIR" ]; then
  mkdir "$BUILD_DIR"
fi
cd "$BUILD_DIR"
cmake .. && make