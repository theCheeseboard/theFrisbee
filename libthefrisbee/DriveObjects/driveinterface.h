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

#include <QObject>
#include <QDBusObjectPath>

#include <tpromise.h>

struct DriveInterfacePrivate;
class DriveObjectManager;
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
            MagnetoOptical
        };

        QString vendor();
        QString model();

        bool optical();
        bool opticalBlank();

        MediaFormat media();
        QList<MediaFormat> mediaCompatibility();
        bool mediaAvailable();

        tPromise<void>* eject();

    signals:

    protected:
        friend DriveObjectManager;
        void updateProperties(QVariantMap properties);

    private slots:
        void propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties);

    private:
        DriveInterfacePrivate* d;

        MediaFormat getMediaFormat(QString format);
};

#endif // DRIVEINTERFACE_H
