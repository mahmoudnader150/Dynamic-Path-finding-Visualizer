cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Dynamic_Path_finding_Visualizer VERSION 1.0)

# Find SFML package (ensure it's installed on your system)
find_package(SFML 2.6 REQUIRED graphics window system)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add all source files
set(SOURCES
        main.cpp
        Algorithms.cpp
        PathfindingVisualizer.cpp
        UIComponents.cpp
)

# Add all header files (to help IDEs, but not strictly necessary for the build)
set(HEADERS
        Algorithms.h
        PathfindingVisualizer.h
        UIComponents.h
        Constants.h
)

# Add the executable target
add_executable(Dynamic_Path_finding_Visualizer ${SOURCES} ${HEADERS})

# Link the SFML libraries (graphics, window, system)
target_link_libraries(Dynamic_Path_finding_Visualizer PRIVATE sfml-graphics sfml-window sfml-system)

# Ensure the include directories for SFML are correctly set
target_include_directories(Dynamic_Path_finding_Visualizer PRIVATE ${SFML_INCLUDE_DIR})
