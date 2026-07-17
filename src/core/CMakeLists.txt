
include_directories(external/nlohmann/json)

add_library(nanon_core SHARED
    nanon/interpreter/base.hpp
    nanon/interpreter/python.cpp
    nanon/interpreter/python.hpp
    nanon/io/socket.hpp
    nanon/io/textmate.cpp
    nanon/io/textmate.hpp
    nanon/textmate/engine.cpp
    nanon/textmate/engine.hpp
    nanon/textmate/grammar.cpp
    nanon/textmate/grammar.hpp
    nanon/textmate/rule.cpp
    nanon/textmate/rule.hpp
    nanon/widgets/editor.cpp
    nanon/widgets/editor.hpp
    nanon/widgets/highlighter.cpp
    nanon/widgets/highlighter.hpp
    nanon/window.cpp
    nanon/window.hpp
)

target_link_libraries(nanon_core PUBLIC Qt6::Core Qt6::Widgets nlohmann_json)

target_include_directories(nanon_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Copy resource dir, define it as compile definition
target_compile_definitions(nanon_core PRIVATE
    RESOURCE_PATH="${CMAKE_BINARY_DIR}/resources/"
)


# append to PYTHONPATH
list(APPEND PYTHONPATH "${CMAKE_BINARY_DIR}/python/")
