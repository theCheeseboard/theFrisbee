#ifndef VOLUMEGROUP_H
#define VOLUMEGROUP_H

#include "udisksinterface.h"
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

    signals:
        void lvsChanged();
        void pvsChanged();

    private:
        VolumeGroupPrivate* d;
};

#endif // VOLUMEGROUP_H
