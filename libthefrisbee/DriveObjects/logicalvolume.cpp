#include "logicalvolume.h"

struct LogicalVolumePrivate {
};

LogicalVolume::LogicalVolume(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new LogicalVolumePrivate();
}

LogicalVolume::~LogicalVolume() {
    delete d;
}

QString LogicalVolume::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.LogicalVolume");
}
