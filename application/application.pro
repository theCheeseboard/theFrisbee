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
    diskpane.cpp \
    main.cpp \
    mainwindow.cpp \
    operations/creatediskimagepopover.cpp

HEADERS += \
    diskPanes/diskpanecomponent.h \
    diskPanes/overviewdiskpane.h \
    diskpane.h \
    mainwindow.h \
    operations/creatediskimagepopover.h

FORMS += \
    diskPanes/overviewdiskpane.ui \
    diskpane.ui \
    mainwindow.ui \
    operations/creatediskimagepopover.ui

DESKTOP_FILE = com.vicr123.thefrisbee.desktop
DESKTOP_FILE_BLUEPRINT = com.vicr123.thefrisbee_blueprint.desktop

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

    blueprint {
        metainfo.files = com.vicr123.thefrisbee_blueprint.metainfo.xml
        icon.files = icons/com.vicr123.thefrisbee_blueprint.svg
    } else {
        metainfo.files = com.vicr123.thefrisbee.metainfo.xml
        icon.files = icons/com.vicr123.thefrisbee.svg
    }

    icon.path = $$THELIBS_INSTALL_PREFIX/share/icons/hicolor/scalable/apps/
    metainfo.path = $$THELIBS_INSTALL_PREFIX/share/metainfo

    defaults.files = defaults.conf
    defaults.path = $$THELIBS_INSTALL_SETTINGS/theSuite/theFrisbee/

    INSTALLS += target icon metainfo defaults
}

unix:!macx: LIBS += -L$$OUT_PWD/../libthefrisbee/ -lthefrisbee

INCLUDEPATH += $$PWD/../libthefrisbee
DEPENDPATH += $$PWD/../libthefrisbee

RESOURCES += \
    resources.qrc

DISTFILES += \
    com.vicr123.thefrisbee.desktop
