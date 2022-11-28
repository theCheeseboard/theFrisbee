#include "imageblockjobprogress.h"
#include "ui_imageblockjobprogress.h"

#include "../imageblockjob.h"

struct ImageBlockJobProgressPrivate {
    ImageBlockJob* job;
};

ImageBlockJobProgress::ImageBlockJobProgress(ImageBlockJob *job, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageBlockJobProgress)
{
    ui->setupUi(this);

    d = new ImageBlockJobProgressPrivate();
    d->job = job;

    ui->titleLabel->setText(tr("Image %1").arg(job->displayName()).toUpper());

    connect(job, &ImageBlockJob::stateChanged, this, [this](ImageBlockJob::State state) {
        updateState();
    });
    connect(job, &ImageBlockJob::totalProgressChanged, this, [this](quint64 totalProgress) {
        ui->progressBar->setMaximum(totalProgress);
    });
    connect(job, &ImageBlockJob::progressChanged, this, [this](quint64 progress) {
        ui->progressBar->setValue(progress);
    });
    connect(job, &ImageBlockJob::descriptionChanged, ui->statusLabel, &QLabel::setText);
    updateState();
    ui->statusLabel->setText(job->description());
}

ImageBlockJobProgress::~ImageBlockJobProgress()
{
    delete ui;
    delete d;
}

void ImageBlockJobProgress::updateState()
{
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
