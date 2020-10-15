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

struct PartitionInterfacePrivate {
    QString name;
    uint number;
    quint64 size;
    quint64 offset;
    QDBusObjectPath parentTable;
};

PartitionInterface::PartitionInterface(QDBusObjectPath path, QObject* parent) : DiskInterface(path, interfaceName(),  parent) {
    d = new PartitionInterfacePrivate();

    bindPropertyUpdater("Name", [ = ](QVariant value) {
        d->name = value.toString();
    });
    bindPropertyUpdater("Number", [ = ](QVariant value) {
        d->number = value.toUInt();
    });
    bindPropertyUpdater("Table", [ = ](QVariant value) {
        d->parentTable = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("Size", [ = ](QVariant value) {
        d->size = value.toUInt();
    });
    bindPropertyUpdater("Offset", [ = ](QVariant value) {
        d->offset = value.toUInt();
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

DiskObject* PartitionInterface::parentTable() {
    return DriveObjectManager::diskForPath(d->parentTable);
}
