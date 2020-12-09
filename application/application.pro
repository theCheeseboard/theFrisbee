QT       += core gui dbus thelib
SHARE_APP_NAME = thefrisbee

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    diskPanes/diskpanecomponent.cpp \
    diskPanes/overviewdiskpane.cpp \
    diskmodel.cpp \
    diskpane.cpp \
    jobs/erasecdrwjob.cpp \
    jobs/progress/erasecdrwjobprogress.cpp \
    jobs/progress/restoreopticaljobprogress.cpp \
    jobs/restoreopticaljob.cpp \
    main.cpp \
    mainwindow.cpp \
    operations/eraseopticalpopover.cpp \
    operations/erasepartitionpopover.cpp \
    operations/erasepartitiontablepopover.cpp \
    operations/imagepopover.cpp \
    operations/partitionpopover.cpp \
    operations/restoreopticalpopover.cpp \
    optical/opticalerrortracker.cpp

HEADERS += \
    diskPanes/diskpanecomponent.h \
    diskPanes/overviewdiskpane.h \
    diskmodel.h \
    diskpane.h \
    jobs/erasecdrwjob.h \
    jobs/progress/erasecdrwjobprogress.h \
    jobs/progress/restoreopticaljobprogress.h \
    jobs/restoreopticaljob.h \
    mainwindow.h \
    operations/eraseopticalpopover.h \
    operations/erasepartitionpopover.h \
    operations/erasepartitiontablepopover.h \
    operations/imagepopover.h \
    operations/partitionpopover.h \
    operations/restoreopticalpopover.h \
    optical/opticalerrortracker.h

FORMS += \
    diskPanes/overviewdiskpane.ui \
    diskpane.ui \
    jobs/progress/erasecdrwjobprogress.ui \
    jobs/progress/restoreopticaljobprogress.ui \
    mainwindow.ui \
    operations/eraseopticalpopover.ui \
    operations/erasepartitionpopover.ui \
    operations/erasepartitiontablepopover.ui \
    operations/imagepopover.ui \
    operations/partitionpopover.ui \
    operations/restoreopticalpopover.ui

unix:!macx {
    # Include the-libs build tools
    include(/usr/share/the-libs/pri/buildmaster.pri)

    DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$[QT_INSTALL_LIBS]\\\"

    QT += thelib
    TARGET = thefrisbee

    CONFIG += link_pkgconfig

    target.path = /usr/bin

    desktop.path = /usr/share/applications
    desktop.files = com.vicr123.thefrisbee.desktop

    icon.path = /usr/share/icons/hicolor/scalable/apps/
    icon.files = icons/thefrisbee.svg

    defaults.files = defaults.conf
    defaults.path = /etc/theSuite/theFrisbee/

    INSTALLS += target desktop icon defaults
}

unix:!macx: LIBS += -L$$OUT_PWD/../libthefrisbee/ -lthefrisbee

INCLUDEPATH += $$PWD/../libthefrisbee
DEPENDPATH += $$PWD/../libthefrisbee

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.thefrisbee.desktop
