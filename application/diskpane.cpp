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
#include "operationmanager.h"

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
    OperationManager::showDiskOperationUi(this->window(), OperationManager::Erase, d->disk);
}

void DiskPane::on_imageButton_clicked() {
    OperationManager::showDiskOperationUi(this->window(), OperationManager::Image, d->disk);
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
    OperationManager::showDiskOperationUi(this->window(), OperationManager::Restore, d->disk);
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
