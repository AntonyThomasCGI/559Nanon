
include_directories(external/nlohmann/json)

add_library(nanon_core SHARED
    nanon/commands/action.cpp
    nanon/commands/action.hpp
    nanon/commands/registry.cpp
    nanon/commands/registry.hpp
    nanon/interpreter/base.hpp
    nanon/interpreter/python.cpp
    nanon/interpreter/python.hpp
    nanon/io/config.cpp
    nanon/io/config.hpp
    nanon/io/socket.hpp
    nanon/edits/edit.hpp
    nanon/edits/language.cpp
    nanon/edits/language.hpp
    nanon/style/theme_manager.cpp
    nanon/style/theme_manager.hpp
    nanon/style/theme.cpp
    nanon/style/theme.hpp
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
    nanon/widgets/search_menu.cpp
    nanon/widgets/search_menu.hpp
    nanon/widgets/tab_bar.cpp
    nanon/widgets/tab_bar.hpp
    nanon/window.cpp
    nanon/window.hpp
    nanon/session.cpp
    nanon/session.hpp
)

target_link_libraries(nanon_core PUBLIC Qt6::Core Qt6::Widgets nlohmann_json)

target_include_directories(nanon_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



add_custom_command(
    TARGET nanon_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
       ${CMAKE_SOURCE_DIR}/python
       ${CMAKE_BINARY_DIR}/python

    COMMAND ${CMAKE_COMMAND} -E copy_directory
       ${CMAKE_SOURCE_DIR}/resources
       ${CMAKE_CURRENT_BINARY_DIR}/resources

    COMMENT "Copying resources and python directory to build directory..."
)

# Copy resource dir, define it as compile definition
target_compile_definitions(nanon_core PRIVATE
    RESOURCE_PATH="${CMAKE_CURRENT_BINARY_DIR}/resources/"
)


# TODO
#target_compile_definitions(nanon_core PRIVATE
#    PYTHON_MODULE_PATH="${CMAKE_CURRENT_BINARY_DIR}/python/"
#)

