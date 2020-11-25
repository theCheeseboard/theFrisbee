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
#include "diskobject.h"

#include <QTimer>
#include <QLocale>
#include <QIcon>

#include "diskinterface.h"
#include "blockinterface.h"
#include "filesysteminterface.h"
#include "partitiontableinterface.h"
#include "partitioninterface.h"
#include "driveinterface.h"
#include "loopinterface.h"

struct DiskObjectPrivate {
    QDBusObjectPath path;

    QSemaphore locker{1};

    QMap<QString, DiskInterface*> interfaces;
};

DiskObject::DiskObject(QDBusObjectPath path, QObject* parent) : QObject(parent) {
    d = new DiskObjectPrivate();
    d->path = path;
}

DiskObject::~DiskObject() {
    delete d;
}

template <> BlockInterface* DiskObject::interface() const {
    return static_cast<BlockInterface*>(d->interfaces.value(BlockInterface::interfaceName(), nullptr));
}

template <> FilesystemInterface* DiskObject::interface() const {
    return static_cast<FilesystemInterface*>(d->interfaces.value(FilesystemInterface::interfaceName(), nullptr));
}

template <> PartitionTableInterface* DiskObject::interface() const {
    return static_cast<PartitionTableInterface*>(d->interfaces.value(PartitionTableInterface::interfaceName(), nullptr));
}

template <> PartitionInterface* DiskObject::interface() const {
    return static_cast<PartitionInterface*>(d->interfaces.value(PartitionInterface::interfaceName(), nullptr));
}

template <> LoopInterface* DiskObject::interface() const {
    return static_cast<LoopInterface*>(d->interfaces.value(LoopInterface::interfaceName(), nullptr));
}

bool DiskObject::isInterfaceAvailable(DiskInterface::Interfaces interface) {
    switch (interface) {
        case DiskInterface::Block:
            return d->interfaces.contains(BlockInterface::interfaceName());
        case DiskInterface::Filesystem:
            return d->interfaces.contains(FilesystemInterface::interfaceName());
        case DiskInterface::PartitionTable:
            return d->interfaces.contains(PartitionTableInterface::interfaceName());
        case DiskInterface::Partition:
            return d->interfaces.contains(PartitionInterface::interfaceName());
        case DiskInterface::Loop:
            return d->interfaces.contains(LoopInterface::interfaceName());
        default:
            return false;
    }
}

QString DiskObject::displayName() {
    PartitionInterface* partition = interface<PartitionInterface>();
    if (partition && !partition->name().isEmpty()) return partition->name();

    BlockInterface* block = interface<BlockInterface>();
    if (block) {
        QString label = block->idLabel();
        if (!label.isEmpty()) return label;

        DriveInterface* drive = block->drive();
        if (drive) return drive->model();
        return block->blockName();
    }

    LoopInterface* loop = interface<LoopInterface>();
    if (loop) {
        return tr("Loop Device");
    }

    return tr("Block Device");
}

QIcon DiskObject::icon() {
    BlockInterface* block = interface<BlockInterface>();

    if (block->drive()) {
        switch (block->drive()->media()) {
            case DriveInterface::Thumb:
                return QIcon::fromTheme("drive-removable-media");
            case DriveInterface::Flash:
            case DriveInterface::CompactFlash:
            case DriveInterface::MemoryStick:
            case DriveInterface::SmartMedia:
            case DriveInterface::Sd:
            case DriveInterface::SdHC:
            case DriveInterface::SdXC:
            case DriveInterface::MMC:
                return QIcon::fromTheme("media-flash-sd-mmc");
            case DriveInterface::Floppy:
            case DriveInterface::Zip:
            case DriveInterface::Jaz:
                return QIcon::fromTheme("media-floppy");
            case DriveInterface::Optical:
            case DriveInterface::MagnetoOptical:
                return QIcon::fromTheme("media-optical");
            case DriveInterface::Cd:
            case DriveInterface::CdR:
            case DriveInterface::CdRw:
                return QIcon::fromTheme("media-optical-audio");
            case DriveInterface::Dvd:
            case DriveInterface::DvdR:
            case DriveInterface::DvdRw:
            case DriveInterface::DvdRam:
            case DriveInterface::DvdPR:
            case DriveInterface::DvdPRw:
            case DriveInterface::DvdPRDl:
            case DriveInterface::DvdPRwDl:
            case DriveInterface::HdDvd:
            case DriveInterface::HdDvdR:
            case DriveInterface::HdDvdRw:
                return QIcon::fromTheme("media-optical-dvd");
            case DriveInterface::Bd:
            case DriveInterface::BdR:
            case DriveInterface::BdRe:
                return QIcon::fromTheme("media-optical-blu-ray");
            case DriveInterface::Unknown:
            default:
                if (block->drive()->isOpticalDrive()) {
                    return QIcon::fromTheme("media-optical");
                } else if (block->drive()->isRemovable()) {
                    return QIcon::fromTheme("drive-removable-media");
                } else {
                    return QIcon::fromTheme("drive-harddisk");
                }
        }
    } else {
        return QIcon::fromTheme("drive-harddisk");
    }
}

void DiskObject::updateInterfaces(QMap<QString, QVariantMap> interfaces) {
    QStringList interfaceNames = {
        BlockInterface::interfaceName(),
        FilesystemInterface::interfaceName(),
        PartitionTableInterface::interfaceName(),
        PartitionInterface::interfaceName(),
        LoopInterface::interfaceName()
    };

    for (QString interface : interfaceNames) {
        if (interfaces.contains(interface)) {
            //Add it to the list of interfaces

            DiskInterface* diskInterface;
            if (d->interfaces.contains(interface)) {
                diskInterface = d->interfaces.value(interface);
            } else {
                diskInterface = makeDiskInterface(interface);
                d->interfaces.insert(interface, diskInterface);
                QTimer::singleShot(0, [ = ] {
                    emit interfaceAdded(diskInterface->interfaceType());
                });
            }

            diskInterface->updateProperties(interfaces.value(interface));
        } else {
            //Remove it from the list of interfaces
            if (d->interfaces.contains(interface)) {
                DiskInterface* diskInterface = d->interfaces.take(interface);
                QTimer::singleShot(0, [ = ] {
                    emit interfaceRemoved(diskInterface->interfaceType());
                });
                diskInterface->deleteLater();
            }
        }
    }
}

DiskInterface* DiskObject::makeDiskInterface(QString interface) {
    if (interface == BlockInterface::interfaceName()) return new BlockInterface(d->path);
    if (interface == FilesystemInterface::interfaceName()) return new FilesystemInterface(d->path);
    if (interface == PartitionTableInterface::interfaceName()) return new PartitionTableInterface(d->path);
    if (interface == PartitionInterface::interfaceName()) return new PartitionInterface(d->path);
    if (interface == LoopInterface::interfaceName()) return new LoopInterface(d->path);
    return nullptr;
}


bool DiskObject::tryLock() {
    bool acquired = d->locker.tryAcquire();
    if (acquired) {
        emit lockedChanged(isLocked());
    }
    return acquired;
}

tPromise<void>* DiskObject::lock() {
    return TPROMISE_CREATE_SAME_THREAD(void, {
        Q_UNUSED(rej);
        TPROMISE_CREATE_NEW_THREAD(void, {
            Q_UNUSED(rej);
            d->locker.acquire();
            res();
        })->then([ = ] {
            emit lockedChanged(isLocked());
            res();
        });
    });
}

void DiskObject::releaseLock() {
    d->locker.release();
    emit lockedChanged(isLocked());
}

bool DiskObject::isLocked() {
    return d->locker.available() == 0;
}

