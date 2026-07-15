
cmake_minimum_required(VERSION 3.16)
project(nanon VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)


# OPTIONS
option(BUILD_MAYA_PLUGIN "Build the Maya plugin target" OFF)
option(MAYA_DEVKIT "The path to maya devkit" "")
#set(MAYA_DEVKIT
#    ""
#    CACHE PATH "Path to Maya DevKit")



add_subdirectory(external/nlohmann/json)

if(BUILD_MAYA_PLUGIN)
     message("Finding maya qt libs...")
     #set(CMAKE_PREFIX_PATH "${MAYA_DEVKIT}/Qt")
     set(CMAKE_PREFIX_PATH
         "${MAYA_DEVKIT}/Qt/lib/cmake/Qt6Core/Qt6CoreConfig.cmake"
         ${CMAKE_PREFIX_PATH}
     )
     #set(Qt6_DIR "${MAYA_DEVKIT}/Qt/lib/cmake/Qt6")

     find_package(Qt6 REQUIRED COMPONENTS
         Core
         Widgets
     )
else()
     message("Finding default qt libs...")
     # Find Qt6 and required components
     find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

endif()

set(CMAKE_AUTOMOC ON)


add_subdirectory(src/core)
add_subdirectory(src/standalone)

if(BUILD_MAYA_PLUGIN)
     list(APPEND CMAKE_MODULE_PATH "${MAYA_DEVKIT}/cmake/modules")
     find_package(Maya REQUIRED)

     add_subdirectory(src/maya)
endif()


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

