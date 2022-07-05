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

#include <QCoroDBusPendingCall>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>

struct LoopInterfacePrivate {
        QDBusObjectPath path;
        QByteArray backingFile;
};

LoopInterface::LoopInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new LoopInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("BackingFile", [this](QVariant value) {
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

QCoro::Task<> LoopInterface::detach() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Delete");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

bool LoopInterface::isAvailable() {
    return !d->backingFile.startsWith('\0');
}
