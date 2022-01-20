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
#include "erasepartitionpopover.h"
#include "ui_erasepartitionpopover.h"

#include <driveobjectmanager.h>
#include "partitioninformation.h"
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>

struct ErasePartitionPopoverPrivate {
    DiskObject* disk;
};

ErasePartitionPopover::ErasePartitionPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ErasePartitionPopover) {
    ui->setupUi(this);
    d = new ErasePartitionPopoverPrivate();

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->eraseConfirmWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);

    ui->eraseButton->setProperty("type", "destructive");
    ui->doEraseButton->setProperty("type", "destructive");
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    for (QString type : PartitionInformation::availableFormatTypes()) {
        ui->filesystemBox->addItem(PartitionInformation::typeName(type), type);
    }
    ui->filesystemBox->addItem(tr("Erase Only"), "empty");

    d->disk = disk;
}

ErasePartitionPopover::~ErasePartitionPopover() {
    delete ui;
    delete d;
}

void ErasePartitionPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void ErasePartitionPopover::on_eraseButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ErasePartitionPopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void ErasePartitionPopover::on_doEraseButton_clicked() {
    //TODO: Update partition type
    d->disk->interface<BlockInterface>()->format(ui->filesystemBox->currentData().toString(), {
        {"label", ui->labelBox->text()},
        {"no-block", true},
        {"update-partition-type", true},
        {"tear-down", true}
    });
    emit done();
}
