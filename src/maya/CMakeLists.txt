
#set(CMAKE_PREFIX_PATH
#    "${MAYA_DEVKIT}/Qt/lib/cmake/Qt6Core/Qt6CoreConfig.cmake"
#    ${CMAKE_PREFIX_PATH}
#)
#
#set(Qt6_DIR "${MAYA_DEVKIT}/Qt/lib/cmake/Qt6")
#
#find_package(Qt6 REQUIRED COMPONENTS
#    Core
#    Gui
#    Widgets
#)


message("Qt version: ${Qt6_VERSION}")
message("Qt location: ${Qt6_DIR}")

get_target_property(QT_WIDGETS_LOCATION Qt6::Widgets IMPORTED_LOCATION)
message(STATUS "Qt6 Widgets location: ${QT_WIDGETS_LOCATION}")

get_target_property(QT_WIDGETS_FRAMEWORK Qt6::Widgets IMPORTED_SONAME)
message(STATUS "Qt6 Widgets soname: ${QT_WIDGETS_FRAMEWORK}")

#set(MAYA_QT_ROOT "${MAYA_DEVKIT}/Qt")
#
#add_library(MayaQt::Core SHARED IMPORTED)
#set_target_properties(MayaQt::Core PROPERTIES
#    IMPORTED_LOCATION
#        "${MAYA_QT_ROOT}/lib/QtCore.framework/QtCore"
#    INTERFACE_INCLUDE_DIRECTORIES
#        "${MAYA_QT_ROOT}/include"
#)
#
#add_library(MayaQt::Gui SHARED IMPORTED)
#set_target_properties(MayaQt::Gui PROPERTIES
#    IMPORTED_LOCATION
#        "${MAYA_QT_ROOT}/lib/QtGui.framework/QtGui"
#    INTERFACE_INCLUDE_DIRECTORIES
#        "${MAYA_QT_ROOT}/include"
#)
#
#add_library(MayaQt::Widgets SHARED IMPORTED)
#set_target_properties(MayaQt::Widgets PROPERTIES
#    IMPORTED_LOCATION
#        "${MAYA_QT_ROOT}/lib/QtWidgets.framework/QtWidgets"
#    INTERFACE_INCLUDE_DIRECTORIES
#        "${MAYA_QT_ROOT}/include"
#)


add_library(nanon_maya MODULE
    nanon_maya_interpreter.hpp
    nanon_maya_interpreter.cpp
    nanon_maya.hpp
    nanon_maya.cpp
)

message("Maya includes: ${MAYA_INCLUDE_DIR}")

target_include_directories(nanon_maya PRIVATE
    ${MAYA_INCLUDE_DIR}
)

message("Maya libs: ${MAYA_LIBRARY_DIR}")
#target_link_libraries(nanon_maya PRIVATE
#    nanon_core
#    Maya:OpenMaya
#    Maya:Foundation
#)

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
