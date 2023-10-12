#include "lvmdiskpane.h"
#include "ui_lvmdiskpane.h"

#include "popovers/pvremovepopover.h"
#include <DriveObjects/diskobject.h>
#include <DriveObjects/partitioninterface.h>
#include <DriveObjects/partitiontableinterface.h>
#include <DriveObjects/physicalvolumeinterface.h>
#include <DriveObjects/volumegroup.h>
#include <diskoperationmanager.h>
#include <partitioninformation.h>
#include <tpopover.h>

struct LvmDiskPanePrivate {
        DiskObject* disk;
};

LvmDiskPane::LvmDiskPane(DiskObject* disk, QWidget* parent) :
    DiskPaneComponent(parent),
    ui(new Ui::LvmDiskPane) {
    ui->setupUi(this);
    d = new LvmDiskPanePrivate();
    d->disk = disk;

    connect(disk, &DiskObject::interfaceAdded, this, &LvmDiskPane::updateDetails);
    connect(disk, &DiskObject::interfaceRemoved, this, &LvmDiskPane::updateDetails);

    this->updateDetails();
}

LvmDiskPane::~LvmDiskPane() {
    delete ui;
    delete d;
}

void LvmDiskPane::updateDetails() {
    auto partition = d->disk->interface<PartitionInterface>();
    if (!partition) {
        this->setVisible(false);
        return;
    }

    auto partitionTable = partition->parentTable()->interface<PartitionTableInterface>();
    if (partition->type() != PartitionInformation::partitionType("lvmpv", partitionTable->type())) {
        this->setVisible(false);
        return;
    }

    this->setVisible(true);
    auto pv = d->disk->interface<PhysicalVolumeInterface>();
    if (pv && pv->volumeGroup()) {
        ui->lvmDescription->setText(tr("This block is part of the %1 volume group.").arg(QLocale().quoteString(pv->volumeGroup()->name())));
        ui->pvSettingsButton->setVisible(true);
        ui->viewVgButton->setVisible(true);
        ui->attachVgButton->setVisible(false);
    } else {
        ui->lvmDescription->setText(tr("This block is not part of a volume group."));
        ui->pvSettingsButton->setVisible(false);
        ui->viewVgButton->setVisible(false);
        ui->attachVgButton->setVisible(true);
    }
}

int LvmDiskPane::order() const {
    return 20;
}

void LvmDiskPane::on_attachVgButton_clicked() {
    DiskOperationManager::showDiskOperationUi(this->window(), DiskOperationManager::AttachPv, d->disk);
}

void LvmDiskPane::on_pvSettingsButton_clicked() {
    auto* jp = new PvRemovePopover(d->disk);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(-200);
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &PvRemovePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &PvRemovePopover::deleteLater);
    popover->show(this->window());
}
