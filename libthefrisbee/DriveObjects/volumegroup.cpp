#include "volumegroup.h"
#include "logicalvolume.h"

#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>

#include "DriveObjects/diskobject.h"
#include "DriveObjects/physicalvolumeinterface.h"
#include "driveobjectmanager.h"
#include "frisbeeexception.h"

struct VolumeGroupPrivate {
        QDBusObjectPath path;
        QString name;
};

VolumeGroup::VolumeGroup(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new VolumeGroupPrivate();
    d->path = path;

    bindPropertyUpdater("Name", [this](QVariant value) {
        d->name = value.toString();
    });

    connect(DriveObjectManager::instance(), &DriveObjectManager::logicalVolumeAdded, this, &VolumeGroup::lvsChanged);
    connect(DriveObjectManager::instance(), &DriveObjectManager::logicalVolumeRemoved, this, &VolumeGroup::lvsChanged);
    connect(DriveObjectManager::instance(), &DriveObjectManager::diskAdded, this, &VolumeGroup::pvsChanged);
    connect(DriveObjectManager::instance(), &DriveObjectManager::diskRemoved, this, &VolumeGroup::pvsChanged);
}

VolumeGroup::~VolumeGroup() {
    delete d;
}

QString VolumeGroup::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.VolumeGroup");
}

tRange<LogicalVolume*> VolumeGroup::lvs() {
    return tRange(DriveObjectManager::logicalVolumes()).filter([this](LogicalVolume* lv) {
        return lv->vg() == this;
    });
}

tRange<DiskObject*> VolumeGroup::pvs() {
    return DriveObjectManager::lvmPhysicalVolumeDisks().filter([this](DiskObject* obj) {
        return obj->interface<PhysicalVolumeInterface>()->volumeGroup() == this;
    });
}

QString VolumeGroup::name() {
    return d->name;
}

QCoro::Task<> VolumeGroup::deleteVg(bool wipe, QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
    message.setArguments({wipe, options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}
