/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#ifndef PARTITIONVISUALISATION_H
#define PARTITIONVISUALISATION_H

#include <QWidget>

class DiskObject;
class tPaintCalculator;
class EditPartitionJob;
struct PartitionVisualisationPrivate;
class PartitionVisualisation : public QWidget {
        Q_OBJECT
    public:
        explicit PartitionVisualisation(QWidget* parent = nullptr);
        ~PartitionVisualisation();

        struct Partition {
                Partition();

                quint64 offset;
                quint64 size;
                QString name;
                QColor color;

                void setDisk(DiskObject* disk);
                DiskObject* disk();

                bool operator==(const Partition& other) const;

                quint64 id();

            private:
                quint64 internalId;
        };

        void setDiskSize(quint64 diskSize);
        quint64 diskSize();

        void setPartitions(QList<Partition> partitions);
        QList<Partition> partitions();

        void setSelectedPartition(Partition partition);
        Partition selectedPartition();

    signals:
        void partitionClicked(Partition partition);
        void emptySpaceClicked(quint64 offset, quint64 availableSpaceAfter);

    private:
        friend EditPartitionJob;
        PartitionVisualisationPrivate* d;

        tPaintCalculator calculatePartitions(QPainter* painter);
        QString hitTestPartition(double x);

        static void mapPartition(quint64 id, DiskObject* disk);
        static DiskObject* mappedDisk(quint64 id);

        // QWidget interface
    protected:
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
        void mouseMoveEvent(QMouseEvent* event);
        void paintEvent(QPaintEvent* event);
        void leaveEvent(QEvent* event);
};

#endif // PARTITIONVISUALISATION_H
