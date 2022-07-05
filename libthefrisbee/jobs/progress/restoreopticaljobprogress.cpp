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
#include "restoreopticaljobprogress.h"
#include "ui_restoreopticaljobprogress.h"

#include "../restoreopticaljob.h"
#include <DriveObjects/diskobject.h>

struct RestoreOpticalJobProgressPrivate {
        RestoreOpticalJob* job;
};

RestoreOpticalJobProgress::RestoreOpticalJobProgress(RestoreOpticalJob* job, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RestoreOpticalJobProgress) {
    ui->setupUi(this);

    d = new RestoreOpticalJobProgressPrivate();
    d->job = job;

    ui->titleLabel->setText(tr("Restore to %1").arg(job->displayName()).toUpper());

    connect(job, &RestoreOpticalJob::stateChanged, this, [this](RestoreOpticalJob::State state) {
        updateState();
    });
    connect(job, &RestoreOpticalJob::totalProgressChanged, this, [this](quint64 totalProgress) {
        ui->progressBar->setMaximum(totalProgress);
    });
    connect(job, &RestoreOpticalJob::progressChanged, this, [this](quint64 progress) {
        ui->progressBar->setValue(progress);
    });
    connect(job, &RestoreOpticalJob::descriptionChanged, ui->statusLabel, &QLabel::setText);
    updateState();
    ui->statusLabel->setText(job->description());
}

RestoreOpticalJobProgress::~RestoreOpticalJobProgress() {
    delete ui;
}

void RestoreOpticalJobProgress::updateState() {
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
