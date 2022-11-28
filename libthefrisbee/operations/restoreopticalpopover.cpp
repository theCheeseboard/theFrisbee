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
#include "restoreopticalpopover.h"
#include "ui_restoreopticalpopover.h"

#include "diskmodel.h"
#include "jobs/restorediskjob.h"
#include "jobs/restoreopticaljob.h"
#include "tjobmanager.h"
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/driveinterface.h>
#include <QFileDialog>
#include <QMessageBox>
#include <frisbeeexception.h>

struct RestoreOpticalPopoverPrivate {
        DiskObject* disk;
};

RestoreOpticalPopover::RestoreOpticalPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RestoreOpticalPopover) {
    ui->setupUi(this);
    d = new RestoreOpticalPopoverPrivate();
    d->disk = disk;

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->restoreConfirmWidget->setFixedWidth(SC_DPI(600));
    ui->restoreConfirmOpticalWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    ui->titleLabel_3->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->mainStack->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->doRestoreButton->setProperty("type", "destructive");
    ui->doRestoreButtonOptical->setProperty("type", "destructive");
    ui->diskSelection->setModel(new DiskModel());

    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive && drive->isOpticalDrive()) {
        ui->titleLabel->setText(tr("Restore Disc"));
    } else {
        ui->titleLabel->setText(tr("Restore to Block"));
    }
}

RestoreOpticalPopover::~RestoreOpticalPopover() {
    delete d;
    delete ui;
}

void RestoreOpticalPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void RestoreOpticalPopover::on_restoreFileButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->restoreFilePage);
    }
}

void RestoreOpticalPopover::on_restoreDiskButton_toggled(bool checked) {
    if (checked) {
        ui->stackedWidget->setCurrentWidget(ui->restoreDiskPage);
    }
}

void RestoreOpticalPopover::on_browseButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"Disk Images (*.img, *.iso)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [this](QString file) {
        ui->restoreFileBox->setText(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void RestoreOpticalPopover::on_restoreButton_clicked() {
    DiskObject* disk = static_cast<DiskObject*>(ui->diskSelection->currentIndex().internalPointer());

    if (ui->stackedWidget->currentWidget() == ui->restoreDiskPage && disk == d->disk) {
        QMessageBox::warning(this, tr("Unable to restore to self"), tr("A medium cannot be restored to itself. Select another medium to restore."));
        return;
    }

    DriveInterface* drive = d->disk->interface<BlockInterface>()->drive();
    if (drive->isOpticalDrive()) {
        if (!drive->opticalBlank()) {
            ui->mainStack->setCurrentWidget(ui->confirmOpticalPage);
        } else {
            // Don't bother confirming
            performRestoreOperation();
        }
    } else {
        ui->mainStack->setCurrentWidget(ui->confirmPage);
    }
}

void RestoreOpticalPopover::on_doRestoreButton_clicked() {
    performRestoreOperation();
}

void RestoreOpticalPopover::on_titleLabel_2_backButtonClicked() {
    ui->mainStack->setCurrentWidget(ui->mainPage);
}

void RestoreOpticalPopover::on_titleLabel_3_backButtonClicked() {
    ui->mainStack->setCurrentWidget(ui->mainPage);
}

void RestoreOpticalPopover::on_doRestoreButtonOptical_clicked() {
    performRestoreOperation();
}

QCoro::Task<> RestoreOpticalPopover::performRestoreOperation() {
    RestoreJob* job;
    if (d->disk->interface<BlockInterface>()->drive()->isOpticalDrive()) {
        job = new RestoreOpticalJob(d->disk);
    } else {
        job = new RestoreDiskJob(d->disk);
    }

    tJobManager::trackJob(job);
    emit done();

    if (ui->stackedWidget->currentWidget() == ui->restoreFilePage) {
        // Restore a file
        QFile* file = new QFile(ui->restoreFileBox->text());
        if (!file->open(QFile::ReadOnly)) {
            // Bail
            job->cancel();
            co_return;
        }

        job->startRestore(file, file->size());
    } else {
        // Restore a disk
        DiskObject* disk = static_cast<DiskObject*>(ui->diskSelection->currentIndex().internalPointer());

        BlockInterface* block = disk->interface<BlockInterface>();
        if (!block) {
            // Bail
            job->cancel();
            co_return;
        }

        try {
            auto ioDevice = co_await block->open(BlockInterface::Read, {});
            job->startRestore(ioDevice, block->size());
        } catch (FrisbeeException& ex) {
            job->cancel();
        }
    }
}
