/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "partitioninterface.h"

#include "driveobjectmanager.h"
#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>

struct PartitionInterfacePrivate {
        QDBusObjectPath path;
        QString name;
        uint number;
        quint64 size;
        quint64 offset;
        QString type;
        QString uuid;
        QDBusObjectPath parentTable;
};

PartitionInterface::PartitionInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new PartitionInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("Name", [this](QVariant value) {
        d->name = value.toString();
    });
    bindPropertyUpdater("Number", [this](QVariant value) {
        d->number = value.toUInt();
    });
    bindPropertyUpdater("Table", [this](QVariant value) {
        d->parentTable = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("Size", [this](QVariant value) {
        d->size = value.toULongLong();
    });
    bindPropertyUpdater("Offset", [this](QVariant value) {
        d->offset = value.toULongLong();
    });
    bindPropertyUpdater("Type", [this](QVariant value) {
        d->type = value.toString();
    });
    bindPropertyUpdater("UUID", [this](QVariant value) {
        d->uuid = value.toString();
    });
}

PartitionInterface::~PartitionInterface() {
    delete d;
}

QString PartitionInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Partition");
}

DiskInterface::Interfaces PartitionInterface::interfaceType() {
    return DiskInterface::Partition;
}

QString PartitionInterface::name() {
    return d->name;
}

uint PartitionInterface::number() {
    return d->number;
}

quint64 PartitionInterface::size() {
    return d->size;
}

quint64 PartitionInterface::offset() {
    return d->offset;
}

QString PartitionInterface::type() {
    return d->type;
}

QString PartitionInterface::uuid() {
    return d->uuid;
}

DiskObject* PartitionInterface::parentTable() {
    return DriveObjectManager::diskForPath(d->parentTable);
}

QCoro::Task<> PartitionInterface::setType(QString type) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SetType");
    message.setArguments({type, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<> PartitionInterface::setName(QString name) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SetName");
    message.setArguments({name, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<> PartitionInterface::resize(quint64 size) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Resize");
    message.setArguments({size, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<> PartitionInterface::deletePartition(QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
    message.setArguments({options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}
