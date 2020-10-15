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

#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>

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

void ImagePopover::on_imageButton_clicked() {
    QWidget* parent = this->window();
    QString fileName = ui->outputFileBox->text();
    DiskObject* disk = d->disk;

    tPromise<void>::runOnNewThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        tPromiseResults<QIODevice*> results = d->disk->interface<BlockInterface>()->open(BlockInterface::Read, {})->await();
        if (results.error.isEmpty()) {
            QIODevice* device = results.result;

            QFile file(fileName);
            file.open(QFile::WriteOnly);

            quint64 number = 0;
            while (number < disk->interface<BlockInterface>()->size()) {
                QByteArray buf = device->read(2048);
                file.write(buf);
                number += buf.length();
            }

            file.close();
            device->close();
            res();
        } else {
            rej(results.error);
        }
    })->then([ = ] {
        tToast* toast = new tToast();
        toast->setTitle(tr("Disk Imaged"));
        toast->setText(tr("The disk image has been created"));
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(parent);
    })->error([ = ](QString error) {
        tToast* toast = new tToast();
        toast->setTitle(tr("Couldn't image disk"));
        toast->setText(error);
        connect(toast, &tToast::dismissed, toast, &tToast::deleteLater);
        toast->show(parent);
    });
    emit done();
}

void ImagePopover::on_titleLabel_backButtonClicked() {
    emit done();
}
