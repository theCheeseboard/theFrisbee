#include "volumegroup.h"

struct VolumeGroupPrivate {
        QString name;
};

VolumeGroup::VolumeGroup(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new VolumeGroupPrivate();

    bindPropertyUpdater("Name", [this](QVariant value) {
        d->name = value.toString();
    });
}

VolumeGroup::~VolumeGroup() {
    delete d;
}

QString VolumeGroup::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.VolumeGroup");
}

QString VolumeGroup::name() {
    return d->name;
}
