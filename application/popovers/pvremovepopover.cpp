#include "pvremovepopover.h"
#include "ui_pvremovepopover.h"

#include <DriveObjects/diskobject.h>
#include <DriveObjects/physicalvolumeinterface.h>
#include <DriveObjects/volumegroup.h>
#include <frisbeeexception.h>
#include <tcontentsizer.h>

struct PvRemovePopoverPrivate {
        DiskObject* disk;
};

PvRemovePopover::PvRemovePopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::PvRemovePopover) {
    ui->setupUi(this);
    d = new PvRemovePopoverPrivate();
    d->disk = disk;

    new tContentSizer(ui->actionsWidget);
    new tContentSizer(ui->relocateDataWidget);
    new tContentSizer(ui->removeWidget);
    new tContentSizer(ui->doRemoveWidget);

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->removeButton->setProperty("type", "destructive");
    ui->removeButton_2->setProperty("type", "destructive");
    ui->doRemoveButton->setProperty("type", "destructive");
}

PvRemovePopover::~PvRemovePopover() {
    delete d;
    delete ui;
}

void PvRemovePopover::on_titleLabel_backButtonClicked() {
    emit done();
}

QCoro::Task<> PvRemovePopover::on_doRelocateButton_clicked() {
    emit done();

    auto disk = d->disk;
    auto pv = disk->interface<PhysicalVolumeInterface>();
    auto vg = pv->volumeGroup();

    co_await disk->lock();

    try {
        co_await vg->emptyDevice(disk, {});
    } catch (FrisbeeException ex) {
    }

    disk->releaseLock();
}

void PvRemovePopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
}

void PvRemovePopover::on_relocateButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->relocatePage);
}

void PvRemovePopover::on_titleLabel_3_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->settingsPage);
}

void PvRemovePopover::on_removeButton_2_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->doRemovePage);
}

void PvRemovePopover::on_titleLabel_4_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->removePage);
}

QCoro::Task<> PvRemovePopover::on_doRemoveButton_clicked() {
    emit done();

    auto disk = d->disk;
    auto pv = disk->interface<PhysicalVolumeInterface>();
    auto vg = pv->volumeGroup();

    co_await disk->lock();

    try {
        if (pv->size() != pv->freeSize()) {
            co_await vg->emptyDevice(disk, {});
        }
        co_await vg->removeDevice(disk, ui->overwriteSpaceBox->isChecked(), {});
    } catch (FrisbeeException ex) {
    }

    disk->releaseLock();
}

void PvRemovePopover::on_removeButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->removePage);
}
