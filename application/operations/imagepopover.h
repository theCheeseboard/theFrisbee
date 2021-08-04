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
#ifndef IMAGEPOPOVER_H
#define IMAGEPOPOVER_H

#include <QWidget>

namespace Ui {
    class ImagePopover;
}

class DiskObject;
struct ImagePopoverPrivate;
class ImagePopover : public QWidget {
        Q_OBJECT

    public:
        explicit ImagePopover(DiskObject* disk, QWidget* parent = nullptr);
        ~ImagePopover();

    private slots:
        void on_imageButton_clicked();

        void on_titleLabel_backButtonClicked();

        void on_browseButton_clicked();

    signals:
        void done();

    private:
        Ui::ImagePopover* ui;
        ImagePopoverPrivate* d;
};

#endif // IMAGEPOPOVER_H
