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
#include "driveobjectmanager.h"

#include "DriveObjects/diskobject.h"
#include "DriveObjects/partitiontableinterface.h"
#include "DriveObjects/driveinterface.h"
#include "DriveObjects/loopinterface.h"
#include "DriveObjects/filesysteminterface.h"
#include "DriveObjects/blockinterface.h"
#include "DriveObjects/encryptedinterface.h"
#include <QDebug>
#include <QTimer>

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusObjectPath>
#include <QDBusArgument>
#include <QDBusInterface>

struct DriveObjectManagerPrivate {
    QMap<QDBusObjectPath, DiskObject*> objects;
    QMap<QDBusObjectPath, DriveInterface*> drives;
};

DriveObjectManager::DriveObjectManager(QObject* parent) : QObject(parent) {
    d = new DriveObjectManagerPrivate();

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", "org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this, SLOT(updateInterfaces()));
    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", "org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this, SLOT(updateInterfaces()));
    updateInterfaces();
}

DriveObjectManager* DriveObjectManager::instance() {
    static DriveObjectManager* manager = new DriveObjectManager();
    return manager;
}

QList<DiskObject*> DriveObjectManager::rootDisks() {
    QList<DiskObject*> disks = instance()->d->objects.values();
    QSet<DiskObject*> notRootDisks;

    for (DiskObject* disk : disks) {
        PartitionTableInterface* partitionTable = disk->interface<PartitionTableInterface>();
        if (partitionTable) {
            for (DiskObject* partition : partitionTable->partitions()) {
                notRootDisks.insert(partition);
            }
        }

        LoopInterface* loop = disk->interface<LoopInterface>();
        if (loop) {
            if (!loop->isAvailable()) notRootDisks.insert(disk);
        }
    }

    for (DiskObject* disk : notRootDisks) {
        disks.removeOne(disk);
    }

    return disks;
}

QList<DiskObject*> DriveObjectManager::filesystemDisks() {
    QList<DiskObject*> disks = instance()->d->objects.values();
    QSet<DiskObject*> notFilesystemDisks;

    for (DiskObject* disk : disks) {
        BlockInterface* block = disk->interface<BlockInterface>();
        if (block && block->hintIgnore()) {
            notFilesystemDisks.insert(disk);
            continue;
        }

        FilesystemInterface* filesystem = disk->interface<FilesystemInterface>();
        if (!filesystem) notFilesystemDisks.insert(disk);
    }

    for (DiskObject* disk : notFilesystemDisks) {
        disks.removeOne(disk);
    }

    return disks;
}

QList<DiskObject*> DriveObjectManager::encryptedDisks() {
    QList<DiskObject*> disks = instance()->d->objects.values();
    QSet<DiskObject*> notEncryptedDisks;

    for (DiskObject* disk : disks) {
        BlockInterface* block = disk->interface<BlockInterface>();
        if (block && block->hintIgnore()) {
            notEncryptedDisks.insert(disk);
            continue;
        }

        EncryptedInterface* encrypted = disk->interface<EncryptedInterface>();
        if (!encrypted) notEncryptedDisks.insert(disk);
    }

    for (DiskObject* disk : notEncryptedDisks) {
        disks.removeOne(disk);
    }

    return disks;
}

QList<DiskObject*> DriveObjectManager::opticalDisks() {
    QList<DiskObject*> disks = instance()->d->objects.values();
    QSet<DiskObject*> notOpticalDisks;

    for (DiskObject* disk : disks) {
        BlockInterface* block = disk->interface<BlockInterface>();
        if (block && block->hintIgnore()) {
            notOpticalDisks.insert(disk);
            continue;
        }

        if (!block->drive() || !block->drive()->isOpticalDrive()) notOpticalDisks.insert(disk);
    }

    for (DiskObject* disk : notOpticalDisks) {
        disks.removeOne(disk);
    }

    return disks;
}

DiskObject* DriveObjectManager::diskForPath(QDBusObjectPath path) {
    return instance()->d->objects.value(path);
}

DiskObject* DriveObjectManager::diskByBlockName(QString blockName) {
    for (DiskObject* object : instance()->d->objects.values()) {
        if (object->interface<BlockInterface>() && object->interface<BlockInterface>()->blockName() == blockName) return object;
    }
    return nullptr;
}

