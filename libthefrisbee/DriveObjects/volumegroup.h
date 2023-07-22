#ifndef VOLUMEGROUP_H
#define VOLUMEGROUP_H

#include "udisksinterface.h"

struct VolumeGroupPrivate;
class VolumeGroup : public UdisksInterface {
        Q_OBJECT
    public:
        explicit VolumeGroup(QDBusObjectPath path, QObject* parent = nullptr);
        ~VolumeGroup();

        static QString interfaceName();

    private:
        VolumeGroupPrivate* d;
};

#endif // VOLUMEGROUP_H
