#ifndef UDISKSINTERFACE_H
#define UDISKSINTERFACE_H

#include <QDBusObjectPath>
#include <QObject>

class DiskObject;
class DriveInterface;
class DriveObjectManager;
struct UdisksInterfacePrivate;
class UdisksInterface : public QObject {
        Q_OBJECT
    public:
        explicit UdisksInterface(QDBusObjectPath path, QString interface, QObject* parent = nullptr);
        ~UdisksInterface();

    protected:
        friend DiskObject;
        friend DriveInterface;
        friend DriveObjectManager;
        void updateProperties(QVariantMap properties);
        void bindPropertyUpdater(QString property, std::function<void(QVariant)> updater);

    signals:

    private slots:
        void propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties);

    private:
        UdisksInterfacePrivate* d;
};

#endif // UDISKSINTERFACE_H
