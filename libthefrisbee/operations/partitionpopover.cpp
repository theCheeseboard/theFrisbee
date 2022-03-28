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
#include <driveobjectmanager.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/partitioninterface.h>
#include <DriveObjects/partitiontableinterface.h>

#include <tlogger.h>
#include <tpaintcalculator.h>
#include <tjobmanager.h>
#include "partitioninformation.h"
#include "jobs/editpartitionjob.h"

struct PartitionPopoverPrivate {
    DiskObject* disk;
    QRandomGenerator colorGenerator;

    PartitionVisualisation::Partition editing;

    QList<PartitionPopover::PartitionOperation> operations;
    PartitionPopover::PartitionOperation currentOperation;
};

PartitionPopover::PartitionPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PartitionPopover) {
    ui->setupUi(this);

    d = new PartitionPopoverPrivate();
    d->disk = disk;

    ui->actionsWidget->setFixedWidth(SC_DPI(600));
    ui->newPartitionsWidget->setFixedWidth(SC_DPI(600));
    ui->editPartitionWidget->setFixedWidth(SC_DPI(600));
    ui->applyWidget->setFixedWidth(SC_DPI(600));
    ui->applyButton->setFixedWidth(SC_DPI(600));

    ui->titleLabel->setBackButtonShown(true);

    QSignalBlocker blocker(ui->partitionType);

    for (QString type : PartitionInformation::availableFormatTypes()) {
        ui->partitionType->addItem(PartitionInformation::typeName(type), type);
    }
    ui->partitionType->addItem(tr("Leave Empty"), "empty");

    ui->visualisation->setFixedHeight(SC_DPI(100));
    initialiseState();

    connect(ui->visualisation, &PartitionVisualisation::partitionClicked, this, [ = ](PartitionVisualisation::Partition partition) {
        if (ui->mainStack->currentWidget() != ui->mainPage) return;
        if (d->editing == partition) return;
        commitEdits();

        d->currentOperation.type = "edit";
        d->currentOperation.data.insert("partition", partition.id());

        d->editing = partition;
        ui->visualisation->setSelectedPartition(partition);
        ui->stackedWidget->setCurrentWidget(ui->editPartitionPage);

        loadEditPage();
    });
    connect(ui->visualisation, &PartitionVisualisation::emptySpaceClicked, this, [ = ](quint64 offset, quint64 availableSpaceAfter) {
        if (ui->mainStack->currentWidget() != ui->mainPage) return;
        commitEdits();

        PartitionVisualisation::Partition p;
        p.name = tr("New Partition");
        p.offset = offset;
        p.size = availableSpaceAfter;
        p.color = QColor::fromRgb(d->colorGenerator.bounded(256), d->colorGenerator.bounded(256), d->colorGenerator.bounded(256));

        QList<PartitionVisualisation::Partition> partitions = ui->visualisation->partitions();
        partitions.append(p);
        ui->visualisation->setPartitions(partitions);

        d->currentOperation.type = "new";
        d->currentOperation.data.insert("name", p.name);
        d->currentOperation.data.insert("size", p.size);
        d->currentOperation.data.insert("offset", p.offset);
        d->currentOperation.data.insert("internalId", p.id());
        d->currentOperation.data.insert("type", "ext4");
        d->currentOperation.edited = true;

        d->editing = p;
        ui->visualisation->setSelectedPartition(p);
        ui->stackedWidget->setCurrentWidget(ui->editPartitionPage);

        loadEditPage();
    });

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);
    ui->stackedWidget->setCurrentWidget(ui->actionsPage, false);
    ui->mainStack->setCurrentAnimation(tStackedWidget::SlideHorizontal);
}

PartitionPopover::~PartitionPopover() {
    delete ui;
}

void PartitionPopover::on_titleLabel_backButtonClicked() {
    if (ui->mainStack->currentWidget() == ui->mainPage) {
        if (ui->stackedWidget->currentWidget() == ui->actionsPage) {
            emit done();
        } else {
            commitEdits();
            ui->stackedWidget->setCurrentWidget(ui->actionsPage);
            ui->visualisation->setSelectedPartition(PartitionVisualisation::Partition());
            d->editing = PartitionVisualisation::Partition();
        }
    } else {
        ui->mainStack->setCurrentWidget(ui->mainPage);
    }
}

