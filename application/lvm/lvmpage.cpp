#include "lvmpage.h"
#include "ui_lvmpage.h"

#include "volumegrouppage.h"
#include <DriveObjects/volumegroup.h>
#include <volumegroupmodel.h>

struct LvmPagePrivate {
        VolumeGroupModel* vgModel;
        int topPadding = 0;
};

LvmPage::LvmPage(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LvmPage) {
    ui->setupUi(this);

    d = new LvmPagePrivate();
    d->vgModel = new VolumeGroupModel(this);
    d->vgModel->setShowAddButton(false);
    ui->listView->setModel(d->vgModel);

    ui->noVgIcon->setPixmap(QIcon::fromTheme("drive-logical-volume").pixmap(QSize(128, 128)));

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->stackedWidget_2->setCurrentAnimation(tStackedWidget::Lift);

    connect(d->vgModel, &VolumeGroupModel::dataChanged, this, &LvmPage::updatePage);
    updatePage();
}

LvmPage::~LvmPage() {
    delete ui;
    delete d;
}

void LvmPage::setTopPadding(int padding) {
    d->topPadding = padding;
    ui->vgsPage->layout()->setContentsMargins(0, padding, 0, 0);
}

void LvmPage::on_listView_clicked(const QModelIndex& index) {
    auto vg = index.data(VolumeGroupModel::VolumeGroupRole).value<VolumeGroup*>();
    if (vg) {
        auto vgPage = new VolumeGroupPage(vg);
        vgPage->setTopPadding(d->topPadding);
        connect(vgPage, &VolumeGroupPage::done, this, [this] {
            ui->stackedWidget_2->setCurrentWidget(ui->vgsPage);
        });
        ui->stackedWidget_2->addWidget(vgPage);
        ui->stackedWidget_2->setCurrentWidget(vgPage);
    }
}

void LvmPage::updatePage() {
    if (d->vgModel->rowCount() == 0) {
        ui->stackedWidget->setCurrentWidget(ui->noVgsPage);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->vgListPage);
    }
}
