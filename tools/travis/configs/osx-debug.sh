#!/bin/sh
source "$CONFIG_FOLDER/osx-source-qt.sh"

$CMAKE_BIN $CMAKE_COMMON_FLAGS -DSCORE_CONFIGURATION=debug ..
$CMAKE_BIN --build . --target all_unity -- -j2
