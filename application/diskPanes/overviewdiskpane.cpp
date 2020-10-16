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
#include "overviewdiskpane.h"
#include "ui_overviewdiskpane.h"

#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>
#include <DriveObjects/filesysteminterface.h>

struct OverviewDiskPanePrivate {
    DiskObject* disk;
};

OverviewDiskPane::OverviewDiskPane(DiskObject* disk, QWidget* parent) :
    DiskPaneComponent(parent),
    ui(new Ui::OverviewDiskPane) {
    ui->setupUi(this);

    d = new OverviewDiskPanePrivate();

    d->disk = disk;
    connect(disk, &DiskObject::interfaceAdded, this, &OverviewDiskPane::updateData);
    connect(disk, &DiskObject::interfaceRemoved, this, &OverviewDiskPane::updateData);

    BlockInterface* block = disk->interface<BlockInterface>();
    connect(block, &BlockInterface::sizeChanged, this, &OverviewDiskPane::updateData);
    if (block->drive()) {
        connect(block->drive(), &DriveInterface::changed, this, &OverviewDiskPane::updateData);
    }

    connect(d->disk, &DiskObject::lockedChanged, this, &OverviewDiskPane::updateLock);
    updateLock(d->disk->isLocked());

    updateData();
}

OverviewDiskPane::~OverviewDiskPane() {
    delete d;
    delete ui;
}

void OverviewDiskPane::updateData() {
    ui->diskNameLabel->setText(d->disk->displayName());

    BlockInterface* block = d->disk->interface<BlockInterface>();

    QStringList parts;
    parts.append(block->blockName());
    parts.append(QLocale().formattedDataSize(block->size()));

    if (block->drive()) {
        ui->ejectButton->setVisible(block->drive()->ejectable());
        if (block->drive()->isOpticalDrive()) {
            if (block->drive()->mediaAvailable()) {
                if (block->drive()->opticalBlank()) {
                    parts.replace(1, tr("Blank Disc"));
                }

                switch (block->drive()->media()) {
                    case DriveInterface::Cd:
                        parts.append(tr("CD"));
                        break;
                    case DriveInterface::CdR:
                        parts.append(tr("CD-R"));
                        break;
                    case DriveInterface::CdRw:
                        parts.append(tr("CD-RW"));
                        break;
                    case DriveInterface::Dvd:
                        parts.append(tr("DVD"));
                        break;
                    case DriveInterface::DvdR:
                        parts.append(tr("DVD-R"));
                        break;
                    case DriveInterface::DvdRw:
                        parts.append(tr("DVD-RW"));
                        break;
                    case DriveInterface::DvdRam:
                        parts.append(tr("DVD-RAM"));
                        break;
                    case DriveInterface::DvdPR:
                        parts.append(tr("DVD+R"));
                        break;
                    case DriveInterface::DvdPRw:
                        parts.append(tr("DVD+RW"));
                        break;
                    case DriveInterface::DvdPRDl:
                        parts.append(tr("DVD+R Dual Layer"));
                        break;
                    case DriveInterface::DvdPRwDl:
                        parts.append(tr("DVD+RW Dual Layer"));
                        break;
                    case DriveInterface::Bd:
                        parts.append(tr("BD"));
                        break;
                    case DriveInterface::BdR:
                        parts.append(tr("BD-R"));
                        break;
                    case DriveInterface::BdRe:
                        parts.append(tr("BD-RE"));
                        break;
                    case DriveInterface::HdDvd:
                        parts.append(tr("HD-DVD"));
                        break;
                    case DriveInterface::HdDvdR:
                        parts.append(tr("HD-DVD-R"));
                        break;
                    case DriveInterface::HdDvdRw:
                        parts.append(tr("HD-DVD-RW"));
                        break;
                    case DriveInterface::MagnetoOptical:
                        parts.append(tr("Magneto Optical"));
                        break;
                    default:
                        break;
                }
            } else {
                parts.replace(1, tr("No Disc Inserted"));
            }
        }
    }

    FilesystemInterface* fs = d->disk->interface<FilesystemInterface>();
    if (fs) {
        bool isMounted = !fs->mountPoints().isEmpty();
        ui->mountButton->setVisible(!isMounted);
        ui->unmountButton->setVisible(isMounted);
    } else {
        ui->mountButton->setVisible(false);
        ui->unmountButton->setVisible(false);
    }

    ui->iconLabel->setPixmap(d->disk->icon().pixmap(SC_DPI_T(QSize(32, 32), QSize)));
    ui->diskInfoLabel->setText(parts.join(" · "));
}

void OverviewDiskPane::updateLock(bool locked) {
    ui->ejectButton->setEnabled(!locked);
    ui->mountButton->setEnabled(!locked);
    ui->unmountButton->setEnabled(!locked);
}

int OverviewDiskPane::order() const {
    return 0;
}

void OverviewDiskPane::on_ejectButton_clicked() {
    d->disk->interface<BlockInterface>()->drive()->eject();
}

void OverviewDiskPane::on_mountButton_clicked() {
    d->disk->interface<FilesystemInterface>()->mount();
}

void OverviewDiskPane::on_unmountButton_clicked() {
    d->disk->interface<FilesystemInterface>()->unmount();
}
