#include "smartpopover.h"
#include "ui_smartpopover.h"

#include <DriveObjects/atadriveinterface.h>
#include <QDateTime>
#include <tcontentsizer.h>

struct SmartPopoverPrivate {
        AtaDriveInterface* ataInterface;
};

SmartPopover::SmartPopover(AtaDriveInterface* ataInterface, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SmartPopover) {
    ui->setupUi(this);

    d = new SmartPopoverPrivate();
    d->ataInterface = ataInterface;

    connect(ataInterface, &AtaDriveInterface::smartEnabledChanged, this, &SmartPopover::updateSmartStatus);
    connect(ataInterface, &AtaDriveInterface::smartSelfTestStatusChanged, this, &SmartPopover::updateSmartStatus);
    connect(ataInterface, &AtaDriveInterface::smartSelfTestPercentRemainingChanged, this, &SmartPopover::updateSmartStatus);

    ui->smartAttributesWidget->setColumnCount(9);
    ui->smartAttributesWidget->setHeaderLabels({tr("ID"), tr("Attribute"), tr("Value"), tr("Normalised"), tr("Threshold"), tr("Worst"), tr("Type"), tr("Updates"), tr("Assessment")});
    ui->smartAttributesWidget->header()->setStretchLastSection(true);
    updateSmartStatus();

    ui->stackedWidget->setCurrentAnimation(tStackedWidget::SlideHorizontal);
    ui->leftWidget->setFixedWidth(SC_DPI_W(450, this));
    new tContentSizer(ui->selfTestConfirmWidget);
}

SmartPopover::~SmartPopover() {
    delete d;
    delete ui;
}

void SmartPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void SmartPopover::on_enableSmartButton_clicked() {
    d->ataInterface->setSmartEnabled(true);
}

QCoro::Task<> SmartPopover::updateSmartStatus() {
    if (d->ataInterface->smartEnabled()) {
        ui->stackedWidget_2->setCurrentWidget(ui->smartEnabledWidget);

        auto attributes = co_await d->ataInterface->smartGetAttributes();
        ui->smartAttributesWidget->clear();
        for (const auto& attribute : attributes) {
            auto item = new QTreeWidgetItem();
            item->setText(0, QLocale().toString(attribute->id()));
            item->setText(1, attribute->displayName());
            item->setText(2, attribute->displayValue());
            item->setText(3, QLocale().toString(attribute->value()));
            item->setText(4, QLocale().toString(attribute->threshold()));
            item->setText(5, QLocale().toString(attribute->worst()));
            item->setText(6, attribute->isPreFailAttribute() ? tr("Pre-Fail") : tr("Old-Age"));
            item->setText(7, attribute->isOnlineAttribute() ? tr("Online") : tr("Offline"));
            ui->smartAttributesWidget->addTopLevelItem(item);
        }

        for (auto i = 0; i < 8; i++) {
            ui->smartAttributesWidget->resizeColumnToContents(i);
        }

        auto selfTestStatus = d->ataInterface->smartSelfTestStatus();
        switch (selfTestStatus) {
            case AtaDriveInterface::SmartSelfTestStatus::Success:
                ui->selfTestResult->setText(tr("Successful"));
                break;
            case AtaDriveInterface::SmartSelfTestStatus::Aborted:
                ui->selfTestResult->setText(tr("Aborted"));
                break;
            case AtaDriveInterface::SmartSelfTestStatus::Interrupted:
                ui->selfTestResult->setText(tr("Interrupted"));
                break;
            case AtaDriveInterface::SmartSelfTestStatus::Fatal:
                ui->selfTestResult->setText(tr("Unable to complete"));
                break;
            case AtaDriveInterface::SmartSelfTestStatus::ErrorUnknown:
            case AtaDriveInterface::SmartSelfTestStatus::ErrorElectrical:
            case AtaDriveInterface::SmartSelfTestStatus::ErrorServo:
            case AtaDriveInterface::SmartSelfTestStatus::ErrorRead:
            case AtaDriveInterface::SmartSelfTestStatus::ErrorHandling:
                ui->selfTestResult->setText(tr("Failed"));
                break;
            case AtaDriveInterface::SmartSelfTestStatus::InProgress:
                ui->selfTestResult->setText(tr("In Progress"));
                break;
        }

        if (selfTestStatus == AtaDriveInterface::SmartSelfTestStatus::InProgress) {
            ui->startSmartSelfTestButton->setEnabled(false);
            ui->smartSelfTestInProgressWidget->setVisible(true);
            ui->selfTestProgressBar->setValue(100 - d->ataInterface->smartSelfTestPercentRemaining());
        } else {
            ui->startSmartSelfTestButton->setEnabled(true);
            ui->smartSelfTestInProgressWidget->setVisible(false);
        }

        ui->lastUpdatedLabel->setText(QLocale().toString(d->ataInterface->smartLastUpdated()));
    } else {
        ui->stackedWidget_2->setCurrentWidget(ui->smartDisabledPage);
    }
    ui->stackedWidget_2->setCurrentAnimation(tStackedWidget::Fade);
}

void SmartPopover::on_disableSmartButton_clicked() {
    d->ataInterface->setSmartEnabled(false);
}

void SmartPopover::on_titleLabel_2_backButtonClicked() {
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

QCoro::Task<> SmartPopover::on_doStartSelfTestButton_clicked() {
    AtaDriveInterface::SmartSelfTestType type;
    if (ui->shortSelfTestRadioButton->isChecked()) {
        type = AtaDriveInterface::SmartSelfTestType::Short;
    } else if (ui->extendedSelfTestRadioButton->isChecked()) {
        type = AtaDriveInterface::SmartSelfTestType::Extended;
    } else {
        type = AtaDriveInterface::SmartSelfTestType::Conveyance;
    }

    co_await d->ataInterface->startSmartSelfTest(type);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void SmartPopover::on_startSmartSelfTestButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->smartSelfTestPage);
}

void SmartPopover::on_abortSmartSelfTestButton_clicked() {
    d->ataInterface->abortSmartSelfTest();
}
