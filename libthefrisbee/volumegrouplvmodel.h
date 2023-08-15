#ifndef VOLUMEGROUPLVMODEL_H
#define VOLUMEGROUPLVMODEL_H

#include <QAbstractListModel>

class VolumeGroup;
struct VolumeGroupLvModelPrivate;
class VolumeGroupLvModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit VolumeGroupLvModel(VolumeGroup* vg, QObject *parent = nullptr);
    ~VolumeGroupLvModel();

    enum Roles {
        LvRole = Qt::UserRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    VolumeGroupLvModelPrivate* d;

    void updateLvs();
};

#endif // VOLUMEGROUPLVMODEL_H
