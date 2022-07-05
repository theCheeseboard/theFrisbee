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

#include <QCoroDBusPendingCall>
#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <frisbeeexception.h>

struct FilesystemInterfacePrivate {
        QDBusObjectPath path;

        QByteArrayList mountPoints;
};

FilesystemInterface::FilesystemInterface(QDBusObjectPath path, QObject* parent) :
    DiskInterface(path, interfaceName(), parent) {
    d = new FilesystemInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("MountPoints", [this](QVariant value) {
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

QCoro::Task<> FilesystemInterface::mount() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Mount");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<> FilesystemInterface::unmount() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Unmount");
    message.setArguments({QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}

QCoro::Task<bool> FilesystemInterface::check(QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Check");
    message.setArguments({options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());

    co_return reply.arguments().first().toBool();
}

QCoro::Task<bool> FilesystemInterface::repair(QVariantMap options) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Repair");
    message.setArguments({options});
    auto call = QDBusConnection::systemBus().asyncCall(message);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());

    co_return reply.arguments().first().toBool();
}

QCoro::Task<> FilesystemInterface::resize(quint64 size) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Resize");
    message.setArguments({size, QVariantMap()});
    auto call = QDBusConnection::systemBus().asyncCall(message, 86400000);
    auto reply = co_await call;
    if (call.isError()) throw FrisbeeException(call.error().message());
}
