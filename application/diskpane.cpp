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
#include "diskpane.h"
#include "ui_diskpane.h"

#include <tapplication.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/partitioninterface.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>
#include <DriveObjects/partitiontableinterface.h>
#include <DriveObjects/filesysteminterface.h>
#include <QMessageBox>
#include <tpopover.h>
#include <ttoast.h>
#include <tjobmanager.h>
#include "operations/erasepartitiontablepopover.h"
#include "operations/erasepartitionpopover.h"
#include "operations/imagepopover.h"
#include "operations/partitionpopover.h"
#include "operations/eraseopticalpopover.h"
#include "operations/restoreopticalpopover.h"
#include "diskPanes/diskpanecomponent.h"
#include "diskPanes/overviewdiskpane.h"

struct DiskPanePrivate {
    DiskObject* disk;
    QList<DiskPaneComponent*> components;
};

DiskPane::DiskPane(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DiskPane) {
    ui->setupUi(this);

    d = new DiskPanePrivate();
    d->disk = disk;
    ui->titleLabel->setText(disk->displayName());

    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive) {
        connect(drive, &DriveInterface::changed, this, &DiskPane::updateButtons);
    }
    updateButtons();

    connect(d->disk, &DiskObject::lockedChanged, this, &DiskPane::updateLock);
    updateLock(d->disk->isLocked());

    ui->eraseButton->setProperty("type", "destructive");

    d->components.append(new OverviewDiskPane(disk, this));
    updateComponents();
}

DiskPane::~DiskPane() {
    delete ui;
    delete d;
}

