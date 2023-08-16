#include "addlvpopover.h"
#include "ui_addlvpopover.h"

#include "DriveObjects/blockinterface.h"
#include "DriveObjects/diskobject.h"
#include "DriveObjects/logicalvolume.h"
#include "DriveObjects/volumegroup.h"
#include "partitioninformation.h"
#include <frisbeeexception.h>
#include <tcontentsizer.h>
#include <terrorflash.h>

struct AddLvPopoverPrivate {
        VolumeGroup* vg;
};

AddLvPopover::AddLvPopover(VolumeGroup* vg, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AddLvPopover) {
    ui->setupUi(this);
    d = new AddLvPopoverPrivate();
    d->vg = vg;

    new tContentSizer(ui->optionsWidget);

    ui->sizeRangeBox->setMaximumSize(d->vg->freeSize());
    ui->sizeRangeBox->setSize(d->vg->freeSize());

    for (QString type : PartitionInformation::availableFormatTypes()) {
        if (type == QStringLiteral("lvmpv")) continue;
        ui->formatBox->addItem(PartitionInformation::typeName(type), type);
    }
    ui->formatBox->addItem(tr("Leave Empty"), "empty");
}

AddLvPopover::~AddLvPopover() {
    delete ui;
    delete d;
}

QCoro::Task<> AddLvPopover::on_createButton_clicked() {
    if (ui->nameBox->text().isEmpty()) {
        tErrorFlash::flashError(ui->nameBox);
        co_return;
    }

    if (ui->sizeRangeBox->size() == 0) {
        tErrorFlash::flashError(ui->sizeRangeBox);
        co_return;
    }

    auto format = ui->formatBox->currentData(Qt::UserRole).toString();
    auto name = ui->nameBox->text();
    auto size = ui->sizeRangeBox->size();

    emit done();

    try {
        auto lv = co_await d->vg->createPlainVolume(name, size, {});
        if (format != QStringLiteral("empty")) {
            QVariantMap formatOptions{
                {"label",                 name},
                {"no-block",              true},
                {"update-partition-type", true},
                {"tear-down",             true}
            };
            co_await lv->block()->interface<BlockInterface>()->format(format, formatOptions);
        }
    } catch (FrisbeeException& exception) {
        tWarn("AddLvPopover") << exception.response();
    }
}

void AddLvPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
