#ifndef VOLUMEGROUPPVMODEL_H
#define VOLUMEGROUPPVMODEL_H

#include <QAbstractListModel>

class VolumeGroup;
struct VolumeGroupPvModelPrivate;
class VolumeGroupPvModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit VolumeGroupPvModel(VolumeGroup* vg, QObject* parent = nullptr);
        ~VolumeGroupPvModel();

        enum Roles {
            PvRole = Qt::UserRole
        };

        // Basic functionality:
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    private:
        VolumeGroupPvModelPrivate* d;
        void updatePvs();
};

#endif // VOLUMEGROUPPVMODEL_H
