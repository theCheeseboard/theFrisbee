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

#include "tjobmanager.h"
#include "jobs/restoreopticaljob.h"
#include <QFileDialog>
#include <QMessageBox>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/driveinterface.h>
#include <DriveObjects/blockinterface.h>
#include "diskmodel.h"

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
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->mainStack->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->doRestoreButton->setProperty("type", "destructive");
    ui->diskSelection->setModel(new DiskModel());
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
    connect(dialog, &QFileDialog::fileSelected, this, [ = ](QString file) {
        ui->restoreFileBox->setText(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void RestoreOpticalPopover::on_restoreButton_clicked() {
    DiskObject* disk = static_cast<DiskObject*>(ui->diskSelection->currentIndex().internalPointer());

    if (ui->stackedWidget->currentWidget() == ui->restoreDiskPage && disk == d->disk) {
        QMessageBox::warning(this, tr("Unable to restore same media"), tr("A medium cannot be restored to itself. Select another medium to restore."));
        return;
    }

    if (!d->disk->interface<BlockInterface>()->drive()->opticalBlank()) {
        ui->mainStack->setCurrentWidget(ui->confirmPage);
    } else {
        //Don't bother confirming
        on_doRestoreButton_clicked();
    }
}

void RestoreOpticalPopover::on_doRestoreButton_clicked() {
    if (ui->stackedWidget->currentWidget() == ui->restoreFilePage) {
        RestoreOpticalJob* job = new RestoreOpticalJob(d->disk);
        tJobManager::trackJob(job);

        emit done();

        //Restore a file
        QFile* file = new QFile(ui->restoreFileBox->text());
        if (!file->open(QFile::ReadOnly)) {
            //Bail
            job->cancel();
            return;
        }

        job->startRestore(file, file->size());
    } else {
        //Restore a disk
        DiskObject* disk = static_cast<DiskObject*>(ui->diskSelection->currentIndex().internalPointer());
        RestoreOpticalJob* job = new RestoreOpticalJob(d->disk);
        tJobManager::trackJob(job);

        emit done();

        BlockInterface* block = disk->interface<BlockInterface>();
        if (!block) {
            //Bail
            job->cancel();
            return;
        }

        block->open(BlockInterface::Read, {})->then([ = ](QIODevice * ioDevice) {
            job->startRestore(ioDevice, block->size());
        })->error([ = ](QString error) {
            //Bail
            job->cancel();
            return;
        });
    }
}

void RestoreOpticalPopover::on_titleLabel_2_backButtonClicked() {
    ui->mainStack->setCurrentWidget(ui->mainPage);
}
