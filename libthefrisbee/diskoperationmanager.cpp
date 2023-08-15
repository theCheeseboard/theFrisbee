/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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

#include "diskoperationmanager.h"

#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>
#include <DriveObjects/filesysteminterface.h>
#include <DriveObjects/partitioninterface.h>
#include <DriveObjects/partitiontableinterface.h>
#include <DriveObjects/physicalvolumeinterface.h>

#include <QMessageBox>
#include <tapplication.h>
#include <tpopover.h>
#include <ttoast.h>

#include <operations/attachpvpopover.h>
#include <operations/eraseopticalpopover.h>
#include <operations/erasepartitionpopover.h>
#include <operations/erasepartitiontablepopover.h>
#include <operations/imagepopover.h>
#include <operations/partitionpopover.h>
#include <operations/restoreopticalpopover.h>

struct OperationManagerPrivate {
        static QMap<DiskOperationManager::DiskOperation, QString> operations;
        static QMap<DiskOperationManager::DiskOperation, QString> operationDescriptions;
};

QMap<DiskOperationManager::DiskOperation, QString> OperationManagerPrivate::operations = {
    {DiskOperationManager::Erase,     "erase"    },
    {DiskOperationManager::Image,     "image"    },
    {DiskOperationManager::Restore,   "restore"  },
    {DiskOperationManager::Partition, "partition"},
    {DiskOperationManager::AttachPv,  "attachPv" }
};

QMap<DiskOperationManager::DiskOperation, QString> OperationManagerPrivate::operationDescriptions = {
    {DiskOperationManager::Erase,     DiskOperationManager::tr("Erase a block device")                           },
    {DiskOperationManager::Image,     DiskOperationManager::tr("Create an image of a block device")              },
    {DiskOperationManager::Restore,   DiskOperationManager::tr("Restore an image back to a block device or disc")},
    {DiskOperationManager::Partition, DiskOperationManager::tr("Edit partitions on a filesystem")                },
    {DiskOperationManager::AttachPv,  DiskOperationManager::tr("Attach a Physical Volume to a Volume Group")     }
};

DiskOperationManager::DiskOperation DiskOperationManager::operationForString(QString operationString) {
    return OperationManagerPrivate::operations.key(operationString);
}

QString DiskOperationManager::operationForOperation(DiskOperation operation) {
    return OperationManagerPrivate::operations.value(operation);
}

QString DiskOperationManager::descriptionForOperation(DiskOperation operation) {
    return OperationManagerPrivate::operationDescriptions.value(operation);
}

bool DiskOperationManager::isValidOperation(QString operationString) {
    return OperationManagerPrivate::operations.values().contains(operationString);
}

void DiskOperationManager::showDiskOperationUi(QWidget* parent, DiskOperation operation, DiskObject* disk) {
    switch (operation) {
        case DiskOperationManager::Erase:
            showEraseOperationUi(parent, disk);
            break;
        case DiskOperationManager::Restore:
            showRestoreOperationUi(parent, disk);
            break;
        case DiskOperationManager::Image:
            showImageOperationUi(parent, disk);
            break;
        case DiskOperationManager::Partition:
            showPartitionOperationUi(parent, disk);
            break;
        case DiskOperationManager::AttachPv:
            showAttachPvOperationUi(parent, disk);
    }
}

