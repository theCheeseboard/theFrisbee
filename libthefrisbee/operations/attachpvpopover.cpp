#include "attachpvpopover.h"
#include "ui_attachpvpopover.h"

#include <DriveObjects/volumegroup.h>
#include <driveobjectmanager.h>
#include <tcontentsizer.h>
#include <terrorflash.h>
#include <volumegroupmodel.h>

struct AttachPvPopoverPrivate {
        DiskObject* disk;
        VolumeGroupModel* vgModel;
};

AttachPvPopover::AttachPvPopover(DiskObject* disk, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AttachPvPopover) {
    ui->setupUi(this);
    d = new AttachPvPopoverPrivate();
    d->disk = disk;

    new tContentSizer(ui->optionsWidget);
    new tContentSizer(ui->newVgOptionsWidget);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);

    d->vgModel = new VolumeGroupModel(this);
    d->vgModel->setShowAddButton(true);
    ui->vgList->setModel(d->vgModel);
}

AttachPvPopover::~AttachPvPopover() {
    delete d;
    delete ui;
}

void AttachPvPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void AttachPvPopover::on_vgList_activated(const QModelIndex& index) {
    auto vg = index.data(VolumeGroupModel::VolumeGroupRole).value<VolumeGroup*>();
    if (vg) {
        vg->addDevice(d->disk, {});
        emit done();
    } else {
        ui->stackedWidget->setCurrentWidget(ui->addVgPage);
    }
}

void AttachPvPopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->selectVgPage);
}

void AttachPvPopover::on_attachToNewVgButton_clicked() {
    auto vgName = ui->newVgName->text();
    if (vgName.isEmpty()) {
        tErrorFlash::flashError(ui->newVgName);
        return;
    }

    DriveObjectManager::volumeGroupCreate(ui->newVgName->text(), {d->disk}, {});
    emit done();
}
