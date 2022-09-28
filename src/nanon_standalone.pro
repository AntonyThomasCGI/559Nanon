include(qtconfig_standalone)

QT += widgets gui core

TARGET = nanon

HEADERS += \
    $$PWD/nanon.hpp \
    $$PWD/io/nanon_textmate.hpp \
    $$PWD/textmate/nanon_rule.hpp

SOURCES += \
    $$PWD/nanon.cpp \
    $$PWD/nanon_standalone.cpp \
    $$PWD/io/nanon_textmate.cpp \
    $$PWD/textmate/nanon_rule.cpp
