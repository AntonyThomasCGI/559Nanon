
cmake_minimum_required(VERSION 3.16)
project(nanon VERSION 1.0 LANGUAGES CXX)

# Find Qt6 and required components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)


add_subdirectory(external/nlohmann/json)

include_directories(external/nlohmann/json)



add_executable(${PROJECT_NAME}
    src/core/interpreter/nanon_interpreter_base.hpp
    src/core/interpreter/nanon_python.cpp
    src/core/interpreter/nanon_python.hpp
    src/core/io/nanon_socket.hpp
    src/core/io/nanon_textmate.cpp
    src/core/io/nanon_textmate.hpp
    src/core/nanon.cpp
    src/core/nanon.hpp
    src/core/textmate/nanon_engine.cpp
    src/core/textmate/nanon_engine.hpp
    src/core/textmate/nanon_grammar.cpp
    src/core/textmate/nanon_grammar.hpp
    src/core/textmate/nanon_rule.cpp
    src/core/textmate/nanon_rule.hpp
    src/standalone/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link Qt6 libraries to your executable
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets nlohmann_json)



file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Inject above resource path as a compile definition for the project
target_compile_definitions(${PROJECT_NAME} PRIVATE
    RESOURCE_PATH="${CMAKE_CURRENT_BINARY_DIR}/resources/"
)

# Copy python dir, append to PYTHONPATH
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(APPEND PYTHONPATH "${CMAKE_CURRENT_DIR}/python")