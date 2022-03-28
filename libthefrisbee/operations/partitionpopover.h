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
#ifndef PARTITIONPOPOVER_H
#define PARTITIONPOPOVER_H

#include <QWidget>
#include <QVariantMap>
#include "components/partitionvisualisation.h"

namespace Ui {
    class PartitionPopover;
}

class tPaintCalculator;
class DiskObject;
struct PartitionPopoverPrivate;
class PartitionPopover : public QWidget {
        Q_OBJECT

    public:
        explicit PartitionPopover(DiskObject* disk, QWidget* parent = nullptr);
        ~PartitionPopover();

        struct PartitionOperation {
            QString type;
            QVariantMap data;
            bool edited = false;
        };

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_deletePartitionButton_clicked();

        void on_applyButton_clicked();

        void on_performApplyButton_clicked();

        void on_partitionName_textEdited(const QString& arg1);

        void on_partitionType_currentIndexChanged(int index);

    private:
        Ui::PartitionPopover* ui;
        PartitionPopoverPrivate* d;

        void initialiseState();
        void commitEdits();
        void loadEditPage();
        void updatePartition(const PartitionVisualisation::Partition partition);
};

#endif // PARTITIONPOPOVER_H
