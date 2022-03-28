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
#include "partitionvisualisation.h"

#include <QPainter>
#include <QMouseEvent>
#include <QRandomGenerator64>
#include <tpaintcalculator.h>

struct PartitionVisualisationPrivate {
    quint64 diskSize = 0;
    QList<PartitionVisualisation::Partition> currentPartitions;

    QString currentPartition;
    PartitionVisualisation::Partition selectedPartition;

    static quint64 internalPartitionIds;
    static QMap<quint64, DiskObject*> diskMappings;
};

quint64 PartitionVisualisationPrivate::internalPartitionIds = 1;
QMap<quint64, DiskObject*> PartitionVisualisationPrivate::diskMappings = QMap<quint64, DiskObject*>();

PartitionVisualisation::PartitionVisualisation(QWidget* parent) : QWidget(parent) {
    d = new PartitionVisualisationPrivate();
    this->setMouseTracking(true);
}

PartitionVisualisation::~PartitionVisualisation() {
    delete d;
}

void PartitionVisualisation::setDiskSize(quint64 diskSize) {
    d->diskSize = diskSize;
    this->update();
}

quint64 PartitionVisualisation::diskSize() {
    return d->diskSize;
}

void PartitionVisualisation::setPartitions(QList<Partition> partitions) {
    d->currentPartitions = partitions;
    this->update();
}

QList<PartitionVisualisation::Partition> PartitionVisualisation::partitions() {
    return d->currentPartitions;
}

void PartitionVisualisation::setSelectedPartition(Partition partition) {
    d->selectedPartition = partition;
    this->update();
}

PartitionVisualisation::Partition PartitionVisualisation::selectedPartition() {
    return d->selectedPartition;
}

tPaintCalculator PartitionVisualisation::calculatePartitions(QPainter* painter) {
    tPaintCalculator calculator;
    calculator.setPainter(painter);
    calculator.setDrawBounds(QRectF(0, 0, this->width(), this->height()));

    calculator.addRect(QRectF(0, 0, this->width(), this->height()), [ = ](QRectF drawBounds) {
        painter->fillRect(drawBounds, Qt::black);
    });

    if (d->diskSize == 0) return calculator;

    double factor = this->width() / (double) d->diskSize;
    for (int i = 0; i < d->currentPartitions.count(); i++) {
        PartitionVisualisation::Partition p = d->currentPartitions.at(i);

        QRectF partitionRect;
        partitionRect.moveLeft(p.offset * factor);
        partitionRect.setWidth(p.size * factor);
        partitionRect.moveTop(0);
        partitionRect.setHeight(this->height());

        QString rectName = QString::number(i);
        calculator.addRect(rectName, partitionRect, [ = ](QRectF drawBounds) {
            QColor col = p.color;
            if (d->currentPartition == rectName) col = p.color.lighter();
            if (d->selectedPartition == p) col = p.color.darker();
            painter->fillRect(drawBounds, col);

            painter->setPen(Qt::white);
            painter->drawText(drawBounds.adjusted(9, 9, -9, -9), Qt::AlignLeft, QStringList({p.name, QLocale().formattedDataSize(p.size)}).join("\n"));
        });
    }

    return calculator;
}

QString PartitionVisualisation::hitTestPartition(double x) {
    tPaintCalculator calculator = this->calculatePartitions(nullptr);
    QStringList partitions = calculator.hitTest(QPointF(x, 0));

    for (const QString& partition : qAsConst(partitions)) {
        bool ok;
        partition.toInt(&ok);
        if (ok) return partition;
    }
    return "";
}

void PartitionVisualisation::mapPartition(quint64 id, DiskObject* disk) {
    PartitionVisualisationPrivate::diskMappings.insert(id, disk);
}

DiskObject* PartitionVisualisation::mappedDisk(quint64 id) {
    return PartitionVisualisationPrivate::diskMappings.value(id);
}


void PartitionVisualisation::mousePressEvent(QMouseEvent* event) {
    if (d->currentPartition.isEmpty()) {
        double factor = this->width() / (double) d->diskSize;
        int ltrPos = event->localPos().x();
        if (this->layoutDirection() == Qt::RightToLeft) ltrPos = this->width() - ltrPos;
        quint64 offset = ltrPos / factor;

        //See if there is a partition to the left and snap to it if so
        QString previousPartition = hitTestPartition(event->localPos().x() + (this->layoutDirection() == Qt::LeftToRight ? -5 : 5));
        if (!previousPartition.isEmpty()) {
            Partition p = d->currentPartitions.at(previousPartition.toInt());
            offset = p.offset + p.size;
        } else if (event->localPos().x() < 5) {
            offset = 1024;
        }

        //Find out how much more space is available
        quint64 endOffset = d->diskSize;
        for (Partition p : d->currentPartitions) {
            if (endOffset > p.offset && p.offset > offset) endOffset = p.offset;
        }

        emit emptySpaceClicked(offset, endOffset - offset);
    } else {
        emit partitionClicked(d->currentPartitions.at(d->currentPartition.toInt()));
    }
}

void PartitionVisualisation::mouseReleaseEvent(QMouseEvent* event) {

}

void PartitionVisualisation::mouseMoveEvent(QMouseEvent* event) {
    d->currentPartition = hitTestPartition(event->localPos().x());
    this->update();
}

void PartitionVisualisation::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    tPaintCalculator calculator = calculatePartitions(&painter);
    calculator.performPaint();
}


void PartitionVisualisation::leaveEvent(QEvent* event) {
    d->currentPartition = "";
    this->update();
}

PartitionVisualisation::Partition::Partition() {
    internalId = PartitionVisualisationPrivate::internalPartitionIds;
    PartitionVisualisationPrivate::internalPartitionIds++;
}

void PartitionVisualisation::Partition::setDisk(DiskObject* disk) {
    PartitionVisualisationPrivate::diskMappings.insert(this->internalId, disk);
}

DiskObject* PartitionVisualisation::Partition::disk() {
    return PartitionVisualisationPrivate::diskMappings.value(this->internalId, nullptr);
}

bool PartitionVisualisation::Partition::operator==(const Partition& other) const {
    return other.internalId == this->internalId;
}

quint64 PartitionVisualisation::Partition::id() {
    return this->internalId;
}
