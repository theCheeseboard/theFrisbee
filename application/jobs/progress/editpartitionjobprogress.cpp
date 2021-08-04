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
#include "editpartitionjobprogress.h"
#include "ui_editpartitionjobprogress.h"

#include <DriveObjects/diskobject.h>
#include "../editpartitionjob.h"

EditPartitionJobProgress::EditPartitionJobProgress(EditPartitionJob* job, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::EditPartitionJobProgress) {
    ui->setupUi(this);

    ui->titleLabel->setText(tr("Partitioning %1").arg(QLocale().quoteString(job->disk()->displayName())).toUpper());

    connect(job, &EditPartitionJob::descriptionChanged, ui->descriptionLabel, &QLabel::setText);
    ui->descriptionLabel->setText(job->description());

    connect(job, &EditPartitionJob::totalProgressChanged, this, [ = ](quint64 totalProgress) {
        ui->progressBar->setMaximum(totalProgress);
    });
    connect(job, &EditPartitionJob::progressChanged, this, [ = ](quint64 progress) {
        ui->progressBar->setValue(progress);
    });
    ui->progressBar->setMaximum(job->totalProgress());
    ui->progressBar->setValue(job->progress());
}

EditPartitionJobProgress::~EditPartitionJobProgress() {
    delete ui;
}
