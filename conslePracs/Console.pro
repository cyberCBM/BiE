QT += core
QT -= gui

CONFIG += c++17

TARGET = qtConsole
CONFIG += console PRECOMPILED_HEADER
CONFIG -= app_bundle

TEMPLATE = app

#using precompiled headers(PCH)
PRECOMPILED_HEADER = stdafx.h

SOURCES += main.cpp \
    stdafx.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    stdafx.h \
    targetver.h \
    DP/AbstractFactory.h \
    DP/Adapter.h \
    DP/Bridge.h \
    DP/Command.h \
    DP/Composite.h \
    DP/COR.h \
    DP/Decorator.h \
    DP/Facade.h \
    DP/Factory.h \
    DP/Observer.h \
    DP/Prototype.h \
    DP/Proxy.h \
    DP/Strategy.h \
    DP/Templated.h \
    c++11/bind.h \
    c++11/customSignalSlot.h \
    c++11/functor.h \
    c++11/generics.h \
    c++11/highOrderFunctions.h \
    c++11/moves.h \
    c++11/myData.h \
    c++11/rvalues.h \
    c++14/constExpr_VarTemplates.h \
    c++14/declTypeAuto.h \
    c++14/funcTypeReduction.h \
    c++14/veriableTemplates.h
