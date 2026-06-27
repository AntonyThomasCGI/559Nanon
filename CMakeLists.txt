
cmake_minimum_required(VERSION 3.16)
project(nanon VERSION 1.0 LANGUAGES CXX)

# Find Qt6 and required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)


add_subdirectory(external/nlohmann/json)

include_directories(external/nlohmann/json)



# Add your source files
add_executable(${PROJECT_NAME}
    src/interpreter/nanon_python.cpp
    src/interpreter/nanon_python.hpp
    src/io/nanon_socket.cpp
    src/io/nanon_socket.hpp
    src/io/nanon_textmate.cpp
    src/io/nanon_textmate.hpp
    src/nanon.cpp
    src/nanon.hpp
    src/nanon_standalone.cpp
    src/textmate/nanon_rule.cpp
    src/textmate/nanon_rule.hpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link Qt6 libraries to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets nlohmann_json)



file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/syntaxes
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Inject above resource path as a compile definition for the project
target_compile_definitions(${PROJECT_NAME} PRIVATE
    SYNTAX_PATH="${CMAKE_CURRENT_BINARY_DIR}/syntaxes/"
)

# Copy python dir, append to PYTHONPATH
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND PYTHONPATH "${CMAKE_CURRENT_DIR}/python")