void DiskPane::on_eraseButton_clicked() {
    //Determine which type of erasure is the most appropriate for this disk

    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive) {
        if (!drive->mediaAvailable()) {
            QMessageBox::warning(this, tr("No Media"), tr("There is no media in the drive to erase."));
            return;
        }
        if (drive->optical()) {
            if (!this->ensureOpticalUtilitiesInstalled()) return;

            if (drive->opticalBlank()) {
                if (QMessageBox::warning(this, tr("Disc already blank"), tr("The disc in the drive is already blank. Do you still want to erase it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
            }

            QList<DriveInterface::MediaFormat> rewritables = {
                DriveInterface::CdRw,
                DriveInterface::DvdRw,
                DriveInterface::DvdPRw,
                DriveInterface::DvdPRwDl
            };

            if (rewritables.contains(drive->media())) {
                EraseOpticalPopover* jp = new EraseOpticalPopover(d->disk);
                tPopover* popover = new tPopover(jp);
                popover->setPopoverWidth(SC_DPI(-200));
                popover->setPopoverSide(tPopover::Bottom);
                connect(jp, &EraseOpticalPopover::done, popover, &tPopover::dismiss);
                connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
                connect(popover, &tPopover::dismissed, jp, &EraseOpticalPopover::deleteLater);
                popover->show(this->window());
            } else {
                QMessageBox* box = new QMessageBox();
                box->setParent(this);
                box->setWindowModality(Qt::WindowModal);
                box->setWindowTitle(tr("Disc not rewritable"));
                box->setText(tr("The disc in the drive is not rewritable."));
                box->setInformativeText(tr("Only rewritable discs can be erased. If you need to destroy the data on this disc, you should physically break it in half."));
                box->setIcon(QMessageBox::Warning);
                connect(box, &QMessageBox::finished, this, [ = ] {
                    box->deleteLater();
                });
                box->open();
            }
            return;
        }
    }

    //Ensure this is not the root disk
    bool warnRoot = false;
    if (d->disk->isInterfaceAvailable(DiskInterface::Filesystem)) {
        for (const QByteArray& mountPoint : d->disk->interface<FilesystemInterface>()->mountPoints()) {
            if (mountPoint == "/") warnRoot = true;
        }
    } else if (d->disk->isInterfaceAvailable(DiskInterface::PartitionTable)) {
        for (DiskObject* partition : d->disk->interface<PartitionTableInterface>()->partitions()) {
            if (partition->isInterfaceAvailable(DiskInterface::Filesystem)) {
                for (const QByteArray& mountPoint : partition->interface<FilesystemInterface>()->mountPoints()) {
                    if (mountPoint == "/") warnRoot = true;
                }
            }
        }
    }

    if (warnRoot) {
        if (QMessageBox::warning(this, tr("System Disk"), tr("This is a system disk. Erasing it may cause your device to stop working altogether. Do you still want to erase it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No) return;
    }

    if (d->disk->isInterfaceAvailable(DiskInterface::Partition)) {

        ErasePartitionPopover* jp = new ErasePartitionPopover(d->disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(SC_DPI(-200));
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &ErasePartitionPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &ErasePartitionTablePopover::deleteLater);
        popover->show(this->window());
    } else {
        ErasePartitionTablePopover* jp = new ErasePartitionTablePopover(d->disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(SC_DPI(-200));
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &ErasePartitionTablePopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &ErasePartitionTablePopover::deleteLater);
        popover->show(this->window());
    }
}

void DiskPane::on_imageButton_clicked() {
    ImagePopover* jp = new ImagePopover(d->disk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI(-200));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &ImagePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &ImagePopover::deleteLater);
    popover->show(this->window());
}

void DiskPane::on_editPartitionsButton_clicked() {
    DiskObject* disk;

    if (d->disk->isInterfaceAvailable(DiskInterface::Partition)) {
        disk = d->disk->interface<PartitionInterface>()->parentTable();
    } else if (d->disk->isInterfaceAvailable(DiskInterface::PartitionTable)) {
        disk = d->disk;
    } else {
        tToast* toast = new tToast();
        toast->setTitle(tr("No Partition Table"));
        toast->setText(tr("Erase the disk to create a partition table."));
        toast->setActions({
            {"erase", "Erase Disk"}
        });
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        connect(toast, &tToast::actionClicked, this, [ = ](QString key) {
            if (key == "erase") ui->eraseButton->click();
        });
        toast->show(this->window());
        return;
    }

    PartitionPopover* jp = new PartitionPopover(disk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(500);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &PartitionPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &PartitionPopover::deleteLater);
    popover->show(this->window());
}

void DiskPane::on_restoreButton_clicked() {
    //Determine which type of erasure is the most appropriate for this disk

    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive && !drive->mediaAvailable()) {
        QMessageBox::warning(this, tr("No Media"), tr("There is no media in the drive to restore to."));
        return;
    }
    if (drive && drive->optical()) {
        if (!this->ensureOpticalUtilitiesInstalled()) return;

        QList<DriveInterface::MediaFormat> rewritables = {
            DriveInterface::CdRw,
            DriveInterface::DvdRw,
            DriveInterface::DvdPRw,
            DriveInterface::DvdPRwDl
        };

        if (rewritables.contains(drive->media()) || drive->opticalBlank()) {
            RestoreOpticalPopover* jp = new RestoreOpticalPopover(d->disk);
            tPopover* popover = new tPopover(jp);
            popover->setPopoverWidth(SC_DPI(-200));
            popover->setPopoverSide(tPopover::Bottom);
            connect(jp, &RestoreOpticalPopover::done, popover, &tPopover::dismiss);
            connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
            connect(popover, &tPopover::dismissed, jp, &RestoreOpticalPopover::deleteLater);
            popover->show(this->window());
        } else {
            QMessageBox::warning(this, tr("Disc not writable"), tr("The disc in the drive is not writable."));
        }
    } else {
        RestoreOpticalPopover* jp = new RestoreOpticalPopover(d->disk);
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(SC_DPI(-200));
        popover->setPopoverSide(tPopover::Bottom);
        connect(jp, &RestoreOpticalPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &RestoreOpticalPopover::deleteLater);
        popover->show(this->window());
    }
}

void DiskPane::updateComponents() {
    ui->componentsLayout->setEnabled(false);

    std::sort(d->components.begin(), d->components.end(), [ = ](DiskPaneComponent * first, DiskPaneComponent * second) {
        return first->order() < second->order();
    });

    for (DiskPaneComponent* component : d->components) {
        ui->componentsLayout->removeWidget(component);
    }

    for (DiskPaneComponent* component : d->components) {
        ui->componentsLayout->addWidget(component);
    }
    ui->componentsLayout->setEnabled(true);
}

void DiskPane::updateButtons() {
    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive) {
        if (drive->isOpticalDrive()) {
            ui->checkButton->setVisible(false);
            ui->editPartitionsButton->setVisible(false);

            ui->operationsWidget->setVisible(drive->mediaAvailable());
            ui->line->setVisible(drive->mediaAvailable());
        }
    }
}

void DiskPane::updateLock(bool locked) {
    ui->jobsRunningWidget->setExpanded(locked);
}

bool DiskPane::ensureOpticalUtilitiesInstalled() {
    if (QStandardPaths::findExecutable("cdrecord").isEmpty()) {
        QMessageBox* box = new QMessageBox();
        box->setParent(this);
        box->setWindowTitle(tr("Optical tools unavailable"));
        box->setWindowModality(Qt::WindowModal);
        box->setIcon(QMessageBox::Warning);
        connect(box, &QMessageBox::finished, this, [ = ] {
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

void DiskPane::on_viewJobsButton_clicked() {
    tJobManager::showJobsPopover(this);
}

void DiskPane::on_checkButton_clicked() {
    QMessageBox* box = new QMessageBox();
    box->setParent(this);
    box->setWindowModality(Qt::WindowModal);
    box->setWindowTitle(tr("Check this disk"));
    box->setText(tr("Check this disk for errors?"));
    box->setInformativeText(tr("Depending on the size of the disk, the check may take a while."));
    box->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box->setIcon(QMessageBox::Question);
    connect(box, &QMessageBox::finished, this, [ = ](int result) {
        if (result == QMessageBox::Yes) {
            //Check the disk
        }
        box->deleteLater();
    });
    box->open();
}
