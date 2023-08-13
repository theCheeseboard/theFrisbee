#include "volumegrouppage.h"
#include "ui_volumegrouppage.h"

#include <DriveObjects/volumegroup.h>

struct VolumeGroupPagePrivate {
};

VolumeGroupPage::VolumeGroupPage(VolumeGroup* vg, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VolumeGroupPage) {
    ui->setupUi(this);
    d = new VolumeGroupPagePrivate();
    ui->titleLabel->setText(vg->name());
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
