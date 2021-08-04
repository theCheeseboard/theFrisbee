QT       += core gui dbus thelib
SHARE_APP_NAME = thefrisbee

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    components/partitionvisualisation.cpp \
    components/sizeeditbox.cpp \
    diskPanes/diskpanecomponent.cpp \
    diskPanes/overviewdiskpane.cpp \
    diskmodel.cpp \
    diskpane.cpp \
    jobs/editpartitionjob.cpp \
    jobs/erasecdrwjob.cpp \
    jobs/progress/editpartitionjobprogress.cpp \
    jobs/progress/erasecdrwjobprogress.cpp \
    jobs/progress/restorediskjobprogress.cpp \
    jobs/progress/restoreopticaljobprogress.cpp \
    jobs/restorediskjob.cpp \
    jobs/restorejob.cpp \
    jobs/restoreopticaljob.cpp \
    main.cpp \
    mainwindow.cpp \
    operations/creatediskimagepopover.cpp \
    operations/eraseopticalpopover.cpp \
    operations/erasepartitionpopover.cpp \
    operations/erasepartitiontablepopover.cpp \
    operations/imagepopover.cpp \
    operations/partitionpopover.cpp \
    operations/restoreopticalpopover.cpp \
    optical/opticalerrortracker.cpp \
    partitioninformation.cpp

HEADERS += \
    components/partitionvisualisation.h \
    components/sizeeditbox.h \
    diskPanes/diskpanecomponent.h \
    diskPanes/overviewdiskpane.h \
    diskmodel.h \
    diskpane.h \
    jobs/editpartitionjob.h \
    jobs/erasecdrwjob.h \
    jobs/progress/editpartitionjobprogress.h \
    jobs/progress/erasecdrwjobprogress.h \
    jobs/progress/restorediskjobprogress.h \
    jobs/progress/restoreopticaljobprogress.h \
    jobs/restorediskjob.h \
    jobs/restorejob.h \
    jobs/restoreopticaljob.h \
    mainwindow.h \
    operations/creatediskimagepopover.h \
    operations/eraseopticalpopover.h \
    operations/erasepartitionpopover.h \
    operations/erasepartitiontablepopover.h \
    operations/imagepopover.h \
    operations/partitionpopover.h \
    operations/restoreopticalpopover.h \
    optical/opticalerrortracker.h \
    partitioninformation.h

FORMS += \
    diskPanes/overviewdiskpane.ui \
    diskpane.ui \
    jobs/progress/editpartitionjobprogress.ui \
    jobs/progress/erasecdrwjobprogress.ui \
    jobs/progress/restorediskjobprogress.ui \
    jobs/progress/restoreopticaljobprogress.ui \
    mainwindow.ui \
    operations/creatediskimagepopover.ui \
    operations/eraseopticalpopover.ui \
    operations/erasepartitionpopover.ui \
    operations/erasepartitiontablepopover.ui \
    operations/imagepopover.ui \
    operations/partitionpopover.ui \
    operations/restoreopticalpopover.ui

DESKTOP_FILE = com.vicr123.thefrisbee.desktop

unix:!macx {
    # Include the-libs build tools
    equals(THELIBS_BUILDTOOLS_PATH, "") {
        THELIBS_BUILDTOOLS_PATH = $$[QT_INSTALL_PREFIX]/share/the-libs/pri
    }
    include($$THELIBS_BUILDTOOLS_PATH/buildmaster.pri)

    DEFINES += SYSTEM_LIBRARY_DIRECTORY=\\\"$$THELIBS_INSTALL_LIB\\\"

    QT += thelib
    TARGET = thefrisbee

    CONFIG += link_pkgconfig

    target.path = $$THELIBS_INSTALL_BIN

    icon.path = $$THELIBS_INSTALL_PREFIX/share/icons/hicolor/scalable/apps/
    icon.files = icons/thefrisbee.svg

    defaults.files = defaults.conf
    defaults.path = $$THELIBS_INSTALL_SETTINGS/theSuite/theFrisbee/

    INSTALLS += target icon defaults
}

unix:!macx: LIBS += -L$$OUT_PWD/../libthefrisbee/ -lthefrisbee

INCLUDEPATH += $$PWD/../libthefrisbee
DEPENDPATH += $$PWD/../libthefrisbee

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.thefrisbee.desktop
