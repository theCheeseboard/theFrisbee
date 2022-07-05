/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "creatediskimagepopover.h"
#include "ui_creatediskimagepopover.h"

#include <QDBusUnixFileDescriptor>
#include <QFile>
#include <QFileDialog>
#include <driveobjectmanager.h>
#include <frisbeeexception.h>
#include <terrorflash.h>
#include <tlogger.h>

CreateDiskImagePopover::CreateDiskImagePopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::CreateDiskImagePopover) {
    ui->setupUi(this);

    ui->optionsWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);
}

CreateDiskImagePopover::~CreateDiskImagePopover() {
    delete ui;
}

void CreateDiskImagePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> CreateDiskImagePopover::on_imageButton_clicked() {
    if (!ui->sizeBox->hasValidSize()) {
        tErrorFlash::flashError(ui->sizeBox);
        co_return;
    }

    QFile file(ui->outputFileBox->text());
    file.open(QFile::WriteOnly);
    file.resize(ui->sizeBox->size());

    if (ui->attachCheckbox->isChecked()) {
        QDBusUnixFileDescriptor fd(file.handle());

        try {
            auto loopSetupOptions = QVariantMap({
                {"read-only", false}
            });
            co_await DriveObjectManager::loopSetup(fd, loopSetupOptions);
        } catch (FrisbeeException& ex) {
            tCritical("LoopSetup") << ex.response();
        }
    } else {
        file.close();
    }

    emit done();
}

void CreateDiskImagePopover::on_browseButton_clicked() {
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
