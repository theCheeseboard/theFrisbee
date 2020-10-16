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
#include "filesysteminterface.h"

struct FilesystemInterfacePrivate {
    QDBusObjectPath path;

    QByteArrayList mountPoints;
};

FilesystemInterface::FilesystemInterface(QDBusObjectPath path, QObject* parent) : DiskInterface(path, interfaceName(), parent) {
    d = new FilesystemInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("MountPoints", [ = ](QVariant value) {
        QDBusArgument mountPoints = value.value<QDBusArgument>();
        d->mountPoints.clear();
        mountPoints >> d->mountPoints;
    });
}

FilesystemInterface::~FilesystemInterface() {
    delete d;
}

QString FilesystemInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Filesystem");
}

DiskInterface::Interfaces FilesystemInterface::interfaceType() {
    return DiskInterface::Filesystem;
}

QByteArrayList FilesystemInterface::mountPoints() {
    return d->mountPoints;
}

tPromise<void>* FilesystemInterface::mount() {
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Mount");
        message.setArguments({QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

tPromise<void>* FilesystemInterface::unmount() {
    return tPromise<void>::runOnSameThread([ = ](tPromiseFunctions<void>::SuccessFunction res, tPromiseFunctions<void>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Unmount");
        message.setArguments({QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [ = ] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}
