#include "logicalvolume.h"

#include "driveobjectmanager.h"
#include "frisbeeexception.h"
#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>

struct LogicalVolumePrivate {
        QDBusObjectPath path;

        QDBusObjectPath vg;
        QDBusObjectPath block;
        QString name;
};

LogicalVolume::LogicalVolume(QDBusObjectPath path, QObject* parent) :
    UdisksInterface{path, interfaceName(), parent} {
    d = new LogicalVolumePrivate();
    d->path = path;

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

QCoro::Task<> LogicalVolume::deleteLogicalVolume(QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
    message.setArguments({options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}
