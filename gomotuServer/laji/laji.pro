TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    ../mainAidFun.c \
    ../mainServer.c \
    ../threadFun.c \
    ../threadPoll.c

DISTFILES += \
    ../test测试参数

HEADERS +=

