#include "blocklvm2interface.h"

#include "../driveobjectmanager.h"
#include <QDBusInterface>

struct BlockLvm2InterfacePrivate {
        QDBusObjectPath logicalVolume;
};

BlockLvm2Interface::BlockLvm2Interface(QDBusObjectPath path, QObject* parent) :
    DiskInterface{path, interfaceName(), parent} {
    d = new BlockLvm2InterfacePrivate();

    bindPropertyUpdater("LogicalVolume", [this](QVariant value) {
        d->logicalVolume = value.value<QDBusObjectPath>();
    });
}

BlockLvm2Interface::~BlockLvm2Interface() {
    delete d;
}

QString BlockLvm2Interface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Block.LVM2");
}

LogicalVolume* BlockLvm2Interface::logicalVolume() {
    return DriveObjectManager::logicalVolumeForPath(d->logicalVolume);
}

DiskInterface::Interfaces BlockLvm2Interface::interfaceType() {
    return BlockLvm2;
}
