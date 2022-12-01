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
#ifndef DRIVEINTERFACE_H
#define DRIVEINTERFACE_H

#include <QDBusObjectPath>
#include <QObject>
#include "diskinterface.h"
#include <QCoroTask>

#include <tpromise.h>

struct DriveInterfacePrivate;
class DriveObjectManager;
class AtaDriveInterface;
class DriveInterface : public QObject {
        Q_OBJECT
    public:
        explicit DriveInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~DriveInterface();

        enum MediaFormat {
            Thumb,
            Flash,
            CompactFlash,
            MemoryStick,
            SmartMedia,
            Sd,
            SdHC,
            SdXC,
            MMC,
            Floppy,
            Zip,
            Jaz,

            OpticalFormatsBegin,
            Optical,
            Cd,
            CdR,
            CdRw,
            Dvd,
            DvdR,
            DvdRw,
            DvdRam,
            DvdPR,
            DvdPRw,
            DvdPRDl,
            DvdPRwDl,
            Bd,
            BdR,
            BdRe,
            HdDvd,
            HdDvdR,
            HdDvdRw,
            MagnetoOptical,
            OpticalFormatsEnd,

            Unknown
        };

        QString vendor();
        QString model();

        bool optical();
        bool opticalBlank();
        bool isOpticalDrive();

        MediaFormat media();
        QList<MediaFormat> mediaCompatibility();
        bool mediaAvailable();

        bool ejectable();
        QCoro::Task<> eject();

        bool isRemovable();

        template<typename T> T* interface() const;
        bool isInterfaceAvailable(DiskInterface::Interfaces interface);

    signals:
        void interfaceAdded(DiskInterface::Interfaces interface);
        void interfaceRemoved(DiskInterface::Interfaces interface);
        void changed();

    protected:
        friend DriveObjectManager;
        void updateProperties(QVariantMap properties);
        void updateInterfaces(QMap<QString, QVariantMap> interfaces);

    private slots:
        void propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties);

    private:
        DriveInterfacePrivate* d;

        MediaFormat getMediaFormat(QString format);
};

#endif // DRIVEINTERFACE_H
