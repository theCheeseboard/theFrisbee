#include "volumegrouppvmodel.h"

#include "DriveObjects/diskobject.h"
#include "DriveObjects/volumegroup.h"

struct VolumeGroupPvModelPrivate {
        VolumeGroup* vg;
        QList<DiskObject*> pvs;
};

VolumeGroupPvModel::VolumeGroupPvModel(VolumeGroup* vg, QObject* parent) :
    QAbstractListModel(parent) {
    d = new VolumeGroupPvModelPrivate();
    d->vg = vg;

    connect(vg, &VolumeGroup::pvsChanged, this, &VolumeGroupPvModel::updatePvs);
    this->updatePvs();
}

VolumeGroupPvModel::~VolumeGroupPvModel() {
    delete d;
}

int VolumeGroupPvModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;

    return d->pvs.count();
}

QVariant VolumeGroupPvModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    auto pv = d->pvs.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return pv->displayName();
        case PvRole:
            return QVariant::fromValue(pv);
    }

    return {};
}

void VolumeGroupPvModel::updatePvs() {
    this->beginResetModel();
    d->pvs = d->vg->pvs().toList();
    this->endResetModel();
}
