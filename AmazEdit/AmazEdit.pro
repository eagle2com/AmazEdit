TEMPLATE = app
CONFIG += console c++14 link_pkgconfig
CONFIG -= app_bundle
CONFIG -= qt
PKGCONFIG += glfw3 freetype2 glew

SOURCES += AmazEdit.cpp\
shader.cpp

HEADERS += shader.h\
stdafx.h
