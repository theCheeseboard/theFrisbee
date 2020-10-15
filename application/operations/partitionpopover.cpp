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
#include "partitionpopover.h"
#include "ui_partitionpopover.h"

#include <QPainter>
#include <QRandomGenerator>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/partitioninterface.h>
#include <DriveObjects/partitiontableinterface.h>

struct PartitionPopoverPrivate {
    struct Partition {
        quint64 offset;
        quint64 size;
        QString name;
        QColor color;
        DiskObject* disk;
    };

    DiskObject* disk;
    QList<Partition> currentPartitions;
    QRandomGenerator colorGenerator;
};

PartitionPopover::PartitionPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PartitionPopover) {
    ui->setupUi(this);

    d = new PartitionPopoverPrivate();
    d->disk = disk;

    ui->actionsWidget->setFixedWidth(SC_DPI(600));
    ui->titleLabel->setBackButtonShown(true);

    ui->visualisation->setFixedHeight(SC_DPI(100));
    ui->visualisation->installEventFilter(this);
    initialiseState();
}

PartitionPopover::~PartitionPopover() {
    delete ui;
}

void PartitionPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

bool PartitionPopover::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->visualisation && event->type() == QEvent::Paint) {
        QPainter painter(ui->visualisation);

        painter.fillRect(0, 0, ui->visualisation->width(), ui->visualisation->height(), Qt::black);

        quint64 diskSize = d->disk->interface<BlockInterface>()->size();
        double factor = ui->visualisation->width() / (double) diskSize;
        for (PartitionPopoverPrivate::Partition p : d->currentPartitions) {
            QRectF partitionRect;
            partitionRect.moveLeft(p.offset * factor);
            partitionRect.setWidth(p.size * factor);
            partitionRect.moveTop(0);
            partitionRect.setHeight(ui->visualisation->height());

//            painter.setBrush(p.color);
//            painter.setPen(Qt::transparent);
            painter.fillRect(partitionRect, p.color);
        }
    }
    return false;
}

void PartitionPopover::initialiseState() {
    d->currentPartitions.clear();
    d->colorGenerator.seed(1);
    PartitionTableInterface* partitionTable = d->disk->interface<PartitionTableInterface>();
    for (DiskObject* disk : partitionTable->partitions()) {
        PartitionInterface* partition = disk->interface<PartitionInterface>();
        BlockInterface* block = disk->interface<BlockInterface>();
        PartitionPopoverPrivate::Partition p;
        p.name = partition->name();
        p.offset = partition->offset();
        p.size = block->size();
        p.disk = disk;
        p.color = QColor::fromRgb(d->colorGenerator.bounded(256), d->colorGenerator.bounded(256), d->colorGenerator.bounded(256));
        d->currentPartitions.append(p);
    }

    ui->visualisation->update();
}
