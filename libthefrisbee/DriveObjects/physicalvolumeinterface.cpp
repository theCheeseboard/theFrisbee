#include "physicalvolumeinterface.h"

#include <driveobjectmanager.h>

struct PhysicalVolumeInterfacePrivate {
        QDBusObjectPath volumeGroup;
};

PhysicalVolumeInterface::PhysicalVolumeInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface{path, interfaceName(), parent} {
    d = new PhysicalVolumeInterfacePrivate();

    bindPropertyUpdater("VolumeGroup", [this](QVariant value) {
        d->volumeGroup = value.value<QDBusObjectPath>();
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

DiskInterface::Interfaces PhysicalVolumeInterface::interfaceType() {
    return PhysicalVolume;
}
