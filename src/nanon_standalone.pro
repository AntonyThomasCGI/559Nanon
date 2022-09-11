include(qtconfig_standalone)

QT += widgets gui

TARGET = nanon

HEADERS += \
    $$PWD/nanon.hpp \
    $$PWD/io/nanon_textmate.hpp

SOURCES += \
    $$PWD/nanon.cpp \
    $$PWD/nanon_standalone.cpp \
    $$PWD/io/nanon_textmate.cpp
