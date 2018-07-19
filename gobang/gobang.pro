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
    encryption.c

HEADERS += \
    board.h \
    gamedata.h \
    networkplay.h \
    login.h \
    choosepattern.h \
    mainwnd.h \
    humancomputeplay.h \
    encryption.h

LIBS += -lcrypto

CONFIG += c++11
