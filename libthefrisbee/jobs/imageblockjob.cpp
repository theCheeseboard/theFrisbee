#include "imageblockjob.h"

#include "DriveObjects/blockinterface.h"
#include "DriveObjects/diskobject.h"
#include "frisbeeexception.h"
#include "jobs/progress/imageblockjobprogress.h"
#include "tnotification.h"
#include <QCoroFuture>
#include <tlogger.h>

struct ImageBlockJobPrivate {
    DiskObject* disk;
    QIODevice* dest;
    QString fileName;

    QString displayName;
    QString description;

    quint64 progress = 0;
    quint64 totalProgress = 0;
    tJob::State state = tJob::Processing;
    int stage = 0;
};

ImageBlockJob::ImageBlockJob(DiskObject *disk, QObject *parent)
    : tJob{parent}
{
    d = new ImageBlockJobPrivate();
    d->disk = disk;
    d->displayName = disk->displayName();

    d->description = tr("Waiting for disk");
}

ImageBlockJob::~ImageBlockJob()
{
    delete d;
}

QCoro::Task<> ImageBlockJob::startImage(QIODevice *dest)
{
    if (d->stage != 0) co_return;

    d->dest = dest;

    // Try to acquire the lock
    d->description = tr("Waiting for other jobs to finish");
    emit descriptionChanged(d->description);

    co_await d->disk->lock();

    connect(this, &ImageBlockJob::stateChanged, this, [this] {
        // Release the lock
        if (d->state == Finished || d->state == Failed) {
            d->disk->releaseLock();
        }
    });

    try {
        BlockInterface* block = d->disk->interface<BlockInterface>();
        quint64 dataSize = block->size();
        auto ioDevice = co_await block->open(BlockInterface::Read, {});

        //Reserve space for the file
        if (auto file = qobject_cast<QFile*>(dest)) {
            if (!file->resize(dataSize)) {
                // Bail out
                d->state = Failed;
                emit stateChanged(Failed);

                d->description = tr("Not enough space to create image file");
                emit descriptionChanged(d->description);

                tInfo("ImageBlockJob") << "Image operation failed";

                tNotification* notification = new tNotification();
                notification->setSummary(tr("Couldn't Image Disc"));
                notification->setText(tr("Unable to create a disk image of %1.").arg(d->displayName));
                notification->post();
                co_return;
            }
        }

        co_await QtConcurrent::run([ioDevice, dest, dataSize, this] {
            quint64 read = 0;

            QFileDevice* fileDevice = qobject_cast<QFileDevice*>(dest);
            while (read < dataSize) {
                QByteArray buffer = ioDevice->read(1048576);
                read += buffer.size();
                dest->write(buffer);

                if (fileDevice) fileDevice->flush();

                d->totalProgress = dataSize;
                emit totalProgressChanged(d->totalProgress);

                d->progress = read;
                emit progressChanged(d->progress);

                d->description = tr("Imaging %1\n%2 of %3 copied").arg(QLocale().quoteString(d->displayName), QLocale().formattedDataSize(read), QLocale().formattedDataSize(dataSize));
                emit descriptionChanged(d->description);
            }

            ioDevice->close();
        });

        d->dest->close();

        d->totalProgress = 1;
        emit totalProgressChanged(d->totalProgress);

        d->progress = 1;
        emit progressChanged(d->progress);

        d->description = tr("Disk imaged successfully");
        emit descriptionChanged(d->description);

        d->state = Finished;
        emit stateChanged(Finished);

        tNotification* notification = new tNotification();
        notification->setSummary(tr("Imaged Disk"));
        notification->setText(tr("A disk image of %1 has been created.").arg(d->displayName));
        notification->post();
    } catch (FrisbeeException& ex) {
        // Bail out
        d->state = Failed;
        emit stateChanged(Failed);

        d->description = tr("Couldn't open the destination device for reading");
        emit descriptionChanged(d->description);

        tInfo("ImageBlockJob") << "Image operation failed";

        tNotification* notification = new tNotification();
        notification->setSummary(tr("Couldn't Image Disc"));
        notification->setText(tr("Unable to create a disk image of %1.").arg(d->displayName));
        notification->post();
    }
}

void ImageBlockJob::cancel()
{
    if (d->stage == 0) {
        d->stage = -1;

        // Bail out
        d->state = Failed;
        emit stateChanged(Failed);

        d->description = tr("Failed to image");
        emit descriptionChanged(d->description);

        tInfo("OpticalRestore") << "Image operation cancelled";
    }
}

QString ImageBlockJob::description()
{
    return d->description;
}

QString ImageBlockJob::displayName()
{
    return d->displayName;
}


quint64 ImageBlockJob::progress()
{
    return d->progress;
}

quint64 ImageBlockJob::totalProgress()
{
    return d->totalProgress;
}

tJob::State ImageBlockJob::state()
{
    return d->state;
}

QWidget *ImageBlockJob::makeProgressWidget()
{
    return new ImageBlockJobProgress(this);
}
