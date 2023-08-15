#include "volumegrouppage.h"
#include "ui_volumegrouppage.h"

#include <DriveObjects/volumegroup.h>
#include <volumegrouplvmodel.h>

struct VolumeGroupPagePrivate {
    VolumeGroup* vg;
    VolumeGroupLvModel* lvModel;
};

VolumeGroupPage::VolumeGroupPage(VolumeGroup* vg, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VolumeGroupPage) {
    ui->setupUi(this);
    d = new VolumeGroupPagePrivate();
    d->vg = vg;
    ui->titleLabel->setText(vg->name());
    ui->disbandButton->setProperty("type", "destructive");

    d->lvModel = new VolumeGroupLvModel(d->vg);
    connect(d->lvModel, &VolumeGroupLvModel::modelReset, this, [this] {
        ui->lvsView->setFixedHeight(ui->lvsView->sizeHintForRow(0) * d->lvModel->rowCount());
    });
    ui->lvsView->setModel(d->lvModel);
    ui->lvsView->setFixedHeight(ui->lvsView->sizeHintForRow(0) * d->lvModel->rowCount());
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

void VolumeGroupPage::on_disbandButton_clicked()
{

}

