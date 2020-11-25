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

#include <QDBusArgument>
#include "driveobjectmanager.h"
#include "diskobject.h"
#include "partitioninterface.h"

struct PartitionTableInterfacePrivate {
    QDBusObjectPath path;

    QList<QDBusObjectPath> partitions;
};

PartitionTableInterface::PartitionTableInterface(QDBusObjectPath path, QObject* parent) : DiskInterface(path, interfaceName(), parent) {
    d = new PartitionTableInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("Partitions", [ = ](QVariant value) {
        QDBusArgument arg = value.value<QDBusArgument>();
        d->partitions.clear();

        arg >> d->partitions;
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
    QList<DiskObject*> diskObjects;
    for (QDBusObjectPath path : d->partitions) {
        diskObjects.append(DriveObjectManager::diskForPath(path));
    }

    std::sort(diskObjects.begin(), diskObjects.end(), [ = ](DiskObject * first, DiskObject * second) {
        PartitionInterface* partitionFirst = first->interface<PartitionInterface>();
        PartitionInterface* partitionSecond = second->interface<PartitionInterface>();
        if (partitionFirst && partitionSecond) {
            return partitionFirst->number() < partitionSecond->number();
        } else {
            // ???
            return false;
        }
    });

    return diskObjects;
}

tPromise<QDBusObjectPath>* PartitionTableInterface::createPartition(quint64 offset, quint64 size, QString type, QString name, QVariantMap options) {
    return TPROMISE_CREATE_SAME_THREAD(QDBusObjectPath, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "CreatePartition");
        message.setArguments({offset, size, type, name, options});

        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res(watcher->reply().arguments().first().value<QDBusObjectPath>());
            }
        });
    });
}

tPromise<QDBusObjectPath>* PartitionTableInterface::createPartitionAndFormat(quint64 offset, quint64 size, QString type, QString name, QVariantMap options, QString formatType, QVariantMap formatOptions) {
    return TPROMISE_CREATE_SAME_THREAD(QDBusObjectPath, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "CreatePartitionAndFormat");
        message.setArguments({offset, size, type, name, options, formatType, formatOptions});

        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res(watcher->reply().arguments().first().value<QDBusObjectPath>());
            }
        });
    });
}
