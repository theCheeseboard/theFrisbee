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

#include <tjobmanager.h>
#include <driveobjectmanager.h>
#include <DriveObjects/diskobject.h>
#include <DriveObjects/blockinterface.h>
#include <tcsdtools.h>
#include <QMenu>
#include <QDebug>
#include "diskmodel.h"
#include "diskpane.h"

struct MainWindowPrivate {
    tCsdTools csd;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    DriveObjectManager::instance();

//    DiskObject* o = new DiskObject(QDBusObjectPath("/"));
//    o->interface<BlockInterface>();

    for (auto disk : DriveObjectManager::rootDisks()) {
        if (!disk->isInterfaceAvailable(DiskInterface::Block)) continue;
        qDebug() << disk->interface<BlockInterface>()->blockName();
    }

    ui->jobButtonLayout->addWidget(tJobManager::makeJobButton());

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    QMenu* menu = new QMenu(this);

//    QMenu* helpMenu = new QMenu(this);
//    helpMenu->setTitle(tr("Help"));
//    helpMenu->addAction(ui->actionFileBug);
//    helpMenu->addAction(ui->actionSources);
//    helpMenu->addSeparator();
//    helpMenu->addAction(ui->actionAbout);

//    menu->addMenu(helpMenu);
//    menu->addAction(ui->actionExit);

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Lift);

    ui->diskList->setModel(new DiskModel());
    connect(ui->diskList->selectionModel(), &QItemSelectionModel::currentChanged, this, [ = ](QModelIndex current, QModelIndex previous) {
        if (current.isValid()) {
            DiskPane* disk = new DiskPane(static_cast<DiskObject*>(current.internalPointer()));
            ui->stackedWidget->addWidget(disk);
            ui->stackedWidget->setCurrentWidget(disk);
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
    delete d;
}

