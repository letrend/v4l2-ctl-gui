cmake_minimum_required(VERSION 2.6)
project(v4l2_ctl_gui)

SET(CMAKE_BUILD_TYPE Debug FORCE)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -std=c++11" )

find_package(Curses REQUIRED)

include_directories(include)

set(SOURCE_FILES src/main.cpp include/ncurses-v4l2-ctl.hpp)
add_executable(v4l2_ctl_gui ${SOURCE_FILES})
target_link_libraries(v4l2_ctl_gui ${CURSES_LIBRARIES} -lmenu)