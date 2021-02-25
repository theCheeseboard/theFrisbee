QT += gui dbus thelib
TARGET = thefrisbee

TEMPLATE = lib
DEFINES += LIBTHEFRISBEE_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DriveObjects/blockinterface.cpp \
    DriveObjects/diskinterface.cpp \
    DriveObjects/diskobject.cpp \
    DriveObjects/driveinterface.cpp \
    DriveObjects/encryptedinterface.cpp \
    DriveObjects/filesysteminterface.cpp \
    DriveObjects/loopinterface.cpp \
    DriveObjects/partitioninterface.cpp \
    DriveObjects/partitiontableinterface.cpp \
    driveobjectmanager.cpp

HEADERS += \
    DriveObjects/blockinterface.h \
    DriveObjects/diskinterface.h \
    DriveObjects/diskobject.h \
    DriveObjects/driveinterface.h \
    DriveObjects/encryptedinterface.h \
    DriveObjects/filesysteminterface.h \
    DriveObjects/loopinterface.h \
    DriveObjects/partitioninterface.h \
    DriveObjects/partitiontableinterface.h \
    libthefrisbee_global.h \
    driveobjectmanager.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_LIBS]

    headers.files = *.h
    headers.path = $$[QT_INSTALL_HEADERS]/libthefrisbee/

    driveobjectheaders.files = DriveObjects/*.h
    driveobjectheaders.path = $$[QT_INSTALL_HEADERS]/libthefrisbee/DriveObjects/

    translations.files = translations/*.qm
    translations.path = /usr/share/thefrisbee/libthefrisbee/translations

    module.files = qt_frisbee.pri
    module.path = $$[QMAKE_MKSPECS]/modules

    trigger.files = trigger-uevent.sh
    trigger.path = $$[QT_INSTALL_LIBS]/libthefrisbee

    polkit.files = com.vicr123.trigger-uevent.policy
    polkit.path = /usr/share/polkit-1/actions

    INSTALLS += target translations headers driveobjectheaders module trigger polkit
}

DISTFILES += \
    com.vicr123.trigger-uevent.policy \
    qt_frisbee.pri \
    trigger-uevent.sh
