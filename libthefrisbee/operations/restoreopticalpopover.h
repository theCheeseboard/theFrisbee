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
#ifndef RESTOREOPTICALPOPOVER_H
#define RESTOREOPTICALPOPOVER_H

#include <QWidget>
#include <Task>

namespace Ui {
    class RestoreOpticalPopover;
}

struct RestoreOpticalPopoverPrivate;
class DiskObject;
class RestoreOpticalPopover : public QWidget {
        Q_OBJECT

    public:
        explicit RestoreOpticalPopover(DiskObject* disk, QWidget* parent = nullptr);
        ~RestoreOpticalPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_restoreFileButton_toggled(bool checked);

        void on_restoreDiskButton_toggled(bool checked);

        void on_browseButton_clicked();

        void on_restoreButton_clicked();

        void on_doRestoreButton_clicked();

        void on_titleLabel_2_backButtonClicked();

        void on_titleLabel_3_backButtonClicked();

        void on_doRestoreButtonOptical_clicked();

    private:
        Ui::RestoreOpticalPopover* ui;
        RestoreOpticalPopoverPrivate* d;

        QCoro::Task<> performRestoreOperation();
};

#endif // RESTOREOPTICALPOPOVER_H
