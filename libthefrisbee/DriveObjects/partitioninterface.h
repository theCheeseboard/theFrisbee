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
#ifndef PARTITIONINTERFACE_H
#define PARTITIONINTERFACE_H

#include "diskinterface.h"

struct PartitionInterfacePrivate;
class PartitionInterface : public DiskInterface {
        Q_OBJECT
    public:
        explicit PartitionInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~PartitionInterface();

        static QString interfaceName();
        Interfaces interfaceType();

        QString name();
        uint number();
        quint64 size();
        quint64 offset();
        DiskObject* parentTable();

    signals:

    private:
        PartitionInterfacePrivate* d;
};

#endif // PARTITIONINTERFACE_H
