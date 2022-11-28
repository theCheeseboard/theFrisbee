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
#include "imagepopover.h"
#include "ui_imagepopover.h"

#include <DriveObjects/blockinterface.h>
#include <DriveObjects/diskobject.h>
#include "jobs/imageblockjob.h"
#include <tjobmanager.h>
#include <QFileDialog>

#include <frisbeeexception.h>
#include <ttoast.h>

struct ImagePopoverPrivate {
        DiskObject* disk;
};

ImagePopover::ImagePopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ImagePopover) {
    ui->setupUi(this);
    d = new ImagePopoverPrivate();
    d->disk = disk;

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
}

ImagePopover::~ImagePopover() {
    delete ui;
}

QCoro::Task<> ImagePopover::on_imageButton_clicked() {
    auto* job = new ImageBlockJob(d->disk);

    tJobManager::trackJob(job);
    emit done();

    // Restore a file
    auto* file = new QFile(ui->outputFileBox->text());
    if (!file->open(QFile::WriteOnly)) {
        // Bail
        job->cancel();
        co_return;
    }

    job->startImage(file);
}

void ImagePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void ImagePopover::on_browseButton_clicked() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setNameFilters({"Disk Images (*.img *.iso)"});
    dialog->setFileMode(QFileDialog::AnyFile);
    connect(dialog, &QFileDialog::fileSelected, this, [this](QString file) {
        ui->outputFileBox->setText(file);
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}
