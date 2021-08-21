QT       += core gui xml
QT       += serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = images/serial.ico #app icon


INCLUDEPATH += \
        app/inc \
        tool/inc


SOURCES += \
    app/src/console.cpp \
    app/src/main.cpp \
    app/src/mainwindow.cpp \
    app/src/optionsdialog.cpp \
    app/src/settingsdialog.cpp \
    tool/src/logger.cpp \
    tool/src/xmlhelper.cpp

HEADERS += \
    app/inc/console.h \
    app/inc/mainwindow.h \
    app/inc/myintValidator.h \
    app/inc/optionsdialog.h \
    app/inc/settingsdialog.h \
    tool/inc/logger.h \
    tool/inc/xmlhelper.h

FORMS += \
    app/ui/mainwindow.ui \
    app/ui/optionsdialog.ui \
    app/ui/settingsdialog.ui

TRANSLATIONS += language/zh_CN.ts \
                language/en_CN.ts

RESOURCES += \
    terminal.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


