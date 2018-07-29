QT += widgets gui network

SOURCES += \
    main.cpp \
    board.cpp \
    gamedata.cpp \
    networkplay.cpp \
    login.cpp \
    choosepattern.cpp \
    mainwnd.cpp \
    humancomputeplay.cpp \
    encryption.c \
    changperinfo.cpp \
    brain.cpp
    autobrain.cpp \

HEADERS += \
    board.h \
    gamedata.h \
    networkplay.h \
    login.h \
    choosepattern.h \
    mainwnd.h \
    humancomputeplay.h \
    encryption.h \
    changperinfo.h \
    brain.h

LIBS += -lcrypto

CONFIG += c++11
