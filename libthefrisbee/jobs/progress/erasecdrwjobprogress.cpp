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
#include "erasecdrwjobprogress.h"
#include "ui_erasecdrwjobprogress.h"

#include "../erasecdrwjob.h"
#include <DriveObjects/diskobject.h>

struct EraseCdRwJobProgressPrivate {
        EraseCdRwJob* job;
};

EraseCdRwJobProgress::EraseCdRwJobProgress(EraseCdRwJob* job, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::EraseCdRwJobProgress) {
    ui->setupUi(this);

    d = new EraseCdRwJobProgressPrivate();
    d->job = job;

    ui->titleLabel->setText(job->titleString().toUpper());

    connect(job, &EraseCdRwJob::stateChanged, this, [this](EraseCdRwJob::State state) {
        updateState();
    });
    connect(job, &EraseCdRwJob::descriptionChanged, ui->statusLabel, &QLabel::setText);
    updateState();
    ui->statusLabel->setText(job->description());
}

EraseCdRwJobProgress::~EraseCdRwJobProgress() {
    delete ui;
}

void EraseCdRwJobProgress::updateState() {
    switch (d->job->state()) {
        case tJob::Processing:
            ui->progressBar->setMaximum(0);
            ui->progressBar->setValue(0);
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