void PartitionPopover::initialiseState() {
    d->colorGenerator.seed(1);
    PartitionTableInterface* partitionTable = d->disk->interface<PartitionTableInterface>();

    QList<PartitionVisualisation::Partition> partitions;
    for (DiskObject* disk : partitionTable->partitions()) {
        PartitionInterface* partition = disk->interface<PartitionInterface>();
        BlockInterface* block = disk->interface<BlockInterface>();
        PartitionVisualisation::Partition p;
        p.name = partition->name();
        p.offset = partition->offset();
        p.size = block->size();
        p.setDisk(disk);
        p.color = QColor::fromRgb(d->colorGenerator.bounded(256), d->colorGenerator.bounded(256), d->colorGenerator.bounded(256));
        partitions.append(p);
    }

    ui->visualisation->setDiskSize(d->disk->interface<BlockInterface>()->size());
    ui->visualisation->setPartitions(partitions);
}

void PartitionPopover::commitEdits() {
    if (d->currentOperation.edited) {
        d->operations.append(d->currentOperation);
    }
    d->currentOperation = PartitionPopover::PartitionOperation();
}

void PartitionPopover::loadEditPage() {
    QSignalBlocker blocker1(ui->partitionName);
    QSignalBlocker blocker2(ui->partitionType);
    ui->partitionName->setText(d->editing.name);

    if (d->currentOperation.type == "new") {
        QAbstractItemModel* model = ui->partitionType->model();
        for (int i = 0; i < model->rowCount(); i++) {
            if (model->data(model->index(i, 0), Qt::UserRole) == d->currentOperation.type) ui->partitionType->setCurrentIndex(i);
        }

        ui->partitionType->setVisible(true);
        ui->partitionTypeLabel->setVisible(true);
    } else {
        ui->partitionType->setVisible(false);
        ui->partitionTypeLabel->setVisible(false);
    }
}

void PartitionPopover::updatePartition(const PartitionVisualisation::Partition partition) {
    QList<PartitionVisualisation::Partition> plist = ui->visualisation->partitions();
    plist.replace(plist.indexOf(partition), partition);
    ui->visualisation->setPartitions(plist);
}

void PartitionPopover::on_deletePartitionButton_clicked() {
    //We can just disregard the new partition if we are deleting it
    if (d->currentOperation.type != "new") {
        PartitionOperation operation;
        operation.type = "delete";
        operation.data.insert("partition", d->editing.id());
        d->operations.append(operation);
    }
    d->currentOperation = PartitionPopover::PartitionOperation();

    //Update the partition mapping
    QList<PartitionVisualisation::Partition> partitions = ui->visualisation->partitions();
    partitions.removeAll(d->editing);
    ui->visualisation->setPartitions(partitions);

    ui->stackedWidget->setCurrentWidget(ui->actionsPage);
}


void PartitionPopover::on_applyButton_clicked() {
    commitEdits();

    ui->visualisation->setSelectedPartition(PartitionVisualisation::Partition());
    d->editing = PartitionVisualisation::Partition();

    bool haveDeleteOperation = false;
    for (PartitionOperation operation : d->operations) {
        if (operation.type == "delete") haveDeleteOperation = true;
    }

    if (haveDeleteOperation) {
        ui->applyDescription->setText(tr("Once these changes are applied, you won't be able to recover any partitions that you've deleted easily."));
        ui->performApplyButton->setProperty("type", "destructive");
    } else {
        ui->applyDescription->setText(tr("Ready to apply these changes to the disk?"));
        ui->performApplyButton->setProperty("type", "normal");
    }

    ui->mainStack->setCurrentWidget(ui->applyPage);
    ui->stackedWidget->setCurrentWidget(ui->actionsPage);
}


void PartitionPopover::on_performApplyButton_clicked() {
    for (const PartitionOperation& op : qAsConst(d->operations)) {
        tDebug("PartitionPopover") << "Operation type: " << op.type;
        for (const QString& key : op.data.keys()) {
            tDebug("PartitionPopover") << key << " -> " << op.data.value(key).toString();
        }
    }

    EditPartitionJob* job = new EditPartitionJob(d->operations, d->disk);
    tJobManager::trackJob(job);
    emit done();
}


void PartitionPopover::on_partitionName_textEdited(const QString& arg1) {
    d->currentOperation.data.insert("name", arg1);
    d->currentOperation.edited = true;

    d->editing.name = arg1;
    updatePartition(d->editing);
}


void PartitionPopover::on_partitionType_currentIndexChanged(int index) {
    d->currentOperation.data.insert("type", ui->partitionType->currentData().toString());
    d->currentOperation.edited = true;
}

