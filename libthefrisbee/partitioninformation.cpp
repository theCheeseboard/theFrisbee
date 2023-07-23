/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "partitioninformation.h"

#include <QMap>
#include <driveobjectmanager.h>

struct PartitionInformationPrivate {
};

PartitionInformation::PartitionInformation(QObject* parent) :
    QObject(parent) {
}

QStringList PartitionInformation::availableFormatTypes() {
    QStringList supportedFilesystems = DriveObjectManager::supportedFilesystems();
    QStringList available;
    if (supportedFilesystems.contains("ext4")) available.append("ext4");
    if (supportedFilesystems.contains("ntfs")) available.append("ntfs");
    if (supportedFilesystems.contains("exfat")) available.append("exfat");
    if (supportedFilesystems.contains("swap")) available.append("swap");
    available.append("lvmpv");
    if (supportedFilesystems.contains("xfs")) available.append("xfs");
    if (supportedFilesystems.contains("btrfs")) available.append("btrfs");
    if (supportedFilesystems.contains("f2fs")) available.append("f2fs");
    if (supportedFilesystems.contains("vfat")) {
        available.append("vfat");
        available.append("efi");
        available.append("xbl");
    }
    return available;
}

QString PartitionInformation::typeName(QString type) {
    QMap<QString, QString> mapping = {
        {"ext4",  tr("ext4 (Linux Filesystem)")          },
        {"ntfs",  tr("NTFS (Windows Filesystem)")        },
        {"exfat", tr("exFAT (All Systems)")              },
        {"swap",  tr("Linux Swap")                       },
        {"xfs",   tr("XFS (Linux Filesystem)")           },
        {"btrfs", tr("BTRFS (Linux Filesystem)")         },
        {"f2fs",  tr("F2FS (Flash Optimised Filesystem)")},
        {"vfat",  tr("FAT (All Systems)")                },
        {"efi",   tr("EFI System Partition")             },
        {"xbl",   tr("Extended Boot Partition")          },
        {"lvmpv", tr("LVM Physical Volume")              }
    };

    return mapping.value(type);
}

QString PartitionInformation::formatType(QString type) {
    QMap<QString, QString> mapping = {
        {"efi",   "vfat" },
        {"xbl",   "vfat" },
        {"lvmpv", "empty"}
    };

    return mapping.value(type, type);
}

QString PartitionInformation::partitionType(QString type, QString table) {
    if (table == "gpt") {
        QMap<QString, QString> mapping = {
            {"ext4",  "0fc63daf-8483-4772-8e79-3d69d8477de4"},
            {"ntfs",  "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7"},
            {"exfat", "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7"},
            {"swap",  "0657fd6d-a4ab-43c4-84e5-0933c84b4f4f"},
            {"xfs",   "0fc63daf-8483-4772-8e79-3d69d8477de4"},
            {"btrfs", "0fc63daf-8483-4772-8e79-3d69d8477de4"},
            {"f2fs",  "0fc63daf-8483-4772-8e79-3d69d8477de4"},
            {"vfat",  "ebd0a0a2-b9e5-4433-87c0-68b6b72699c7"},
            {"efi",   "c12a7328-f81f-11d2-ba4b-00a0c93ec93b"},
            {"xbl",   "bc13c2ff-59e6-4262-a352-b275fd6f7172"},
            {"lvmpv", "e6d6d379-f507-44c2-a23c-238f2a3df928"}
        };
        return mapping.value(type);
    } else if (table == "dos") {
        // TODO: DOS partition table type
        QMap<QString, QString> mapping = {
            {"ext4",  "0x83"},
            {"ntfs",  "0x07"},
            {"exfat", "0x07"},
            {"swap",  "0x82"},
            {"xfs",   "0x83"},
            {"btrfs", "0x83"},
            {"f2fs",  "0x83"},
            {"vfat",  "0x0b"},
            {"efi",   "0xef"},
            {"xbl",   "0xea"},
            {"lvmpv", "0x8e"}
        };
        return mapping.value(type);
    }
    return "";
}
