#include "volumegrouppage.h"
#include "ui_volumegrouppage.h"

#include "diskpane.h"
#include <DriveObjects/diskobject.h>
#include <DriveObjects/logicalvolume.h>
#include <DriveObjects/volumegroup.h>
#include <operations/disbandvgpopover.h>
#include <tpopover.h>
#include <volumegrouplvmodel.h>
#include <volumegrouppvmodel.h>

struct VolumeGroupPagePrivate {
        VolumeGroup* vg;
        VolumeGroupLvModel* lvModel;
        VolumeGroupPvModel* pvModel;
};

VolumeGroupPage::VolumeGroupPage(VolumeGroup* vg, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VolumeGroupPage) {
    ui->setupUi(this);
    d = new VolumeGroupPagePrivate();
    d->vg = vg;
    ui->titleLabel->setText(vg->name());
    ui->disbandButton->setProperty("type", "destructive");
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);
    ui->leftWidget->setFixedWidth(400);

    d->lvModel = new VolumeGroupLvModel(d->vg);
    connect(d->lvModel, &VolumeGroupLvModel::modelReset, this, [this] {
        ui->lvsView->setFixedHeight(ui->lvsView->sizeHintForRow(0) * d->lvModel->rowCount());
    });
    ui->lvsView->setModel(d->lvModel);
    ui->lvsView->setFixedHeight(ui->lvsView->sizeHintForRow(0) * d->lvModel->rowCount());
    connect(ui->lvsView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](QModelIndex current, QModelIndex previous) {
        ui->pvsView->clearSelection();
        ui->pvsView->selectionModel()->clearCurrentIndex();

        auto lv = current.data(VolumeGroupLvModel::LvRole).value<LogicalVolume*>();
        if (lv) {
            DiskPane* disk = new DiskPane(lv->block());
            ui->stackedWidget->addWidget(disk);
            ui->stackedWidget->setCurrentWidget(disk);
        }
    });

    d->pvModel = new VolumeGroupPvModel(d->vg);
    connect(d->pvModel, &VolumeGroupLvModel::modelReset, this, [this] {
        ui->pvsView->setFixedHeight(ui->pvsView->sizeHintForRow(0) * d->pvModel->rowCount());
    });
    ui->pvsView->setModel(d->pvModel);
    ui->pvsView->setFixedHeight(ui->lvsView->sizeHintForRow(0) * d->pvModel->rowCount());
    connect(ui->pvsView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](QModelIndex current, QModelIndex previous) {
        ui->lvsView->clearSelection();
        ui->lvsView->selectionModel()->clearCurrentIndex();

        auto pv = current.data(VolumeGroupPvModel::PvRole).value<DiskObject*>();
        if (pv) {
            DiskPane* disk = new DiskPane(pv);
            ui->stackedWidget->addWidget(disk);
            ui->stackedWidget->setCurrentWidget(disk);
        }
    });
}

VolumeGroupPage::~VolumeGroupPage() {
    delete ui;
    delete d;
}

void VolumeGroupPage::setTopPadding(int padding) {
    ui->leftWidget->layout()->setContentsMargins(0, padding, 0, 0);
    ui->verticalLayout_2->setContentsMargins(0, padding, 0, 0);
}

void VolumeGroupPage::on_titleLabel_backButtonClicked() {
    emit done();
}

void VolumeGroupPage::on_disbandButton_clicked() {
    auto* jp = new DisbandVgPopover(d->vg);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(-200);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &DisbandVgPopover::done, popover, &tPopover::dismiss);
    connect(jp, &DisbandVgPopover::deleted, this, &VolumeGroupPage::done);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &DisbandVgPopover::deleteLater);
    popover->show(this->window());
}