DriveInterface* DriveObjectManager::driveForPath(QDBusObjectPath path) {
    return instance()->d->drives.value(path);
}

QStringList DriveObjectManager::supportedFilesystems() {
    QDBusInterface managerInterface("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2/Manager", "org.freedesktop.UDisks2.Manager", QDBusConnection::systemBus());
    return managerInterface.property("SupportedFilesystems").toStringList();
}

tPromise<QDBusObjectPath>* DriveObjectManager::loopSetup(QDBusUnixFileDescriptor fd, QVariantMap options) {
    return TPROMISE_CREATE_SAME_THREAD(QDBusObjectPath, {
        QDBusMessage loopSetup = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2/Manager", "org.freedesktop.UDisks2.Manager", "LoopSetup");
        loopSetup.setArguments({QVariant::fromValue(fd), options});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(loopSetup));
        connect(watcher, &QDBusPendingCallWatcher::finished, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res(watcher->reply().arguments().first().value<QDBusObjectPath>());
            }
            watcher->deleteLater();
        });
    });
}

void DriveObjectManager::updateInterfaces() {

    QDBusMessage managedObjects = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
    QMap<QDBusObjectPath, QMap<QString, QVariantMap>> reply;
    QDBusArgument arg = QDBusConnection::systemBus().call(managedObjects).arguments().first().value<QDBusArgument>();

    arg >> reply;

    for (QDBusObjectPath path : reply.keys()) {
        auto interfaces = reply.value(path);

        //Determine which type of object to create
        if (path.path().startsWith("/org/freedesktop/UDisks2/block_devices")) {
            DiskObject* object;
            if (d->objects.contains(path)) {
                object = d->objects.value(path);
            } else {
                object = new DiskObject(path);

                connect(object, &DiskObject::interfaceAdded, this, [ = ](DiskInterface::Interfaces interface) {
                    if (interface == DiskInterface::Loop) {
                        LoopInterface* loop = object->interface<LoopInterface>();
                        connect(loop, &LoopInterface::backingFileChanged, this, &DriveObjectManager::rootDisksChanged);

                        emit rootDisksChanged();
                    } else if (interface == DiskInterface::Filesystem) {
                        emit filesystemDisksChanged();
                    }
                });
                connect(object, &DiskObject::interfaceRemoved, this, [ = ](DiskInterface::Interfaces interface) {
                    if (interface == DiskInterface::Filesystem) {
                        emit filesystemDisksChanged();
                    }
                });

                d->objects.insert(path, object);
                QTimer::singleShot(0, [ = ] {
                    emit diskAdded(object);
                    emit rootDisksChanged();
                    emit filesystemDisksChanged();
                });
            }

            object->updateInterfaces(interfaces);
        } else if (path.path().startsWith("/org/freedesktop/UDisks2/drives")) {
            DriveInterface* object;
            if (d->drives.contains(path)) {
                object = d->drives.value(path);
            } else {
                object = new DriveInterface(path);
                d->drives.insert(path, object);
                QTimer::singleShot(0, [ = ] {
                    emit driveAdded(object);
                });
            }

            object->updateProperties(interfaces.value("org.freedesktop.UDisks2.Drive"));
        }
    }

    QList<QDBusObjectPath> gone;
    for (QDBusObjectPath path : d->objects.keys()) {
        if (!reply.contains(path)) gone.append(path);
    }
    for (QDBusObjectPath path : d->drives.keys()) {
        if (!reply.contains(path)) gone.append(path);
    }

    for (QDBusObjectPath path : gone) {
        if (d->objects.contains(path)) {
            DiskObject* disk = d->objects.take(path);
            QTimer::singleShot(0, [ = ] {
                emit diskRemoved(disk);
                emit rootDisksChanged();
                emit filesystemDisksChanged();
            });
            disk->deleteLater();
        } else {
            DriveInterface* drive = d->drives.take(path);
            QTimer::singleShot(0, [ = ] {
                emit driveRemoved(drive);
            });
            drive->deleteLater();
        }
    }
}
