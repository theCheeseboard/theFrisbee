/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "diskmodel.h"
#include "diskpane.h"
#include "operations/creatediskimagepopover.h"
#include <DriveObjects/blockinterface.h>
#include <DriveObjects/diskobject.h>
#include <QCoroSignal>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <driveobjectmanager.h>
#include <frisbeeexception.h>
#include <taboutdialog.h>
#include <tapplication.h>
#include <tcsdtools.h>
#include <thelpmenu.h>
#include <tjobmanager.h>
#include <tlogger.h>
#include <tpaintcalculator.h>
#include <tpopover.h>

struct MainWindowPrivate {
        tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    DriveObjectManager::instance();

    ui->jobButtonLayout->addWidget(tJobManager::makeJobButton());

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    ui->menuBar->setVisible(false);

    QMenu* menu = new QMenu(this);
    menu->addAction(ui->actionCreate_Disk_Image);
    menu->addAction(ui->actionMountImage);
    menu->addSeparator();
    menu->addMenu(new tHelpMenu(this));
    menu->addAction(ui->actionExit);

    ui->menuButton->setIcon(tApplication::applicationIcon());
    ui->menuButton->setIconSize(SC_DPI_WT(QSize(24, 24), QSize, this));
    ui->menuButton->setMenu(menu);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);

    ui->leftWidget->setFixedWidth(SC_DPI_W(400, this));

    ui->diskList->setModel(new DiskModel());
    connect(ui->diskList->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](QModelIndex current, QModelIndex previous) {
        if (current.isValid()) {
            DiskPane* disk = new DiskPane(static_cast<DiskObject*>(current.internalPointer()));
            ui->stackedWidget->addWidget(disk);
            ui->stackedWidget->setCurrentWidget(disk);
        }
    });

    ui->topWidget->installEventFilter(this);
}

MainWindow::~MainWindow() {
    delete ui;
    delete d;
}

QCoro::Task<> MainWindow::on_actionMountImage_triggered() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setNameFilters({"Disk Images (*.img *.iso)"});
    dialog->setFileMode(QFileDialog::AnyFile);

    dialog->open();

    auto result = co_await qCoro(dialog, &QFileDialog::finished);
    dialog->deleteLater();
    if (result == QFileDialog::Rejected) co_return;

    QFile f(dialog->selectedFiles().first());
    f.open(QFile::ReadOnly);

    QDBusUnixFileDescriptor fd(f.handle());
    try {
        auto loopSetupOptions = QVariantMap({
            {"read-only", false}
        });
        co_await DriveObjectManager::loopSetup(fd, loopSetupOptions);
    } catch (FrisbeeException& ex) {
        tCritical("LoopSetup") << ex.response();
    }
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (watched == ui->topWidget && event->type() == QEvent::Paint) {
        QPainter* painter = new QPainter(ui->topWidget);

        tPaintCalculator calculator;
        calculator.setPainter(painter);
        calculator.setDrawBounds(ui->topWidget->size());

        calculator.addRect(QRectF(SC_DPI_W(400, this) + (this->layoutDirection() == Qt::RightToLeft ? 1 : 0), 0, 0, ui->topWidget->height()), [painter, this](QRectF drawBounds) {
            painter->setPen(libContemporaryCommon::lineColor(this->palette().color(QPalette::WindowText)));
            painter->drawLine(drawBounds.topLeft(), drawBounds.bottomLeft());
        });

        calculator.performPaint();
        delete painter;
    }
    return false;
}

void MainWindow::on_actionCreate_Disk_Image_triggered() {
    CreateDiskImagePopover* jp = new CreateDiskImagePopover();
    tPopover* popover = new tPopover(jp);
    popover->setPopoverWidth(SC_DPI_W(-200, this));
    popover->setPopoverSide(tPopover::Bottom);
    connect(jp, &CreateDiskImagePopover::done, popover, &tPopover::dismiss);
    connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
    connect(popover, &tPopover::dismissed, jp, &CreateDiskImagePopover::deleteLater);
    popover->show(this->window());
}
