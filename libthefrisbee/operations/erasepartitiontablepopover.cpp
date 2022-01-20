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
#include "erasepartitiontablepopover.h"
#include "ui_erasepartitiontablepopover.h"

#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>

struct ErasePartitionTablePopoverPrivate {
    DiskObject* disk;
};

ErasePartitionTablePopover::ErasePartitionTablePopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ErasePartitionTablePopover) {
    ui->setupUi(this);
    d = new ErasePartitionTablePopoverPrivate();
    d->disk = disk;

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->eraseConfirmWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);

    ui->eraseButton->setProperty("type", "destructive");
    ui->doEraseButton->setProperty("type", "destructive");
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

ErasePartitionTablePopover::~ErasePartitionTablePopover() {
    delete d;
    delete ui;
}

void ErasePartitionTablePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void ErasePartitionTablePopover::on_eraseButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void ErasePartitionTablePopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void ErasePartitionTablePopover::on_doEraseButton_clicked() {
    QString formatType;
    switch (ui->partitionTableBox->currentIndex()) {
        case 0:
            formatType = "gpt";
            break;
        case 1:
            formatType = "dos";
            break;
        case 2:
            formatType = "empty";
            break;
    }

    d->disk->interface<BlockInterface>()->format(formatType, {});
    emit done();
}
