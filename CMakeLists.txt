
cmake_minimum_required(VERSION 3.16)
project(nanon VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)



# OPTIONS
option(BUILD_MAYA_PLUGIN "Build the Maya plugin target" OFF)
option(MAYA_DEVKIT "The path to maya devkit" "")



add_subdirectory(external/nlohmann/json)


if(BUILD_MAYA_PLUGIN)
    message("Finding maya qt libs...")

    # There must be a cleaner way to do this...
    set(CMAKE_PREFIX_PATH
        "${MAYA_DEVKIT}/Qt/lib/cmake/Qt6Core/Qt6CoreConfig.cmake"
        ${CMAKE_PREFIX_PATH}
    )

    find_package(Qt6 REQUIRED COMPONENTS
        Core
        Widgets
    )
else()
    message("Finding default qt libs...")

    find_package(Qt6 REQUIRED COMPONENTS
        Core
        Widgets
    )

endif()


add_subdirectory(src/core)
add_subdirectory(src/standalone)

if(BUILD_MAYA_PLUGIN)
    list(APPEND CMAKE_MODULE_PATH "${MAYA_DEVKIT}/cmake/modules")
    find_package(Maya REQUIRED)

    add_subdirectory(src/maya)
endif()
