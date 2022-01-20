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
#include "editpartitionjob.h"

#include <tnotification.h>
#include <driveobjectmanager.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/partitiontableinterface.h>
#include <DriveObjects/partitioninterface.h>
#include <tlogger.h>
#include "progress/editpartitionjobprogress.h"
#include "partitioninformation.h"
#include "components/partitionvisualisation.h"

struct EditPartitionJobPrivate {
    quint64 progress = 0;
    quint64 totalProgress = 0;
    QString description;
    tJob::State state = tJob::Processing;

    QList<PartitionPopover::PartitionOperation> operations;
    DiskObject* disk;
    QString displayName;
};

EditPartitionJob::EditPartitionJob(QList<PartitionPopover::PartitionOperation> operations, DiskObject* disk, QObject* parent) : tJob(parent) {
    d = new EditPartitionJobPrivate();
    d->operations = operations;
    d->disk = disk;
    d->displayName = d->disk->displayName();

    //Try to acquire the lock
    d->description = tr("Waiting for other jobs to finish");
    emit descriptionChanged(d->description);

    d->disk->lock()->then([ = ] {
        connect(this, &EditPartitionJob::stateChanged, this, [ = ] {
            //Release the lock
            if (d->state == Finished || d->state == Failed) {
                d->disk->releaseLock();
            }
        });

        d->totalProgress = d->operations.length();
        emit totalProgressChanged(d->totalProgress);

        d->progress = -1;

        tInfo("EditPartitionJob") << "Partition Operation Starts";
        processNextOperation();
    });
}

EditPartitionJob::~EditPartitionJob() {
    delete d;
}

DiskObject* EditPartitionJob::disk() {
    return d->disk;
}

QString EditPartitionJob::description() {
    return d->description;
}

QString EditPartitionJob::displayName() {
    return d->displayName;
}

void EditPartitionJob::processNextOperation() {
    emit progressChanged(++d->progress);

    if (d->operations.isEmpty()) {
        d->state = Finished;
        emit stateChanged(Finished);

        d->description = tr("Partitioning Complete");
        emit descriptionChanged(d->description);

        tInfo("EditPartitionJob") << "Operation Success";

        tNotification* notification = new tNotification();
        notification->setSummary(tr("Partitioning Operations Complete"));
        notification->setText(tr("Changes to %1 have been applied.").arg(d->displayName));
        notification->post();
        return;
    }

    PartitionTableInterface* table = d->disk->interface<PartitionTableInterface>();
    PartitionPopover::PartitionOperation operation = d->operations.takeFirst();
    if (operation.type == "new") {
        //Create a new partition
        QString type = operation.data.value("type").toString();
        QString partType = PartitionInformation::partitionType(type, table->type());
        QString formatType = PartitionInformation::formatType(type);

        QString name = operation.data.value("name").toString();
        if (table->type() == "dos") name = ""; //dos table type does not support names

        d->description = tr("Creating partition %1").arg(QLocale().quoteString(operation.data.value("name").toString()));
        emit descriptionChanged(d->description);

        tDebug("EditPartitionJob") << "Creating partition:";
        tDebug("EditPartitionJob") << "Name: " << operation.data.value("name").toString();
        tDebug("EditPartitionJob") << "Partition Type: " << partType;
        tDebug("EditPartitionJob") << "Format Type: " << formatType;

        //TODO: support empty type

        table->createPartitionAndFormat(operation.data.value("offset").toULongLong(), operation.data.value("size").toULongLong(), partType, name, {}, formatType, {})->then([ = ](QDBusObjectPath newPartition) {
            DiskObject* newPartitionObject = DriveObjectManager::diskForPath(newPartition);
            PartitionVisualisation::mapPartition(operation.data.value("internalId").toULongLong(), newPartitionObject);

            tDebug("EditPartitionJob") << "Partition Created";

            processNextOperation();
        })->error([ = ](QString error) {
            tWarn("EditPartitionJob") << "Partition Create Failed";
            tWarn("EditPartitionJob") << error;

            fail();
        });
    } else if (operation.type == "delete") {
        DiskObject* partition = PartitionVisualisation::mappedDisk(operation.data.value("partition").toULongLong());
        if (!partition) {
            tWarn("EditPartitionJob") << "Partition does not refer to a UDisks object";
            fail();
            return;
        }

        tDebug("EditPartitionJob") << "Deleting partition:";
        tDebug("EditPartitionJob") << "Name: " << partition->displayName();

        d->description = tr("Deleting partition %1").arg(QLocale().quoteString(partition->displayName()));
        emit descriptionChanged(d->description);

        partition->interface<PartitionInterface>()->deletePartition()->then([ = ] {
            tDebug("EditPartitionJob") << "Partition Deleted";
            processNextOperation();
        })->error([ = ](QString error) {
            tWarn("EditPartitionJob") << "Partition Delete Failed";
            tWarn("EditPartitionJob") << error;

            fail();
        });
    }
}

void EditPartitionJob::fail() {
    d->state = Failed;
    emit stateChanged(Failed);

    d->description = tr("A partition operation failed.");
    emit descriptionChanged(d->description);

    tNotification* notification = new tNotification();
    notification->setSummary(tr("Partitioning Operations Failed"));
    notification->setText(tr("Changes to %1 could not be applied. The disk may be in an inconsistent state.").arg(d->displayName));
    notification->post();
}

quint64 EditPartitionJob::progress() {
    return d->progress;
}

quint64 EditPartitionJob::totalProgress() {
    return d->totalProgress;
}

tJob::State EditPartitionJob::state() {
    return d->state;
}

QWidget* EditPartitionJob::makeProgressWidget() {
    return new EditPartitionJobProgress(this);
}
