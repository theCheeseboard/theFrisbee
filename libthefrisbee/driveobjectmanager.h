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
#ifndef DRIVEOBJECTMANAGER_H
#define DRIVEOBJECTMANAGER_H

#include <QObject>
#include <QDBusObjectPath>
#include <tpromise.h>
#include <QDBusUnixFileDescriptor>

struct DriveObjectManagerPrivate;
class DiskObject;
class DriveInterface;
class DriveObjectManager : public QObject {
        Q_OBJECT
    public:
        explicit DriveObjectManager(QObject* parent = nullptr);

        static DriveObjectManager* instance();
        static QList<DiskObject*> rootDisks();
        static QList<DiskObject*> filesystemDisks();

        static DiskObject* diskForPath(QDBusObjectPath path);
        static DiskObject* diskByBlockName(QString blockName);
        static DriveInterface* driveForPath(QDBusObjectPath path);

        static QStringList supportedFilesystems();

        static tPromise<QDBusObjectPath>* loopSetup(QDBusUnixFileDescriptor fd, QVariantMap options);

    public slots:
        void updateInterfaces();

    signals:
        void diskAdded(DiskObject* disk);
        void diskRemoved(DiskObject* disk);
        void driveAdded(DriveInterface* drive);
        void driveRemoved(DriveInterface* drive);
        void rootDisksChanged();
        void filesystemDisksChanged();

    private:
        DriveObjectManagerPrivate* d;
};

#endif // DRIVEOBJECTMANAGER_H
