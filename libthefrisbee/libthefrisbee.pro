QT += gui dbus thelib

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
    DriveObjects/filesysteminterface.cpp \
    DriveObjects/partitioninterface.cpp \
    DriveObjects/partitiontableinterface.cpp \
    driveobjectmanager.cpp

HEADERS += \
    DriveObjects/blockinterface.h \
    DriveObjects/diskinterface.h \
    DriveObjects/diskobject.h \
    DriveObjects/driveinterface.h \
    DriveObjects/filesysteminterface.h \
    DriveObjects/partitioninterface.h \
    DriveObjects/partitiontableinterface.h \
    libthefrisbee_global.h \
    driveobjectmanager.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
