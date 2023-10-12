#include "disbandvgpopover.h"
#include "ui_disbandvgpopover.h"

#include "DriveObjects/volumegroup.h"
#include <tcontentsizer.h>

struct DisbandVgPopoverPrivate {
        VolumeGroup* vg;
};

DisbandVgPopover::DisbandVgPopover(VolumeGroup* vg, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DisbandVgPopover) {
    ui->setupUi(this);
    d = new DisbandVgPopoverPrivate();
    d->vg = vg;

    new tContentSizer(ui->disbandConfirmWidget);
    ui->disbandButton->setProperty("type", "destructive");
}

DisbandVgPopover::~DisbandVgPopover() {
    delete d;
    delete ui;
}

void DisbandVgPopover::on_disbandButton_clicked() {
    d->vg->deleteVg(false, {});
    emit deleted();
    emit done();
}

void DisbandVgPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
