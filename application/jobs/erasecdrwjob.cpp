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
#include "erasecdrwjob.h"

#include <QProcess>
#include <tnotification.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/filesysteminterface.h>
#include <DriveObjects/driveinterface.h>
#include <tlogger.h>
#include "progress/erasecdrwjobprogress.h"

struct EraseCdRwJobPrivate {
    DiskObject* disk;
    bool quick;

    QString discType;
    QString description;

    int stage = 0;

    EraseCdRwJob::State state = EraseCdRwJob::Processing;
};

EraseCdRwJob::EraseCdRwJob(DiskObject* disk, bool quick, QObject* parent) : tJob(parent) {
    d = new EraseCdRwJobPrivate();
    d->disk = disk;
    d->quick = quick;

    switch (disk->interface<BlockInterface>()->drive()->media()) {
        case DriveInterface::CdRw:
            d->discType = tr("CD-RW");
            break;
        case DriveInterface::DvdRw:
            d->discType = tr("DVD-RW");
            break;
        case DriveInterface::DvdPRw:
        case DriveInterface::DvdPRwDl:
            d->discType = tr("DVD+RW");
            break;
        case DriveInterface::BdRe:
            d->discType = tr("BD-RE");
            break;
        default:
            d->discType = tr("Unknown");
    }

    //Try to acquire the lock
    d->description = tr("Waiting for other jobs to finish");
    emit descriptionChanged(d->description);

    d->disk->lock()->then([ = ] {
        connect(this, &EraseCdRwJob::stateChanged, this, [ = ] {
            //Release the lock
            if (d->state == Finished || d->state == Failed) {
                d->disk->releaseLock();
            }
        });

        tInfo("OpticalErase") << "Erase Optical Operation Starts";
        runNextStage();
    });
}

EraseCdRwJob::~EraseCdRwJob() {
    delete d;
}

DiskObject* EraseCdRwJob::disk() {
    return d->disk;
}

QString EraseCdRwJob::description() {
    return d->description;
}

void EraseCdRwJob::runNextStage() {
    QString displayName = d->disk->displayName();

    d->stage++;
    switch (d->stage) {
        case 1: {
            //Check if the disc needs to be unmounted

            d->description = tr("Unmounting Disc");
            emit descriptionChanged(d->description);

            FilesystemInterface* fs = d->disk->interface<FilesystemInterface>();
            if (!fs) {
                //This disc does not have a filesystem so doesn't need to be unmounted
                runNextStage();
                return;
            }

            QByteArrayList mountPoints = fs->mountPoints();
            if (mountPoints.count() == 0) {
                //This disc is not mounted
                runNextStage();
                return;
            }

            //Unmount the disc
            fs->unmount()->then([ = ] {
                runNextStage();
            })->error([ = ](QString error) {
                //Bail out
                d->state = Failed;
                emit stateChanged(Failed);

                d->description = tr("Failed to erase %1").arg(d->discType);
                emit descriptionChanged(d->description);

                tCritical("OpticalErase") << "Operation failed: Could not unmount";

                tNotification* notification = new tNotification();
                notification->setSummary(tr("Couldn't Erase Disc"));
                notification->setText(tr("The disc in %1 could not be erased.").arg(displayName));
                notification->post();
            });

            break;
        }
        case 2: {
            d->description = tr("Erasing %1").arg(d->discType);
            emit descriptionChanged(d->description);

            //Erase the disc
            QProcess* proc = new QProcess();
            proc->setProcessChannelMode(QProcess::MergedChannels);
            connect(proc, &QProcess::readyRead, this, [ = ] {
                QByteArray peek = proc->peek(1024);
                while (proc->canReadLine() || peek.contains('\r')) {
                    QString line;
                    if (proc->canReadLine()) {
                        line = proc->readLine();
                    } else {
                        line = proc->read(peek.indexOf('\r') + 1);
                    }
                    line = line.trimmed();

                    tDebug("OpticalErase") << line;
                    peek = proc->peek(1024);
                }
            });
            connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [ = ](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitStatus);
                if (exitCode == 0) {
                    runNextStage();
                } else {
                    d->state = Failed;
                    emit stateChanged(Failed);

                    d->description = tr("Failed to erase %1").arg(d->discType);
                    emit descriptionChanged(d->description);

                    tCritical("OpticalErase") << "Operation failed: cdrecord returned with exit code" << exitCode;

                    tNotification* notification = new tNotification();
                    notification->setSummary(tr("Couldn't Erase Disc"));
                    notification->setText(tr("The disc in %1 could not be erased.").arg(displayName));
                    notification->post();
                }
                emit progressChanged(1);
                emit totalProgressChanged(1);
                proc->deleteLater();
            });

            QStringList cdrecordArgs = {"-v", QStringLiteral("blank=%1").arg(d->quick ? "fast" : "all"), QStringLiteral("dev=%1").arg(d->disk->interface<BlockInterface>()->blockName()), "gracetime=0"};
            tDebug("OpticalErase") << "Starting cdrecord with arguments" << cdrecordArgs;
            proc->start("cdrecord", cdrecordArgs);
            break;
        }
        case 3: {
            d->description = tr("Ejecting Disc");
            emit descriptionChanged(d->description);

            //Eject the disc
            d->disk->interface<BlockInterface>()->drive()->eject()->then([ = ] {
                d->state = Finished;
                emit stateChanged(Finished);

                d->description = tr("%1 Erased").arg(d->discType);
                emit descriptionChanged(d->description);

                tInfo("OpticalErase") << "Operation Success";

                tNotification* notification = new tNotification();
                notification->setSummary(tr("Erased Disc"));
                notification->setText(tr("The disc in %1 has been erased.").arg(displayName));
                notification->post();
            });
            break;
        }
    }
}


quint64 EraseCdRwJob::progress() {
    return d->state != Processing; //0 if processing, 1 if not
}

quint64 EraseCdRwJob::totalProgress() {
    return d->state != Processing;
}

EraseCdRwJob::State EraseCdRwJob::state() {
    return d->state;
}

QWidget* EraseCdRwJob::makeProgressWidget() {
    return new EraseCdRwJobProgress(this);
}
