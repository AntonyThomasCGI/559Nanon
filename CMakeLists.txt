
cmake_minimum_required(VERSION 3.16)
project(nanon VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)


option(BUILD_MAYA_PLUGIN "Build the Maya plugin target" OFF)

add_subdirectory(external/nlohmann/json)

# Find Qt6 and required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)


add_subdirectory(src/core)
add_subdirectory(src/standalone)

if(BUILD_MAYA_PLUGIN)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    find_package(Maya REQUIRED)

    add_subdirectory(src/maya)
endif()


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

