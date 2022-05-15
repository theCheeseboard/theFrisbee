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
#include "loopinterface.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

struct LoopInterfacePrivate {
        QDBusObjectPath path;
        QByteArray backingFile;
};

LoopInterface::LoopInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new LoopInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("BackingFile", [=](QVariant value) {
        d->backingFile = value.toByteArray();
        emit backingFileChanged();
    });
}

LoopInterface::~LoopInterface() {
    delete d;
}

QString LoopInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Loop");
}

LoopInterface::Interfaces LoopInterface::interfaceType() {
    return Loop;
}

QByteArray LoopInterface::backingFile() {
    return d->backingFile;
}

tPromise<void>* LoopInterface::detach() {
    return TPROMISE_CREATE_NEW_THREAD(void, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
        message.setArguments({QVariantMap()});
        QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(QDBusConnection::systemBus().asyncCall(message));
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
            if (watcher->isError()) {
                rej(watcher->error().message());
            } else {
                res();
            }
        });
    });
}

bool LoopInterface::isAvailable() {
    return !d->backingFile.startsWith('\0');
}
