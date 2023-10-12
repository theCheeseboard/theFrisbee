#include "volumegrouplvmodel.h"

#include "DriveObjects/logicalvolume.h"
#include "DriveObjects/volumegroup.h"

struct VolumeGroupLvModelPrivate {
    VolumeGroup* vg;
    QList<LogicalVolume*> lvs;
    bool showAddLvItem = true;
};

VolumeGroupLvModel::VolumeGroupLvModel(VolumeGroup *vg, QObject *parent)
    : QAbstractListModel(parent)
{
    d = new VolumeGroupLvModelPrivate();
    d->vg = vg;
    this->updateLvs();
}

VolumeGroupLvModel::~VolumeGroupLvModel()
{
    delete d;
}

int VolumeGroupLvModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;

    return d->lvs.count() + (d->showAddLvItem ? 1 : 0);
}

QVariant VolumeGroupLvModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    if (d->lvs.count() == index.row() && d->showAddLvItem) {
        switch (role) {
        case Qt::DisplayRole:
            return tr("New Logical Volume");
        case Qt::DecorationRole:
            return QIcon::fromTheme("list-add");
        }
        return {};
    }

    auto lv = d->lvs.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return lv->name();
        case LvRole:
            return QVariant::fromValue(lv);
    }

        return {};
}

void VolumeGroupLvModel::updateLvs()
{
    this->beginResetModel();
    d->lvs = d->vg->lvs().toList();
    this->endResetModel();
}
