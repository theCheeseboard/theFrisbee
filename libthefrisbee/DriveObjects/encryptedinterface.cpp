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

struct EncryptedInterfacePrivate {
    QDBusObjectPath path;

    QDBusObjectPath cleartextDevice;
};

EncryptedInterface::EncryptedInterface(QDBusObjectPath path, QObject* parent) : DiskInterface(path, interfaceName(), parent) {
    d = new EncryptedInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("CleartextDevice", [ = ](QVariant value) {
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

tPromise<DiskObject*>* EncryptedInterface::unlock(QString passphrase, QVariantMap options) {
    return TPROMISE_CREATE_SAME_THREAD(DiskObject*, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Unlock");
        message.setArguments({passphrase, options});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                QDBusObjectPath objectPath = watcher->reply().arguments().first().value<QDBusObjectPath>();
                res(DriveObjectManager::diskForPath(objectPath));
            }
            watcher->deleteLater();
        });
    });
}

tPromise<void>* EncryptedInterface::lock(QVariantMap options) {
    return TPROMISE_CREATE_SAME_THREAD(void, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Lock");
        message.setArguments({options});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
            watcher->deleteLater();
        });
    });
}

DiskInterface::Interfaces EncryptedInterface::interfaceType() {
    return Encrypted;
}
