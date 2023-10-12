#include "removelvpopover.h"
#include "ui_removelvpopover.h"

#include "DriveObjects/logicalvolume.h"
#include <tcontentsizer.h>

struct RemoveLvPopoverPrivate {
        LogicalVolume* lv;
};

RemoveLvPopover::RemoveLvPopover(LogicalVolume* lv, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RemoveLvPopover) {
    ui->setupUi(this);
    d = new RemoveLvPopoverPrivate();
    d->lv = lv;

    new tContentSizer(ui->optionsWidget);

    ui->deleteButton->setProperty("type", "destructive");
}

RemoveLvPopover::~RemoveLvPopover() {
    delete d;
    delete ui;
}

void RemoveLvPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void RemoveLvPopover::on_deleteButton_clicked() {
    d->lv->deleteLogicalVolume({});
    emit done();
}
