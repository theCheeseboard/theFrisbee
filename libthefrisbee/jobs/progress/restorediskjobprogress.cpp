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
#include "restorediskjobprogress.h"
#include "ui_restorediskjobprogress.h"

#include <DriveObjects/diskobject.h>
#include "../restorediskjob.h"

struct RestoreDiskJobProgressPrivate {
    RestoreDiskJob* job;
};

RestoreDiskJobProgress::RestoreDiskJobProgress(RestoreDiskJob* job, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RestoreDiskJobProgress) {
    ui->setupUi(this);

    d = new RestoreDiskJobProgressPrivate();
    d->job = job;

    ui->titleLabel->setText(tr("Restore to %1").arg(job->displayName()).toUpper());

    connect(job, &RestoreDiskJob::stateChanged, this, [ = ](RestoreDiskJob::State state) {
        updateState();
    });
    connect(job, &RestoreDiskJob::totalProgressChanged, this, [ = ](quint64 totalProgress) {
        ui->progressBar->setMaximum(totalProgress);
    });
    connect(job, &RestoreDiskJob::progressChanged, this, [ = ](quint64 progress) {
        ui->progressBar->setValue(progress);
    });
    connect(job, &RestoreDiskJob::descriptionChanged, ui->statusLabel, &QLabel::setText);
    updateState();
    ui->statusLabel->setText(job->description());
}

RestoreDiskJobProgress::~RestoreDiskJobProgress() {
    delete ui;
}

void RestoreDiskJobProgress::updateState() {
    switch (d->job->state()) {
        case tJob::Processing:
            ui->progressBar->setMaximum(d->job->totalProgress());
            ui->progressBar->setValue(d->job->progress());
            break;
        case tJob::Finished:
            ui->progressBar->setMaximum(1);
            ui->progressBar->setValue(1);
            break;
        case tJob::Failed:
            ui->progressBar->setMaximum(1);
            ui->progressBar->setValue(1);
            break;
    }
}
