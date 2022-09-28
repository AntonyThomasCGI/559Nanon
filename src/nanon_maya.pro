include(qtconfig_maya)

QT += widgets gui core

TARGET = nanon

HEADERS += \
    $$PWD/nanon.hpp \
    $$PWD/nanon_maya.hpp \
    $$PWD/io/nanon_textmate.hpp \
    $$PWD/textmate/nanon_rule.hpp

SOURCES += \
    $$PWD/nanon.cpp \
    $$PWD/nanon_maya.cpp \
    $$PWD/io/nanon_textmate.cpp \
    $$PWD/textmate/nanon_rule.cpp
