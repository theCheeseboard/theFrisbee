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
#ifndef RESTOREDISKJOB_H
#define RESTOREDISKJOB_H

#include "restorejob.h"
#include <QCoroTask>

struct RestoreDiskJobPrivate;
class DiskObject;
class RestoreDiskJob : public RestoreJob {
        Q_OBJECT
    public:
        explicit RestoreDiskJob(DiskObject* disk, QObject* parent = nullptr);
        ~RestoreDiskJob();

        QCoro::Task<> startRestore(QIODevice* source, quint64 dataSize);
        void cancel();

        DiskObject* disk();

        QString description();
        QString displayName();

    signals:
        void descriptionChanged(QString description);

    private:
        RestoreDiskJobPrivate* d;

        // tJob interface
    public:
        quint64 progress();
        quint64 totalProgress();
        State state();
        QWidget* makeProgressWidget();
        QString titleString();
        QString statusString();
};

#endif // RestoreDiskJob_H
