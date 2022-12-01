#include "smartdiskpane.h"
#include "ui_smartdiskpane.h"

#include "DriveObjects/atadriveinterface.h"
#include "DriveObjects/blockinterface.h"
#include "DriveObjects/diskobject.h"
#include "DriveObjects/driveinterface.h"

#include "popovers/smartpopover.h"
#include <tpopover.h>

struct SmartDiskPanePrivate {
        AtaDriveInterface* ataInterface = nullptr;
};

SmartDiskPane::SmartDiskPane(DiskObject* disk, QWidget* parent) :
    DiskPaneComponent(parent),
    ui(new Ui::SmartDiskPane) {
    ui->setupUi(this);
    d = new SmartDiskPanePrivate();

    auto drive = disk->interface<BlockInterface>()->drive();
    if (!drive) {
        this->setVisible(false);
        return;
    }

    d->ataInterface = drive->interface<AtaDriveInterface>();
    if (!d->ataInterface) {
        this->setVisible(false);
        return;
    }

    if (!d->ataInterface->smartSupported()) {
        this->setVisible(false);
        return;
    }

    connect(d->ataInterface, &AtaDriveInterface::smartEnabledChanged, this, &SmartDiskPane::updateSmartInfo);
    updateSmartInfo();
}

SmartDiskPane::~SmartDiskPane() {
    delete ui;
    delete d;
}

void SmartDiskPane::updateSmartInfo() {
    if (!d->ataInterface->smartEnabled()) {
        ui->smartDescription->setText(tr("SMART is currently not enabled for this drive"));
        return;
    }

    if (d->ataInterface->smartFailing()) {
        ui->smartDescription->setText(tr("The disk is about to fail"));
    } else {
        ui->smartDescription->setText(tr("The disk is operating normally"));
    }
}

int SmartDiskPane::order() const {
    return 5;
}

void SmartDiskPane::on_manageSmartButton_clicked() {
    auto* jp = new SmartPopover(d->ataInterface);
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &SmartPopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &SmartPopover::deleteLater);
    popover->show(this->window());
}
