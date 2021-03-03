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
#include "diskmodel.h"

#include <driveobjectmanager.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/partitiontableinterface.h>
#include <DriveObjects/encryptedinterface.h>
#include <DriveObjects/partitioninterface.h>

struct DiskModelPrivate {
    QMap<DiskObject*, QModelIndex> indices;
};

DiskModel::DiskModel(QObject* parent)
    : QAbstractItemModel(parent) {
    d = new DiskModelPrivate();

    connect(DriveObjectManager::instance(), &DriveObjectManager::rootDisksChanged, this, &DiskModel::updateList);
}

DiskModel::~DiskModel() {
    delete d;
}

QModelIndex DiskModel::index(int row, int column, const QModelIndex& parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();

    if (!parent.isValid()) {
        QModelIndex index = createIndex(row, column, DriveObjectManager::rootDisks().value(row));
        d->indices.insert(static_cast<DiskObject*>(index.internalPointer()), index);
        return index;
    } else {
        DiskObject* parentDisk = static_cast<DiskObject*>(parent.internalPointer());
        PartitionTableInterface* partitionTable = parentDisk->interface<PartitionTableInterface>();
        EncryptedInterface* encrypted = parentDisk->interface<EncryptedInterface>();
        if (partitionTable) {
            QModelIndex index = createIndex(row, column, partitionTable->partitions().value(row));
            d->indices.insert(static_cast<DiskObject*>(index.internalPointer()), index);
            return index;
        } else if (encrypted && encrypted->cleartextDevice()) {
            QModelIndex index = createIndex(row, column, encrypted->cleartextDevice());
            d->indices.insert(static_cast<DiskObject*>(index.internalPointer()), index);
            return index;
        }
    }

    return QModelIndex();
}

QModelIndex DiskModel::parent(const QModelIndex& index) const {
    DiskObject* candidateDisk = nullptr;
    DiskObject* disk = static_cast<DiskObject*>(index.internalPointer());
    PartitionInterface* partition = disk->interface<PartitionInterface>();
    if (disk->interface<BlockInterface>()->cryptoBackingDevice()) {
        candidateDisk = disk->interface<BlockInterface>()->cryptoBackingDevice();
    } else if (partition && partition->parentTable()) {
        candidateDisk = disk->interface<PartitionInterface>()->parentTable();
    }

    QList<QModelIndex> parents;
    parents.append(QModelIndex());

    while (!parents.isEmpty()) {
        QModelIndex checkingIndex = parents.takeFirst();
        if (checkingIndex.isValid()) {
            DiskObject* disk = static_cast<DiskObject*>(checkingIndex.internalPointer());
            if (disk == candidateDisk) return checkingIndex;
        }

        for (int row = 0; row < rowCount(checkingIndex); row++) {
            parents.append(this->index(row, 0, checkingIndex));
        }
    }

    return QModelIndex();
}

int DiskModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        DiskObject* parentDisk = static_cast<DiskObject*>(parent.internalPointer());
        PartitionTableInterface* partitionTable = parentDisk->interface<PartitionTableInterface>();
        EncryptedInterface* encrypted = parentDisk->interface<EncryptedInterface>();
        if (partitionTable) {
            return partitionTable->partitions().count();
        } else if (encrypted) {
            if (encrypted->cleartextDevice()) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    return DriveObjectManager::rootDisks().count();
}

int DiskModel::columnCount(const QModelIndex& parent) const {
    return 1;
}

QVariant DiskModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return QVariant();

    DiskObject* object = static_cast<DiskObject*>(index.internalPointer());
    switch (role) {
        case Qt::DisplayRole:
            return object->displayName();
    }

    return QVariant();
}

void DiskModel::updateList() {
    beginResetModel();
    endResetModel();
//    emit dataChanged(index(0), index(rowCount()));
//    for (int row = 0; row < rowCount(); row++) {
//        emit dataChanged(index(0, 0, index(row)), index(rowCount(index(row)), 0, index(row)));
//    }
}
