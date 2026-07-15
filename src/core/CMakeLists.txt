
include_directories(external/nlohmann/json)

add_library(nanon_core SHARED
    interpreters/nanon_interpreter.hpp
    interpreters/nanon_python.cpp
    interpreters/nanon_python.hpp
    io/nanon_socket.hpp
    io/nanon_textmate.cpp
    io/nanon_textmate.hpp
    nanon.cpp
    nanon.hpp
    textmate/nanon_engine.cpp
    textmate/nanon_engine.hpp
    textmate/nanon_grammar.cpp
    textmate/nanon_grammar.hpp
    textmate/nanon_rule.cpp
    textmate/nanon_rule.hpp
)

target_link_libraries(nanon_core PUBLIC Qt6::Core Qt6::Widgets nlohmann_json)

target_include_directories(nanon_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Copy resource dir, define it as compile definition
target_compile_definitions(nanon_core PRIVATE
    RESOURCE_PATH="${CMAKE_BINARY_DIR}/resources/"
)


# append to PYTHONPATH
list(APPEND PYTHONPATH "${CMAKE_BINARY_DIR}/python/")
