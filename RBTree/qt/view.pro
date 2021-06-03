TEMPLATE = app
QT += qml quick widgets
CONFIG += debug console
INCLUDEPATH += include
DESTDIR = build
HEADERS = $$files(include/*.h, true)
#include/RBTree.h include/Backend.h
SOURCES = $$files(src/*.cpp, true) $$files(src/*.c, true)
#main.cpp RBTree.c Backend.cpp
#RESOURCES = qml.qrc
