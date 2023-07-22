#ifndef LOGICALVOLUME_H
#define LOGICALVOLUME_H

#include "udisksinterface.h"

struct LogicalVolumePrivate;
class LogicalVolume : public UdisksInterface {
        Q_OBJECT
    public:
        explicit LogicalVolume(QDBusObjectPath path, QObject* parent = nullptr);
        ~LogicalVolume();

        static QString interfaceName();

    private:
        LogicalVolumePrivate* d;
};

#endif // LOGICALVOLUME_H
