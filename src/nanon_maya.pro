include(qtconfig)

QT += widgets gui

TARGET = nanon

HEADERS += \
    $$PWD/nanon.hpp \
    $$PWD/nanon_maya.hpp \
    $$PWD/io/nanon_textmate.hpp

SOURCES += \
    $$PWD/nanon.cpp \
    $$PWD/nanon_maya.cpp \
    $$PWD/io/nanon_textmate.cpp
