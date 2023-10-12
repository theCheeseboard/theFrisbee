#include "physicalvolumeinterface.h"

#include <driveobjectmanager.h>

struct PhysicalVolumeInterfacePrivate {
        QDBusObjectPath path;

        QDBusObjectPath volumeGroup;
        quint64 size;
        quint64 freeSize;
};

PhysicalVolumeInterface::PhysicalVolumeInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface{path, interfaceName(), parent} {
    d = new PhysicalVolumeInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("VolumeGroup", [this](QVariant value) {
        d->volumeGroup = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("Size", [this](QVariant value) {
        d->size = value.toULongLong();
    });
    bindPropertyUpdater("FreeSize", [this](QVariant value) {
        d->freeSize = value.toULongLong();
    });
}

PhysicalVolumeInterface::~PhysicalVolumeInterface() {
    delete d;
}

QString PhysicalVolumeInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.PhysicalVolume");
}

VolumeGroup* PhysicalVolumeInterface::volumeGroup() {
    return DriveObjectManager::volumeGroupForPath(d->volumeGroup);
}

quint64 PhysicalVolumeInterface::size() {
    return d->size;
}

quint64 PhysicalVolumeInterface::freeSize() {
    return d->freeSize;
}

DiskInterface::Interfaces PhysicalVolumeInterface::interfaceType() {
    return PhysicalVolume;
}
