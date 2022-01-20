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
#include "eraseopticalpopover.h"
#include "ui_eraseopticalpopover.h"

#include <tjobmanager.h>
#include "jobs/erasecdrwjob.h"
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>

struct EraseOpticalPopoverPrivate {
    DiskObject* disk;
};

EraseOpticalPopover::EraseOpticalPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::EraseOpticalPopover) {
    ui->setupUi(this);
    d = new EraseOpticalPopoverPrivate();
    d->disk = disk;

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->eraseConfirmWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
    ui->titleLabel_2->setBackButtonShown(true);

    ui->eraseButton->setProperty("type", "destructive");
    ui->doEraseButton->setProperty("type", "destructive");
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

EraseOpticalPopover::~EraseOpticalPopover() {
    delete ui;
    delete d;
}

void EraseOpticalPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void EraseOpticalPopover::on_eraseButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void EraseOpticalPopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void EraseOpticalPopover::on_doEraseButton_clicked() {
    EraseCdRwJob* job = new EraseCdRwJob(d->disk, ui->ersaeTypeBox->currentIndex() == 0);
    tJobManager::trackJob(job);

//    switch (d->disk->interface<BlockInterface>()->drive()->media()) {
//        case DriveInterface::CdRw: {
//            break;
//        }
//        default:
//            break;
//    }

    emit done();
}
