#ifndef VOLUMEGROUP_H
#define VOLUMEGROUP_H

#include "udisksinterface.h"
#include <QCoroTask>
#include <ranges/trange.h>

struct VolumeGroupPrivate;
class LogicalVolume;
class DiskObject;
class VolumeGroup : public UdisksInterface {
        Q_OBJECT
    public:
        explicit VolumeGroup(QDBusObjectPath path, QObject* parent = nullptr);
        ~VolumeGroup();

        static QString interfaceName();
        tRange<LogicalVolume*> lvs();
        tRange<DiskObject*> pvs();

        QString name();
        quint64 size();
        quint64 freeSize();

        QCoro::Task<> deleteVg(bool wipe, QVariantMap options);
        QCoro::Task<> addDevice(DiskObject* block, QVariantMap options);
        QCoro::Task<LogicalVolume*> createPlainVolume(QString name, quint64 size, QVariantMap options);
        QCoro::Task<> emptyDevice(DiskObject* block, QVariantMap options);
        QCoro::Task<> removeDevice(DiskObject* block, bool wipe, QVariantMap options);

    signals:
        void lvsChanged();
        void pvsChanged();

    private:
        VolumeGroupPrivate* d;
};

#endif // VOLUMEGROUP_H
