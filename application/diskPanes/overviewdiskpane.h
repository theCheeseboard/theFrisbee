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
#ifndef OVERVIEWDISKPANE_H
#define OVERVIEWDISKPANE_H

#include "diskpanecomponent.h"

namespace Ui {
    class OverviewDiskPane;
}

struct OverviewDiskPanePrivate;
class DiskObject;
class OverviewDiskPane : public DiskPaneComponent {
        Q_OBJECT

    public:
        explicit OverviewDiskPane(DiskObject* disk, QWidget* parent = nullptr);
        ~OverviewDiskPane();

    private slots:
        void on_ejectButton_clicked();

        void on_mountButton_clicked();

        void on_unmountButton_clicked();

    private:
        Ui::OverviewDiskPane* ui;
        OverviewDiskPanePrivate* d;

        void updateData();
        void updateLock(bool locked);

        // DiskPaneComponent interface
    public:
        int order() const;

};

#endif // OVERVIEWDISKPANE_H
