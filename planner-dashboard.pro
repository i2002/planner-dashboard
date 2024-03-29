QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    apppage.cpp \
    commands/droidcamworkerthread.cpp \
    droidcamcontroller.cpp \
    main.cpp \
    multimonitorcontroller.cpp \
    overlaywindow.cpp \
    plannerdashboard.cpp \
    schemehandler.cpp

HEADERS += \
    apppage.h \
    commands/droidcamworkerthread.h \
    droidcamcontroller.h \
    multimonitorcontroller.h \
    overlaywindow.h \
    plannerdashboard.h \
    schemehandler.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    planner-dashboard.qrc

include(vendor/vendor.pri)

DISTFILES += \
    .gitignore
