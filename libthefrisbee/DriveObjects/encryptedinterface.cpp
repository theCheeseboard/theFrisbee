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
#include "encryptedinterface.h"

#include "driveobjectmanager.h"
#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>

struct EncryptedInterfacePrivate {
        QDBusObjectPath path;

        QDBusObjectPath cleartextDevice;
};

EncryptedInterface::EncryptedInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new EncryptedInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("CleartextDevice", [this](QVariant value) {
        d->cleartextDevice = value.value<QDBusObjectPath>();
    });
}

EncryptedInterface::~EncryptedInterface() {
    delete d;
}

QString EncryptedInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Encrypted");
}

DiskObject* EncryptedInterface::cleartextDevice() {
    return DriveObjectManager::diskForPath(d->cleartextDevice);
}

QCoro::Task<DiskObject*> EncryptedInterface::unlock(QString passphrase, QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Unlock");
    message.setArguments({passphrase, options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());

    QDBusObjectPath objectPath = reply.arguments().first().value<QDBusObjectPath>();
    co_return DriveObjectManager::diskForPath(objectPath);
}

QCoro::Task<> EncryptedInterface::lock(QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Lock");
    message.setArguments({options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

DiskInterface::Interfaces EncryptedInterface::interfaceType() {
    return Encrypted;
}
