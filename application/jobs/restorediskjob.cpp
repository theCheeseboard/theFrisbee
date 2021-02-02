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
#include "restorediskjob.h"

#include <tlogger.h>
#include <QProcess>
#include <tnotification.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>
#include "progress/restorediskjobprogress.h"
#include "optical/opticalerrortracker.h"

struct RestoreDiskJobPrivate {
    quint64 progress = 0;
    quint64 totalProgress = 0;

    QIODevice* source;
    quint64 dataSize;
    DiskObject* disk;

    QString description;

    tJob::State state;
    int stage = 0;
};

RestoreDiskJob::RestoreDiskJob(DiskObject* disk, QObject* parent) : RestoreJob(parent) {
    d = new RestoreDiskJobPrivate();
    d->disk = disk;

    d->description = tr("Waiting for disk");
}

RestoreDiskJob::~RestoreDiskJob() {
    delete d;
}

void RestoreDiskJob::startRestore(QIODevice* source, quint64 dataSize) {
    if (d->stage != 0) return;

    d->source = source;
    d->dataSize = dataSize;

    //Try to acquire the lock
    d->description = tr("Waiting for other jobs to finish");
    emit descriptionChanged(d->description);

    d->disk->lock()->then([ = ] {
        connect(this, &RestoreDiskJob::stateChanged, this, [ = ] {
            //Release the lock
            if (d->state == Finished || d->state == Failed) {
                d->disk->releaseLock();
            }
        });

        BlockInterface* block = d->disk->interface<BlockInterface>();
        block->open(BlockInterface::Write, {})->then([ = ](QIODevice * ioDevice) {
            TPROMISE_CREATE_NEW_THREAD(void, {
                Q_UNUSED(rej);
                quint64 read = 0;

                QFileDevice* fileDevice = qobject_cast<QFileDevice*>(ioDevice);
                while (read < dataSize) {
                    QByteArray buffer = source->read(1048576);
                    read += buffer.size();
                    ioDevice->write(buffer);

                    if (fileDevice) fileDevice->flush();

                    d->totalProgress = dataSize;
                    emit totalProgressChanged(d->totalProgress);

                    d->progress = read;
                    emit progressChanged(d->progress);

                    d->description = tr("Restoring to the target disk\n%1 of %2 restored").arg(QLocale().formattedDataSize(read), QLocale().formattedDataSize(dataSize));
                    emit descriptionChanged(d->description);
                }

                ioDevice->close();

                res();
            })->then([ = ] {
                d->source->close();

                d->totalProgress = 1;
                emit totalProgressChanged(d->totalProgress);

                d->progress = 1;
                emit progressChanged(d->progress);

                d->description = tr("Disk restored successfully");
                emit descriptionChanged(d->description);

                d->state = Finished;
                emit stateChanged(Finished);

                //TODO: fire a notification
            });
        })->error([ = ](QString error) {
            //Bail out
            d->state = Failed;
            emit stateChanged(Failed);

            d->description = tr("Couldn't open the destination device for writing");
            emit descriptionChanged(d->description);

            tInfo("OpticalRestore") << "Restore operation failed";
            return;
        });
    });
}

void RestoreDiskJob::cancel() {
    if (d->stage == 0) {
        d->stage = -1;

        //Bail out
        d->state = Failed;
        emit stateChanged(Failed);

        d->description = tr("Failed to restore image");
        emit descriptionChanged(d->description);

        tInfo("OpticalRestore") << "Restore operation cancelled";
    }
}

DiskObject* RestoreDiskJob::disk() {
    return d->disk;
}

QString RestoreDiskJob::description() {
    return d->description;
}

quint64 RestoreDiskJob::progress() {
    return d->progress;
}

quint64 RestoreDiskJob::totalProgress() {
    return d->totalProgress;
}

tJob::State RestoreDiskJob::state() {
    return d->state;
}

QWidget* RestoreDiskJob::makeProgressWidget() {
    return new RestoreDiskJobProgress(this);
}
