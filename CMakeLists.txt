cmake_minimum_required(VERSION 3.25)
project(Dynamic_Path_finding_Visualizer)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(Dynamic_Path_finding_Visualizer main.cpp)
target_link_libraries(Dynamic_Path_finding_Visualizer sfml-graphics sfml-window sfml-system)
