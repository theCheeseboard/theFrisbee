#include "volumegroup.h"
#include "logicalvolume.h"

#include "driveobjectmanager.h"

struct VolumeGroupPrivate {
        QString name;
};

VolumeGroup::VolumeGroup(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new VolumeGroupPrivate();

    bindPropertyUpdater("Name", [this](QVariant value) {
        d->name = value.toString();
    });

    connect(DriveObjectManager::instance(), &DriveObjectManager::logicalVolumeAdded, this, &VolumeGroup::lvsChanged);
    connect(DriveObjectManager::instance(), &DriveObjectManager::logicalVolumeRemoved, this, &VolumeGroup::lvsChanged);
}

VolumeGroup::~VolumeGroup() {
    delete d;
}

QString VolumeGroup::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.VolumeGroup");
}

tRange<LogicalVolume *> VolumeGroup::lvs()
{
    return tRange(DriveObjectManager::logicalVolumes()).filter([this](LogicalVolume* lv) {
        return lv->vg() == this;
    });
}

QString VolumeGroup::name() {
    return d->name;
}
