#include "volumegroup.h"

struct VolumeGroupPrivate {
};

VolumeGroup::VolumeGroup(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new VolumeGroupPrivate();
}

VolumeGroup::~VolumeGroup() {
    delete d;
}

QString VolumeGroup::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.VolumeGroup");
}
