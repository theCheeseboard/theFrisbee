/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "diskinterface.h"

#include <QDBusConnection>

struct DiskInterfacePrivate {
    QString interface;
    QMap<QString, std::function<void (QVariant)>> updaters;
};

DiskInterface::DiskInterface(QDBusObjectPath path, QString interface, QObject* parent) : QObject(parent) {
    d = new DiskInterfacePrivate();
    d->interface = interface;

    QDBusConnection::systemBus().connect("org.freedesktop.UDisks2", path.path(), "org.freedesktop.DBus.Properties", "PropertiesChanged", this, SLOT(propertiesChanged(QString, QVariantMap, QStringList)));
}

DiskInterface::~DiskInterface() {
    delete d;
}

void DiskInterface::updateProperties(QVariantMap properties) {
    for (QString property : properties.keys()) {
        if (d->updaters.contains(property)) d->updaters.value(property)(properties.value(property));
    }
}

void DiskInterface::bindPropertyUpdater(QString property, std::function<void (QVariant)> updater) {
    d->updaters.insert(property, updater);
}

void DiskInterface::propertiesChanged(QString interface, QVariantMap changedProperties, QStringList invalidatedProperties) {
    Q_UNUSED(invalidatedProperties);
    if (interface == d->interface) updateProperties(changedProperties);
}

