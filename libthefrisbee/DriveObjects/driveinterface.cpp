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
#include "driveinterface.h"

#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>

struct DriveInterfacePrivate {
        QDBusObjectPath path;
        QVariantMap properties;
};

DriveInterface::DriveInterface(QDBusObjectPath path, QObject* parent) :
    QObject(parent) {
    d = new DriveInterfacePrivate();
    d->path = path;

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", path.path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

DriveInterface::~DriveInterface() {
    delete d;
}

QString DriveInterface::vendor() {
    return d->properties.value("Vendor").toString();
}

QString DriveInterface::model() {
    return d->properties.value("Model").toString();
}

bool DriveInterface::optical() {
    return d->properties.value("Optical").toBool();
}

bool DriveInterface::opticalBlank() {
    return d->properties.value("OpticalBlank").toBool();
}

bool DriveInterface::isOpticalDrive() {
    for (MediaFormat format : mediaCompatibility()) {
        if (format > OpticalFormatsBegin && format < OpticalFormatsEnd) return true;
    }
    return false;
}

DriveInterface::MediaFormat DriveInterface::media() {
    return getMediaFormat(d->properties.value("Media").toString());
}

QList<DriveInterface::MediaFormat> DriveInterface::mediaCompatibility() {
    QStringList compatibility = d->properties.value("MediaCompatibility").toStringList();
    QList<MediaFormat> formats;
    for (QString item : compatibility) {
        formats.append(getMediaFormat(item));
    }
    return formats;
}

bool DriveInterface::mediaAvailable() {
    return d->properties.value("MediaAvailable").toBool();
}

bool DriveInterface::ejectable() {
    return d->properties.value("Ejectable").toBool();
}

QCoro::Task<> DriveInterface::eject() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), "org.freedesktop.UDisks2.Drive", "Eject");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

bool DriveInterface::isRemovable() {
    return d->properties.value("Removable").toBool();
}

void DriveInterface::updateProperties(QVariantMap properties) {
    for (QString property : properties.keys()) {
        d->properties.insert(property, properties.value(property));
    }
}

void DriveInterface::propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties) {
    updateProperties(changedProperties);
    emit changed();
}

DriveInterface::MediaFormat DriveInterface::getMediaFormat(QString format) {
    QMap<QString, MediaFormat> formats = {
        {"thumb",                  Thumb         },
        {"flash",                  Flash         },
        {"flash_cf",               CompactFlash  },
        {"flash_ms",               MemoryStick   },
        {"flash_sm",               SmartMedia    },
        {"flash_sd",               Sd            },
        {"flash_sdhc",             SdHC          },
        {"flash_sdxc",             SdXC          },
        {"flash_mmc",              MMC           },
        {"floppy",                 Floppy        },
        {"floppy_zip",             Zip           },
        {"floppy_jaz",             Jaz           },
        {"optical",                Optical       },
        {"optical_cd",             Cd            },
        {"optical_cd_r",           CdR           },
        {"optical_cd_rw",          CdRw          },
        {"optical_dvd",            Dvd           },
        {"optical_dvd_r",          DvdR          },
        {"optical_dvd_rw",         DvdRw         },
        {"optical_dvd_ram",        DvdRam        },
        {"optical_dvd_plus_r",     DvdPR         },
        {"optical_dvd_plus_rw",    DvdPRw        },
        {"optical_dvd_plus_r_dl",  DvdPRDl       },
        {"optical_dvd_plus_rw_dl", DvdPRwDl      },
        {"optical_bd",             Bd            },
        {"optical_bd_r",           BdR           },
        {"optical_bd_re",          BdRe          },
        {"optical_hddvd",          HdDvd         },
        {"optical_hddvd_r",        HdDvdR        },
        {"optical_hddvd_rw",       HdDvdRw       },
        {"optical_mo",             MagnetoOptical}
    };

    return formats.value(format, Unknown);
}
