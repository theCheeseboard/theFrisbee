QT       += core gui dbus thelib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    diskmodel.cpp \
    diskpane.cpp \
    jobs/erasecdrwjob.cpp \
    jobs/progress/erasecdrwjobprogress.cpp \
    jobs/progress/restoreopticaljobprogress.cpp \
    jobs/restoreopticaljob.cpp \
    main.cpp \
    mainwindow.cpp \
    operations/eraseopticalpopover.cpp \
    operations/erasepartitiontablepopover.cpp \
    operations/imagepopover.cpp \
    operations/partitionpopover.cpp \
    operations/restoreopticalpopover.cpp

HEADERS += \
    diskmodel.h \
    diskpane.h \
    jobs/erasecdrwjob.h \
    jobs/progress/erasecdrwjobprogress.h \
    jobs/progress/restoreopticaljobprogress.h \
    jobs/restoreopticaljob.h \
    mainwindow.h \
    operations/eraseopticalpopover.h \
    operations/erasepartitiontablepopover.h \
    operations/imagepopover.h \
    operations/partitionpopover.h \
    operations/restoreopticalpopover.h

FORMS += \
    diskpane.ui \
    jobs/progress/erasecdrwjobprogress.ui \
    jobs/progress/restoreopticaljobprogress.ui \
    mainwindow.ui \
    operations/eraseopticalpopover.ui \
    operations/erasepartitiontablepopover.ui \
    operations/imagepopover.ui \
    operations/partitionpopover.ui \
    operations/restoreopticalpopover.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$OUT_PWD/../libthefrisbee/ -llibthefrisbee

INCLUDEPATH += $$PWD/../libthefrisbee
DEPENDPATH += $$PWD/../libthefrisbee
