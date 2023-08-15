#ifndef VOLUMEGROUP_H
#define VOLUMEGROUP_H

#include "udisksinterface.h"
#include <ranges/trange.h>

struct VolumeGroupPrivate;
class LogicalVolume;
class VolumeGroup : public UdisksInterface {
        Q_OBJECT
    public:
        explicit VolumeGroup(QDBusObjectPath path, QObject* parent = nullptr);
        ~VolumeGroup();

        static QString interfaceName();
        tRange<LogicalVolume*> lvs();

        QString name();

    signals:
        void lvsChanged();

    private:
        VolumeGroupPrivate* d;
};

#endif // VOLUMEGROUP_H
