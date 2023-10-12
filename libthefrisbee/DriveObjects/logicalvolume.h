#ifndef LOGICALVOLUME_H
#define LOGICALVOLUME_H

#include "udisksinterface.h"
#include <QCoroTask>

struct LogicalVolumePrivate;
class VolumeGroup;
class LogicalVolume : public UdisksInterface {
        Q_OBJECT
    public:
        explicit LogicalVolume(QDBusObjectPath path, QObject* parent = nullptr);
        ~LogicalVolume();

        static QString interfaceName();

        QString name();
        VolumeGroup* vg();
        DiskObject* block();

        QCoro::Task<> deleteLogicalVolume(QVariantMap options);

    private:
        LogicalVolumePrivate* d;
};

#endif // LOGICALVOLUME_H
