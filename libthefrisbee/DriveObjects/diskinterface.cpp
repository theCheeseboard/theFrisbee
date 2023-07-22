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
#include "diskinterface.h"
#include "DriveObjects/atadriveinterface.h"
#include "DriveObjects/blockinterface.h"
#include "DriveObjects/blocklvm2interface.h"
#include "DriveObjects/encryptedinterface.h"
#include "DriveObjects/filesysteminterface.h"
#include "DriveObjects/loopinterface.h"
#include "DriveObjects/partitioninterface.h"
#include "DriveObjects/partitiontableinterface.h"
#include "DriveObjects/physicalvolumeinterface.h"

#include <QDBusConnection>

struct DiskInterfacePrivate {
};

DiskInterface::DiskInterface(QDBusObjectPath path, QString interface, QObject* parent) :
    UdisksInterface(path, interface, parent) {
    d = new DiskInterfacePrivate();
}

DiskInterface::~DiskInterface() {
    delete d;
}

DiskInterface* DiskInterface::makeDiskInterface(QString interface, QDBusObjectPath path) {
    if (interface == BlockInterface::interfaceName()) return new BlockInterface(path);
    if (interface == FilesystemInterface::interfaceName()) return new FilesystemInterface(path);
    if (interface == PartitionTableInterface::interfaceName()) return new PartitionTableInterface(path);
    if (interface == PartitionInterface::interfaceName()) return new PartitionInterface(path);
    if (interface == LoopInterface::interfaceName()) return new LoopInterface(path);
    if (interface == EncryptedInterface::interfaceName()) return new EncryptedInterface(path);
    if (interface == AtaDriveInterface::interfaceName()) return new AtaDriveInterface(path);
    if (interface == BlockLvm2Interface::interfaceName()) return new BlockLvm2Interface(path);
    if (interface == PhysicalVolumeInterface::interfaceName()) return new PhysicalVolumeInterface(path);
    return nullptr;
}
