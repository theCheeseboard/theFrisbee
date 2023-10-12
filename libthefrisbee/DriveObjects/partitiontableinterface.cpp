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
#include "partitiontableinterface.h"

#include "diskobject.h"
#include "driveobjectmanager.h"
#include "partitioninterface.h"
#include <QCoroDBusPendingCall>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>
#include <ranges/trange.h>

struct PartitionTableInterfacePrivate {
        QDBusObjectPath path;

        QList<QDBusObjectPath> partitions;
        QString type;

        bool partitionObjectUpdateRequired = false;
        QList<QPointer<DiskObject>> partitionObjects;
};

PartitionTableInterface::PartitionTableInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new PartitionTableInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("Partitions", [this](QVariant value) {
        QDBusArgument arg = value.value<QDBusArgument>();
        d->partitions.clear();

        arg >> d->partitions;
        d->partitionObjectUpdateRequired = true;
    });
    bindPropertyUpdater("Type", [this](QVariant value) {
        d->type = value.toString();
    });
}

PartitionTableInterface::~PartitionTableInterface() {
    delete d;
}

QString PartitionTableInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.PartitionTable");
}

DiskInterface::Interfaces PartitionTableInterface::interfaceType() {
    return DiskInterface::PartitionTable;
}

QList<DiskObject*> PartitionTableInterface::partitions() {
    updatePartitions();
    return tRange(d->partitionObjects).filter([](QPointer<DiskObject> object) {
                                          return !object.isNull();
                                      })
        .map<DiskObject*>([](QPointer<DiskObject> object) {
            return object.data();
        })
        .toList();
}

QString PartitionTableInterface::type() {
    return d->type;
}

QCoro::Task<QDBusObjectPath> PartitionTableInterface::createPartition(quint64 offset, quint64 size, QString type, QString name, QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "CreatePartition");
    message.setArguments({offset, size, type, name, options});

    auto call = QDBusConnection::systemBus().asyncCall(message, 300000);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
    co_return reply.arguments().first().value<QDBusObjectPath>();
}

QCoro::Task<QDBusObjectPath> PartitionTableInterface::createPartitionAndFormat(quint64 offset, quint64 size, QString type, QString name, QVariantMap options, QString formatType, QVariantMap formatOptions) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "CreatePartitionAndFormat");
    message.setArguments({offset, size, type, name, options, formatType, formatOptions});

    auto call = QDBusConnection::systemBus().asyncCall(message, 300000);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
    co_return reply.arguments().first().value<QDBusObjectPath>();
}

void PartitionTableInterface::updatePartitions() {
    if (!d->partitionObjectUpdateRequired) {
        return;
    }

    QList<QPointer<DiskObject>> diskObjects;
    for (const QDBusObjectPath& path : d->partitions) {
        QPointer<DiskObject> disk = DriveObjectManager::diskForPath(path);
        if (disk) diskObjects.append(disk);
    }

    std::sort(diskObjects.begin(), diskObjects.end(), [=](QPointer<DiskObject> first, QPointer<DiskObject> second) {
        PartitionInterface* partitionFirst = first->interface<PartitionInterface>();
        PartitionInterface* partitionSecond = second->interface<PartitionInterface>();
        if (partitionFirst && partitionSecond) {
            return partitionFirst->number() < partitionSecond->number();
        } else {
            // ???
            return false;
        }
    });
    d->partitionObjects = diskObjects;
    d->partitionObjectUpdateRequired = false;
}
