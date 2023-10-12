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
#ifndef DISKINTERFACE_H
#define DISKINTERFACE_H

#include "udisksinterface.h"
#include <QDBusObjectPath>

struct DiskInterfacePrivate;
class DiskInterface : public UdisksInterface {
        Q_OBJECT
    public:
        enum Interfaces {
            Block,
            Filesystem,
            PartitionTable,
            Partition,
            Loop,
            Encrypted,
            AtaDrive,
            BlockLvm2,
            PhysicalVolume
        };

        explicit DiskInterface(QDBusObjectPath path, QString interface, QObject* parent = nullptr);
        ~DiskInterface();

        virtual Interfaces interfaceType() = 0;

        static DiskInterface* makeDiskInterface(QString interface, QDBusObjectPath path);

    private:
        DiskInterfacePrivate* d;
};

#endif // DISKINTERFACE_H
