/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#ifndef OPERATIONMANAGER_H
#define OPERATIONMANAGER_H

#include <QWidget>
#include <DriveObjects/diskobject.h>

class OperationManager : public QObject {
        Q_OBJECT
    public:
        OperationManager() = delete;

        enum DiskOperation : int {
            Erase,
            Restore,
            Image,
            LastOperation = Image
        };

        static DiskOperation operationForString(QString operationString);
        static QString operationForOperation(DiskOperation operation);
        static QString descriptionForOperation(DiskOperation operation);
        static bool isValidOperation(QString operationString);

        static void showDiskOperationUi(QWidget* parent, DiskOperation operation, DiskObject* disk);

        static bool ensureOpticalUtilitiesInstalled(QWidget* parent);

    private:
        static void showEraseOperationUi(QWidget* parent, DiskObject* disk);
        static void showRestoreOperationUi(QWidget* parent, DiskObject* disk);
        static void showImageOperationUi(QWidget* parent, DiskObject* disk);
};

#endif // OPERATIONMANAGER_H
