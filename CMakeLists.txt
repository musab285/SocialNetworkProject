cmake_minimum_required(VERSION 3.16)

project(SocialNetworkApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# Qt Configuration
# ---------------------------------------------------------
# Find the Qt6 package. Components: Widgets is the GUI module.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Automatically handle Qt's Meta-Object System (MOC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ---------------------------------------------------------
# Add Executable
# ---------------------------------------------------------
add_executable(SocialNetworkApp
    main.cpp
    SocialGraph.h
)

# Link the executable to the Qt Libraries
target_link_libraries(SocialNetworkApp PRIVATE Qt6::Widgets)