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
#ifndef DRIVEOBJECT_H
#define DRIVEOBJECT_H

#include <QObject>
#include <QDBusObjectPath>
#include <tpromise.h>
#include "diskinterface.h"

class DriveObjectManager;
struct DiskObjectPrivate;
class DiskObject : public QObject {
        Q_OBJECT
    public:
        explicit DiskObject(QDBusObjectPath path, QObject* parent = nullptr);
        ~DiskObject();

        template <typename T> T* interface() const;
        bool isInterfaceAvailable(DiskInterface::Interfaces interface);

        QString displayName();
        QIcon icon();

        bool tryLock();
        tPromise<void>* lock();
        void releaseLock();
        bool isLocked();

        QDBusObjectPath path();

    signals:
        void interfaceAdded(DiskInterface::Interfaces interface);
        void interfaceRemoved(DiskInterface::Interfaces interface);
        void lockedChanged(bool locked);

    protected:
        friend DriveObjectManager;
        void updateInterfaces(QMap<QString, QVariantMap> interfaces);

    private:
        DiskObjectPrivate* d;

        DiskInterface* makeDiskInterface(QString interface);
};

#endif // DRIVEOBJECT_H
