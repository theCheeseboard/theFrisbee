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
#include "blockinterface.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusUnixFileDescriptor>
#include <QDBusPendingCall>
#include <QMutex>
#include "driveobjectmanager.h"
#include <unistd.h>

struct BlockInterfacePrivate {
    QDBusObjectPath path;

    QString name;
    quint64 size;
    QDBusObjectPath drive;
    QString idLabel;
    bool hintIgnore;
    bool hintSystem;
    QDBusObjectPath cryptoBackingDevice;
};

BlockInterface::BlockInterface(QDBusObjectPath path, QObject* parent) : DiskInterface(path, interfaceName(), parent) {
    d = new BlockInterfacePrivate();
    d->path = path;

    bindPropertyUpdater("Device", [ = ](QVariant value) {
        d->name = value.toString();
    });
    bindPropertyUpdater("Size", [ = ](QVariant value) {
        d->size = value.toULongLong();
        emit sizeChanged(d->size);
    });
    bindPropertyUpdater("Drive", [ = ](QVariant value) {
        d->drive = value.value<QDBusObjectPath>();
    });
    bindPropertyUpdater("IdLabel", [ = ](QVariant value) {
        d->idLabel = value.toString();
    });
    bindPropertyUpdater("HintIgnore", [ = ](QVariant value) {
        d->hintIgnore = value.toBool();
    });
    bindPropertyUpdater("HintSystem", [ = ](QVariant value) {
        d->hintSystem = value.toBool();
    });
    bindPropertyUpdater("CryptoBackingDevice", [ = ](QVariant value) {
        d->cryptoBackingDevice = value.value<QDBusObjectPath>();
    });
}

BlockInterface::~BlockInterface() {
    delete d;
}

QString BlockInterface::interfaceName() {
    return QStringLiteral("org.freedesktop.UDisks2.Block");
}


BlockInterface::Interfaces BlockInterface::interfaceType() {
    return DiskInterface::Block;
}

QString BlockInterface::blockName() {
    return d->name;
}

quint64 BlockInterface::size() {
    return d->size;
}

tPromise<void>* BlockInterface::triggerReload() {
    return TPROMISE_CREATE_SAME_THREAD(void, {
        QProcess* proc = new QProcess();
        connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [ = ](int exitCode, QProcess::ExitStatus status) {
            Q_UNUSED(status)
            if (exitCode == 0) {
                res();
            } else {
                rej("Process returned code " + QString::number(exitCode));
            }
        });
        proc->start("pkexec", {"/usr/lib/libthefrisbee/trigger-uevent.sh", d->name});
    });
}

DriveInterface* BlockInterface::drive() {
    return DriveObjectManager::driveForPath(d->drive);
}

DiskObject* BlockInterface::cryptoBackingDevice() {
    return DriveObjectManager::diskForPath(d->cryptoBackingDevice);
}

tPromise<void>* BlockInterface::format(QString type, QVariantMap options) {
    //TODO: Different handling if this is an optical device

    return TPROMISE_CREATE_SAME_THREAD(void, {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "Format");
        message.setArguments({type, options});

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

tPromise<QIODevice*>* BlockInterface::open(BlockInterface::OpenMode mode, QVariantMap options) {
    QFile::OpenMode fileOpenMode;
    QString openMode;
    switch (mode) {
        case BlockInterface::Read:
            openMode = "r";
            fileOpenMode = QFile::ReadOnly;
            break;
        case BlockInterface::Write:
            fileOpenMode = QFile::WriteOnly;
            openMode = "w";
            break;
        case BlockInterface::ReadWrite:
            fileOpenMode = QFile::ReadWrite;
            openMode = "rw";
            break;
    }

    return tPromise<QIODevice*>::runOnNewThread([ = ](tPromiseFunctions<QIODevice*>::SuccessFunction res, tPromiseFunctions<QIODevice*>::FailureFunction rej) {
        QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.UDisks2", d->path.path(), interfaceName(), "OpenDevice");
        message.setArguments({openMode, options});

        QDBusMessage reply = QDBusConnection::systemBus().call(message);
        if (reply.type() == QDBusMessage::ErrorMessage) {
            rej("Could not open");
        } else {
            QDBusUnixFileDescriptor fdArg = reply.arguments().first().value<QDBusUnixFileDescriptor>();
            int fd = dup(fdArg.fileDescriptor());

            QFile* file = new QFile();
            if (file->open(fd, fileOpenMode | QIODevice::Unbuffered, QFile::AutoCloseHandle)) {
                res(file);
            } else {
                file->deleteLater();

                close(fd);
                rej("Could not open file descriptor");
            }
        }
    });
}

QString BlockInterface::idLabel() {
    return d->idLabel;
}

bool BlockInterface::hintIgnore() {
    return d->hintIgnore;
}

bool BlockInterface::hintSystem() {
    return d->hintSystem;
}
