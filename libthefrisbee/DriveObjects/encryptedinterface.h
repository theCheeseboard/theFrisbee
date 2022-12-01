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
#ifndef ENCRYPTEDINTERFACE_H
#define ENCRYPTEDINTERFACE_H

#include "diskinterface.h"
#include <QCoroTask>
#include <tpromise.h>

struct EncryptedInterfacePrivate;
class EncryptedInterface : public DiskInterface {
        Q_OBJECT
    public:
        explicit EncryptedInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~EncryptedInterface();

        static QString interfaceName();

        DiskObject* cleartextDevice();

        QCoro::Task<DiskObject*> unlock(QString passphrase, QVariantMap options = {});
        QCoro::Task<> lock(QVariantMap options = {});

    signals:

    private:
        EncryptedInterfacePrivate* d;

        // DiskInterface interface
    public:
        Interfaces interfaceType();
};

#endif // ENCRYPTEDINTERFACE_H
