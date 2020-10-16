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
#ifndef BLOCKINTERFACE_H
#define BLOCKINTERFACE_H

#include "diskinterface.h"
#include <tpromise.h>

struct BlockInterfacePrivate;
class DriveInterface;
class QIODevice;
class BlockInterface : public DiskInterface {
        Q_OBJECT
    public:
        explicit BlockInterface(QDBusObjectPath path, QObject* parent = nullptr);
        ~BlockInterface();

        enum OpenMode {
            Read,
            Write,
            ReadWrite
        };

        static QString interfaceName();
        Interfaces interfaceType();

        QString blockName();
        quint64 size();

        DriveInterface* drive();

        void format(QString type, QVariantMap options);
        tPromise<QIODevice*>* open(OpenMode mode, QVariantMap options);

    signals:
        void sizeChanged(qulonglong size);

    private:
        BlockInterfacePrivate* d;
};

#endif // BLOCKINTERFACE_H