void DiskOperationManager::showEraseOperationUi(QWidget* parent, DiskObject* disk) {
    // Determine which type of erasure is the most appropriate for this disk

    DriveInterface* drive = disk->interface<BlockInterface>()->drive();
    if (drive) {
        if (!drive->mediaAvailable()) {
            QMessageBox::warning(parent, tr("No Media"), tr("There is no media in the drive to erase."));
            return;
        }
        if (drive->optical()) {
            if (!ensureOpticalUtilitiesInstalled(parent)) return;

            if (drive->opticalBlank()) {
                if (QMessageBox::warning(parent, tr("Disc already blank"), tr("The disc in the drive is already blank. Do you still want to erase it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
            }

            QList<DriveInterface::MediaFormat> rewritables = {
                DriveInterface::CdRw,
                DriveInterface::DvdRw,
                DriveInterface::DvdPRw,
                DriveInterface::DvdPRwDl};

            if (rewritables.contains(drive->media())) {
                EraseOpticalPopover* jp = new EraseOpticalPopover(disk);
                tPopover* popover = new tPopover(jp);
                popover->setPopoverWidth(-200);
                popover->setPopoverSide(tPopover::Bottom);
                connect(jp, &EraseOpticalPopover::done, popover, &tPopover::dismiss);
                connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
                connect(popover, &tPopover::dismissed, jp, &EraseOpticalPopover::deleteLater);
                popover->show(parent->window());
            } else {
                QMessageBox* box = new QMessageBox();
                box->setParent(parent);
                box->setWindowModality(Qt::WindowModal);
                box->setWindowTitle(tr("Disc not rewritable"));
                box->setText(tr("The disc in the drive is not rewritable."));
                box->setInformativeText(tr("Only rewritable discs can be erased. If you need to destroy the data on this disc, you should physically break it in half."));
                box->setIcon(QMessageBox::Warning);
                connect(box, &QMessageBox::finished, parent, [=] {
                    box->deleteLater();
                });
                box->open();
            }
            return;
        }
    }

    // Ensure this is not the root disk
    bool warnRoot = false;
    if (disk->isInterfaceAvailable(DiskInterface::Filesystem)) {
        for (const QByteArray& mountPoint : disk->interface<FilesystemInterface>()->mountPoints()) {
            if (mountPoint == "/") warnRoot = true;
        }
    } else if (disk->isInterfaceAvailable(DiskInterface::PartitionTable)) {
        for (DiskObject* partition : disk->interface<PartitionTableInterface>()->partitions()) {
            if (partition->isInterfaceAvailable(DiskInterface::Filesystem)) {
                for (const QByteArray& mountPoint : partition->interface<FilesystemInterface>()->mountPoints()) {
                    if (mountPoint == "/") warnRoot = true;
                }
            }
        }
    }

    if (warnRoot) {
        if (QMessageBox::warning(parent, tr("System Disk"), tr("This is a system disk. Erasing it may cause your device to stop working altogether. Do you still want to erase it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
    }

    // Check for LVM
    if (disk->isInterfaceAvailable(DiskInterface::PhysicalVolume)) {
        if (auto vg = disk->interface<PhysicalVolumeInterface>()->volumeGroup()) {
            if (QMessageBox::warning(parent, tr("LVM"), tr("This is a physical volume attached to an activated volume group. Erasing it may cause one or more logical volumes on the volume group to become unavailable permanently. Do you still want to erase it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
        }
    }

    if (disk->isInterfaceAvailable(DiskInterface::Partition) || disk->isInterfaceAvailable(DiskInterface::BlockLvm2)) {
        ErasePartitionPopover* jp = new ErasePartitionPopover(disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(-200);
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &ErasePartitionPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &ErasePartitionTablePopover::deleteLater);
        popover->show(parent->window());
    } else {
        ErasePartitionTablePopover* jp = new ErasePartitionTablePopover(disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(-200);
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &ErasePartitionTablePopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &ErasePartitionTablePopover::deleteLater);
        popover->show(parent->window());
    }
}

void DiskOperationManager::showRestoreOperationUi(QWidget* parent, DiskObject* disk) {
    // Determine which type of erasure is the most appropriate for this disk

    DriveInterface* drive = disk->interface<BlockInterface>()->drive();
    if (drive && !drive->mediaAvailable()) {
        QMessageBox::warning(parent, tr("No Media"), tr("There is no media in the drive to restore to."));
        return;
    }
    if (drive && drive->optical()) {
        if (!ensureOpticalUtilitiesInstalled(parent)) return;

        QList<DriveInterface::MediaFormat> rewritables = {
            DriveInterface::CdRw,
            DriveInterface::DvdRw,
            DriveInterface::DvdPRw,
            DriveInterface::DvdPRwDl};

        if (rewritables.contains(drive->media()) || drive->opticalBlank()) {
            RestoreOpticalPopover* jp = new RestoreOpticalPopover(disk);
            tPopover* popover = new tPopover(jp);
            popover->setPopoverWidth(-200);
            popover->setPopoverSide(tPopover::Bottom);
            connect(jp, &RestoreOpticalPopover::done, popover, &tPopover::dismiss);
            connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
            connect(popover, &tPopover::dismissed, jp, &RestoreOpticalPopover::deleteLater);
            popover->show(parent->window());
        } else {
            QMessageBox::warning(parent, tr("Disc not writable"), tr("The disc in the drive is not writable."));
        }
    } else {
        RestoreOpticalPopover* jp = new RestoreOpticalPopover(disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(-200);
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &RestoreOpticalPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &RestoreOpticalPopover::deleteLater);
        popover->show(parent->window());
    }
}

void DiskOperationManager::showImageOperationUi(QWidget* parent, DiskObject* disk) {
    ImagePopover* jp = new ImagePopover(disk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(-200);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &ImagePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &ImagePopover::deleteLater);
    popover->show(parent->window());
}

void DiskOperationManager::showPartitionOperationUi(QWidget* parent, DiskObject* disk) {
    DiskObject* partitionDisk;

    if (disk->isInterfaceAvailable(DiskInterface::Partition)) {
        partitionDisk = disk->interface<PartitionInterface>()->parentTable();
    } else if (disk->isInterfaceAvailable(DiskInterface::PartitionTable)) {
        partitionDisk = disk;
    } else {
        tToast* toast = new tToast();
        toast->setTitle(tr("No Partition Table"));
        toast->setText(tr("Erase the disk to create a partition table."));
        toast->setActions({
            {"erase", "Erase Disk"}
        });
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        connect(toast, &tToast::actionClicked, parent, [=](QString key) {
            if (key == "erase") showEraseOperationUi(parent, disk);
        });
        toast->show(parent->window());
        return;
    }

    PartitionPopover* jp = new PartitionPopover(partitionDisk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(500);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &PartitionPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &PartitionPopover::deleteLater);
    popover->show(parent->window());
}

void DiskOperationManager::showAttachPvOperationUi(QWidget* parent, DiskObject* disk) {
    auto* jp = new AttachPvPopover(disk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(-200);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &AttachPvPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &AttachPvPopover::deleteLater);
    popover->show(parent->window());
}

bool DiskOperationManager::ensureOpticalUtilitiesInstalled(QWidget* parent) {
    if (QStandardPaths::findExecutable("cdrecord").isEmpty()) {
        QMessageBox* box = new QMessageBox();
        box->setParent(parent);
        box->setWindowTitle(tr("Optical tools unavailable"));
        box->setWindowModality(Qt::WindowModal);
        box->setIcon(QMessageBox::Warning);
        connect(box, &QMessageBox::finished, parent, [=] {
            box->deleteLater();
        });

        if (tApplication::currentPlatform() == tApplication::Flatpak) {
            box->setText(tr("theFrisbee can't write to optical discs when installed as a Flatpak."));
        } else {
            box->setText(tr("Your system does not have the necessary tools installed to write to optical discs."));
            box->setInformativeText(tr("You'll need to install either cdrtools or cdrkit using your system's package manager."));
        }
        box->open();
        return false;
    }

    return true;
}
