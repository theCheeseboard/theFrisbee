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
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

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

    bindPropertyUpdater("Name", [=](QVariant value) {
        d->name = value.toString();
    });
    bindPropertyUpdater("Number", [=](QVariant value) {
        d->number = value.toUInt();
    });
    bindPropertyUpdater("Table", [=](QVariant value) {
        d->parentTable = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("Size", [=](QVariant value) {
        d->size = value.toULongLong();
    });
    bindPropertyUpdater("Offset", [=](QVariant value) {
        d->offset = value.toULongLong();
    });
    bindPropertyUpdater("Type", [=](QVariant value) {
        d->type = value.toString();
    });
    bindPropertyUpdater("UUID", [=](QVariant value) {
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

tPromise<void>* PartitionInterface::setType(QString type) {
    return tPromise<void>::runOnSameThread([=](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SetType");
        message.setArguments({type, QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>* PartitionInterface::setName(QString name) {
    return tPromise<void>::runOnSameThread([=](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "SetName");
        message.setArguments({name, QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>* PartitionInterface::resize(quint64 size) {
    return tPromise<void>::runOnSameThread([=](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Resize");
        message.setArguments({size, QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message, 86400000));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>* PartitionInterface::deletePartition(QVariantMap options) {
    return tPromise<void>::runOnSameThread([=](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
        message.setArguments({options});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}
