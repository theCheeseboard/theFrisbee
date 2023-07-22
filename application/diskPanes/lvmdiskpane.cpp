#include "lvmdiskpane.h"
#include "ui_lvmdiskpane.h"

#include <DriveObjects/diskobject.h>
#include <DriveObjects/physicalvolumeinterface.h>
#include <DriveObjects/volumegroup.h>

struct LvmDiskPanePrivate {
        DiskObject* disk;
};

LvmDiskPane::LvmDiskPane(DiskObject* disk, QWidget* parent) :
    DiskPaneComponent(parent),
    ui(new Ui::LvmDiskPane) {
    ui->setupUi(this);
    d = new LvmDiskPanePrivate();

    auto pv = disk->interface<PhysicalVolumeInterface>();
    if (!pv) {
        this->setVisible(false);
        return;
    }

    //    connect(pv->volumeGroup(), &VolumeGroup::)
    ui->lvmDescription->setText(tr("This block is part of the %1 volume group").arg(QLocale().quoteString(pv->volumeGroup()->name())));
    ui->evictDataButton->setProperty("type", "destructive");
}

LvmDiskPane::~LvmDiskPane() {
    delete ui;
    delete d;
}

int LvmDiskPane::order() const {
    return 20;
}
