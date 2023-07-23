#include "lvmpage.h"
#include "ui_lvmpage.h"

#include <volumegroupmodel.h>

struct LvmPagePrivate {
        VolumeGroupModel* vgModel;
};

LvmPage::LvmPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LvmPage) {
    ui->setupUi(this);

    d = new LvmPagePrivate();
    d->vgModel = new VolumeGroupModel(this);
    d->vgModel->setShowAddButton(true);
    ui->listView->setModel(d->vgModel);
}

LvmPage::~LvmPage() {
    delete ui;
    delete d;
}

void LvmPage::setTopPadding(int padding) {
    ui->vgsPage->layout()->setContentsMargins(0, padding, 0, 0);
}
