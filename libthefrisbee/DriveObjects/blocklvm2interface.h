#ifndef BLOCKLVM2INTERFACE_H
#define BLOCKLVM2INTERFACE_H

#include "diskinterface.h"

class LogicalVolume;
struct BlockLvm2InterfacePrivate;
class BlockLvm2Interface : public DiskInterface {
        Q_OBJECT
    public:
        explicit BlockLvm2Interface(QDBusObjectPath path, QObject* parent = nullptr);
        ~BlockLvm2Interface();

        static QString interfaceName();

        LogicalVolume* logicalVolume();

    signals:

    private:
        BlockLvm2InterfacePrivate* d;

        // DiskInterface interface
    public:
        Interfaces interfaceType();
};

#endif // BLOCKLVM2INTERFACE_H
