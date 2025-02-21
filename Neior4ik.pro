QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    phonebook.cpp \
    contact.cpp \
    contactdialog.cpp \
    databasemanager.cpp \
    phonenumber.cpp \
    phonewidget.cpp \
    filestorage.cpp \
    databasestorage.cpp \
    storagechoicedialog.cpp

HEADERS += \
    mainwindow.h \
    phonebook.h \
    contact.h \
    contactdialog.h \
    databasemanager.h \
    phonenumber.h \
    phonewidget.h \
    istorage.h \
    filestorage.h \
    databasestorage.h \
    storagechoicedialog.h

#TRANSLATIONS += \
#    Neior4ik_ru_RU.ts

#CODECFORSRC = UTF-8

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_LRELEASE = C:/msys64/mingw64/bin/lrelease.exe
