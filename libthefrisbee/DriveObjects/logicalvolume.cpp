#include "logicalvolume.h"

#include "driveobjectmanager.h"

struct LogicalVolumePrivate {
        QDBusObjectPath vg;
        QDBusObjectPath block;
        QString name;
};

LogicalVolume::LogicalVolume(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new LogicalVolumePrivate();

    bindPropertyUpdater("Name", [this](QVariant value) {
        d->name = value.toString();
    });
    bindPropertyUpdater("VolumeGroup", [this](QVariant value) {
        d->vg = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("BlockDevice", [this](QVariant value) {
        d->block = value.value<QDBusObjectPath>();
    });
}

LogicalVolume::~LogicalVolume() {
    delete d;
}

QString LogicalVolume::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.LogicalVolume");
}

QString LogicalVolume::name() {
    return d->name;
}

VolumeGroup* LogicalVolume::vg() {
    return DriveObjectManager::volumeGroupForPath(d->vg);
}

DiskObject* LogicalVolume::block() {
    return DriveObjectManager::diskForPath(d->block);
}
