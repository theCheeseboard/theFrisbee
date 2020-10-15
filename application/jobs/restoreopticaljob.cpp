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
#include "restoreopticaljob.h"

#include <QProcess>
#include <tnotification.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/driveinterface.h>
#include "progress/restoreopticaljobprogress.h"

struct RestoreOpticalJobPrivate {
    quint64 progress = 0;
    quint64 totalProgress = 0;

    QIODevice* source;
    quint64 dataSize;
    DiskObject* disk;

    QProcess* burnProcess;
    quint64 writtenBytes = 0;

    QString description;

    tJob::State state;
    int stage = 0;
};

RestoreOpticalJob::RestoreOpticalJob(DiskObject* disk, QObject* parent) : tJob(parent) {
    d = new RestoreOpticalJobPrivate();
    d->disk = disk;

    d->description = tr("Waiting for restore medium");
}

RestoreOpticalJob::~RestoreOpticalJob() {
    delete d;
}

void RestoreOpticalJob::startRestore(QIODevice* source, quint64 dataSize) {
    if (d->stage != 0) return;

    d->source = source;
    d->dataSize = dataSize;
    runNextStage();
}

void RestoreOpticalJob::cancel() {
    if (d->stage == 0) {
        d->stage = -1;

        //Bail out
        d->state = Failed;
        emit stateChanged(Failed);

        d->description = tr("Failed to restore image");
        emit descriptionChanged(d->description);
    }
}

DiskObject* RestoreOpticalJob::disk() {
    return d->disk;
}

QString RestoreOpticalJob::description() {
    return d->description;
}

void RestoreOpticalJob::runNextStage() {
    QString displayName = d->disk->displayName();

    d->stage++;
    switch (d->stage) {
        case 1: {
            //Start restoring the image
            d->description = tr("Preparing to restore");
            emit descriptionChanged(d->description);

            d->burnProcess = new QProcess();
            connect(d->burnProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [ = ](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitStatus);
                if (exitCode == 0) {
                    d->source->close();

                    runNextStage();
                } else {
                    d->state = Failed;
                    emit stateChanged(Failed);

                    d->description = tr("Failed to restore disc");
                    emit descriptionChanged(d->description);

                    tNotification* notification = new tNotification();
                    notification->setSummary(tr("Couldn't Restore Disc"));
                    notification->setText(tr("The disc in %1 could not be restored.").arg(displayName));
                    notification->post();
                }
                emit progressChanged(1);
                emit totalProgressChanged(1);
                d->burnProcess->deleteLater();
            });
            connect(d->burnProcess, &QProcess::readyRead, this, [ = ] {
                QByteArray peek = d->burnProcess->peek(1024);
                while (d->burnProcess->canReadLine() || peek.contains('\r')) {
                    QString line;
                    if (d->burnProcess->canReadLine()) {
                        line = d->burnProcess->readLine();
                    } else {
                        line = d->burnProcess->read(peek.indexOf('\r') + 1);
                    }
                    line = line.trimmed();

                    qDebug() << line;

                    if (line.startsWith("Blanking time")) {
                        d->description = tr("Preparing to restore");
                        emit descriptionChanged(d->description);

                        d->progress = 0;
                        emit progressChanged(d->progress);


                        d->totalProgress = 0;
                        emit totalProgressChanged(d->totalProgress);
                    } else if (line.startsWith("Blanking")) {
                        d->description = tr("Erasing Disc");
                        emit descriptionChanged(d->description);
                    } else if (line.startsWith("Track")) {
                        QStringList parts = line.split(" ", Qt::SkipEmptyParts);
                        if (parts.length() >= 5 && parts.at(3) == "of") {
                            if (parts.length() >= 12) {
                                d->description = tr("Burning Image (%1)\n%2 of %3").arg(parts.at(11)).arg(QLocale().formattedDataSize(parts.at(2).toULongLong() * 1048576)).arg(QLocale().formattedDataSize(parts.at(4).toULongLong() * 1048576));
                                emit descriptionChanged(d->description);
                            } else {
                                d->description = tr("Burning Image");
                                emit descriptionChanged(d->description);
                            }

                            d->totalProgress = parts.at(4).toInt();
                            emit totalProgressChanged(d->totalProgress);

                            d->progress = parts.at(2).toInt();
                            emit progressChanged(d->progress);
                        }
                    } else if (line.startsWith("Fixating")) {
                        d->description = tr("Finalizing Disc");
                        emit descriptionChanged(d->description);

                        d->progress = 0;
                        emit progressChanged(d->progress);


                        d->totalProgress = 0;
                        emit totalProgressChanged(d->totalProgress);
                    }

                    peek = d->burnProcess->peek(1024);
                }
            });
            connect(d->burnProcess, &QProcess::bytesWritten, this, &RestoreOpticalJob::writeBlock);

            QStringList args;
            args.append("-v");

            if (!d->disk->interface<BlockInterface>()->drive()->opticalBlank()) {
                args.append("blank=fast");
            }

            args.append(QStringLiteral("dev=%1").arg(d->disk->interface<BlockInterface>()->blockName()));
            args.append("gracetime=0");
            args.append(QStringLiteral("tsize=%1").arg(d->dataSize));
            args.append("-");

            d->burnProcess->start("cdrecord", args);

            writeBlock();

            break;
        }
        case 2: {
            d->description = tr("Ejecting Disc");
            emit descriptionChanged(d->description);

            //Eject the disc
            d->disk->interface<BlockInterface>()->drive()->eject()->then([ = ] {
                d->state = Finished;
                emit stateChanged(Finished);

                d->description = tr("Restore Complete");
                emit descriptionChanged(d->description);

                tNotification* notification = new tNotification();
                notification->setSummary(tr("Restored Disc"));
                notification->setText(tr("The disc in %1 has been restored.").arg(displayName));
                notification->post();
            });
            break;
        }
    }
}

void RestoreOpticalJob::writeBlock() {
    if (!d->burnProcess->isWritable()) return;

    while (d->burnProcess->bytesToWrite() < 16384) {
        QByteArray buf = d->source->read(2048);
        d->writtenBytes += buf.length();

        if (d->burnProcess->isWritable()) {
            d->burnProcess->write(buf);
        }

        if (d->writtenBytes >= d->dataSize) {
            d->burnProcess->closeWriteChannel();
            return;
        }
    }
}

quint64 RestoreOpticalJob::progress() {
    return d->progress;
}

quint64 RestoreOpticalJob::totalProgress() {
    return d->totalProgress;
}

tJob::State RestoreOpticalJob::state() {
    return d->state;
}

QWidget* RestoreOpticalJob::makeProgressWidget() {
    return new RestoreOpticalJobProgress(this);
}
