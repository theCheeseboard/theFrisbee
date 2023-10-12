#include "sizerangebox.h"
#include "ui_sizerangebox.h"

struct SizeRangeBoxPrivate {
        static constexpr int divisionFactor = 1024 * 1024;
};

SizeRangeBox::SizeRangeBox(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SizeRangeBox) {
    ui->setupUi(this);
    d = new SizeRangeBoxPrivate();
}

SizeRangeBox::~SizeRangeBox() {
    delete ui;
    delete d;
}

void SizeRangeBox::setMaximumSize(quint64 size) {
    ui->slider->setMaximum(size / SizeRangeBoxPrivate::divisionFactor);
}

quint64 SizeRangeBox::size() {
    return static_cast<quint64>(ui->slider->value()) * SizeRangeBoxPrivate::divisionFactor;
}

void SizeRangeBox::setSize(quint64 size) {
    ui->slider->setValue(size / SizeRangeBoxPrivate::divisionFactor);
}

void SizeRangeBox::on_slider_valueChanged(int value) {
    ui->sizeBox->setSize(static_cast<quint64>(value) * SizeRangeBoxPrivate::divisionFactor);
}

void SizeRangeBox::on_sizeBox_editingFinished() {
    this->setSize(ui->sizeBox->size());
}
