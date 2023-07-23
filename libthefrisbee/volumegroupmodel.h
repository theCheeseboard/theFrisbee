#ifndef VOLUMEGROUPMODEL_H
#define VOLUMEGROUPMODEL_H

#include <QAbstractListModel>

struct VolumeGroupModelPrivate;
class VolumeGroupModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit VolumeGroupModel(QObject* parent = nullptr);
        ~VolumeGroupModel();

        enum Roles {
            VolumeGroupRole = Qt::UserRole
        };

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        void setShowAddButton(bool showAddButton);

    private:
        VolumeGroupModelPrivate* d;
};

#endif // VOLUMEGROUPMODEL_H
