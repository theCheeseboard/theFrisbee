#include "udisksinterface.h"

#include <QDBusConnection>

struct UdisksInterfacePrivate {
        QString interface;
        QMap<QString, std::function<void(QVariant)>> updaters;
};

UdisksInterface::UdisksInterface(QDBusObjectPath path, QString interface, QObject* parent) :
    QObject{parent} {
    d = new UdisksInterfacePrivate();
    d->interface = interface;

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", path.path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

UdisksInterface::~UdisksInterface() {
    delete d;
}

void UdisksInterface::updateProperties(QVariantMap properties) {
    for (QString property : properties.keys()) {
        if (d->updaters.contains(property)) d->updaters.value(property)(properties.value(property));
    }
}

void UdisksInterface::bindPropertyUpdater(QString property, std::function<void(QVariant)> updater) {
    d->updaters.insert(property, updater);
}

void UdisksInterface::propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties) {
    Q_UNUSED(invalidatedProperties);
    if (interface == d->interface) updateProperties(changedProperties);
}
