#-------------------------------------------------
#
# Project created by QtCreator 2021-02-03T13:29:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Omega
TEMPLATE = app

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    hexagon.cpp \
    canvas.cpp \
    boarddialog.cpp \
    gamestate.cpp \
    cell.cpp \
    aibotbase.cpp \
    randombot.cpp \
    mast.cpp \
    mctsbot.cpp \
    evenscheduler.cpp

HEADERS += \
        mainwindow.h \
    hexagon.h \
    canvas.h \
    boarddialog.h \
    gamestate.h \
    cell.h \
    aibotbase.h \
    randombot.h \
    hmcravenode.h \
    mcts.h \
    zhashtable.h \
    recyclingnode.h \
    node.h \
    mast.h \
    stopscheduler.h \
    mctsbot.h \
    evenscheduler.h \
    uctnode.h

FORMS += \
        mainwindow.ui \
    boarddialog.ui
