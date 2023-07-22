#ifndef PHYSICALVOLUMEINTERFACE_H
#define PHYSICALVOLUMEINTERFACE_H

#include "diskinterface.h"
#include <QObject>

class VolumeGroup;
struct PhysicalVolumeInterfacePrivate;
class PhysicalVolumeInterface : public DiskInterface {
        Q_OBJECT
    public:
        explicit PhysicalVolumeInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~PhysicalVolumeInterface();

        static QString interfaceName();

        VolumeGroup* volumeGroup();

    private:
        PhysicalVolumeInterfacePrivate* d;

        // DiskInterface interface
    public:
        Interfaces interfaceType();
};

#endif // PHYSICALVOLUMEINTERFACE_H
