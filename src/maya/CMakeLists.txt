
message("Qt version: ${Qt6_VERSION}")
message("Qt location: ${Qt6_DIR}")

get_target_property(QT_WIDGETS_LOCATION Qt6::Widgets IMPORTED_LOCATION)
message(STATUS "Qt6 Widgets location: ${QT_WIDGETS_LOCATION}")

get_target_property(QT_WIDGETS_FRAMEWORK Qt6::Widgets IMPORTED_SONAME)
message(STATUS "Qt6 Widgets soname: ${QT_WIDGETS_FRAMEWORK}")


add_library(nanon_maya MODULE
    maya_interpreter.cpp
    maya_interpreter.hpp
    maya_nanon.cpp
    maya_nanon.hpp
)

message("Maya includes: ${MAYA_INCLUDE_DIR}")

target_include_directories(nanon_maya PRIVATE
    ${MAYA_INCLUDE_DIR}
)

message("Maya libs: ${MAYA_LIBRARY_DIR}")

include_directories(${MAYA_INCLUDE_DIR})
link_directories(${MAYA_LIBRARY_DIR})

target_link_libraries(nanon_maya PRIVATE
    nanon_core
    ${MAYA_LIBRARY_DIR}/libOpenMaya.dylib
    ${MAYA_LIBRARY_DIR}/libOpenMayaUI.dylib
    ${MAYA_LIBRARY_DIR}/libFoundation.dylib
    Qt::Widgets
    Qt::Gui
    Qt::Core
)


if(APPLE)
    set_target_properties(nanon_maya PROPERTIES
        PREFIX ""
        SUFFIX ".bundle"
    )
elseif(WIN32)
    set_target_properties(nanon_maya PROPERTIES
        PREFIX ""
        SUFFIX ".mll"
    )
elseif(UNIX)
    set_target_properties(nanon_maya PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
endif()
