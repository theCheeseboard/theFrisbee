#include "volumegroupmodel.h"

#include "DriveObjects/volumegroup.h"
#include "driveobjectmanager.h"

struct VolumeGroupModelPrivate {
        bool showAddButton = false;
};

VolumeGroupModel::VolumeGroupModel(QObject* parent) :
    QAbstractListModel(parent) {
    d = new VolumeGroupModelPrivate();

    connect(DriveObjectManager::instance(), &DriveObjectManager::volumeGroupAdded, this, [this] {
        emit dataChanged(index(0), index(rowCount()));
    });
    connect(DriveObjectManager::instance(), &DriveObjectManager::volumeGroupRemoved, this, [this] {
        emit dataChanged(index(0), index(rowCount()));
    });
}

VolumeGroupModel::~VolumeGroupModel() {
    delete d;
}

int VolumeGroupModel::rowCount(const QModelIndex& parent) const {
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid()) {
        return 0;
    }

    return DriveObjectManager::volumeGroups().length() + (d->showAddButton ? 1 : 0);
}

QVariant VolumeGroupModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    if (d->showAddButton && index.row() == DriveObjectManager::volumeGroups().count()) {
        switch (role) {
            case Qt::DisplayRole:
                return tr("New Volume Group");
            case Qt::DecorationRole:
                return QIcon::fromTheme("list-add");
        }
        return {};
    }

    if (index.row() >= DriveObjectManager::volumeGroups().count()) return {};
    auto vg = DriveObjectManager::volumeGroups().at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return vg->name();
        case VolumeGroupRole:
            return QVariant::fromValue(vg);
    }

    return {};
}

void VolumeGroupModel::setShowAddButton(bool showAddButton) {
    d->showAddButton = showAddButton;
    emit dataChanged(index(0), index(rowCount()));
}
