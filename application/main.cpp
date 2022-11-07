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

#include "diskoperationmanager.h"
#include <QCommandLineParser>
#include <QDir>
#include <driveobjectmanager.h>
#include <tapplication.h>
#include <tsettings.h>
#include <tstylemanager.h>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);
    a.setApplicationShareDir("thefrisbee");
    a.addLibraryTranslator(LIBTHEFRISBEE_TRANSLATOR);
    a.installTranslators();

    a.setApplicationVersion("2.0");
    a.setGenericName(QApplication::translate("main", "Disk Utility"));
    a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2022");
    a.setOrganizationName("theSuite");
    a.setApplicationUrl(tApplication::Sources, QUrl("http://github.com/vicr123/thefrisbee"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("http://github.com/vicr123/thefrisbee/issues"));
    a.setApplicationName(T_APPMETA_READABLE_NAME);
    a.setDesktopFileName(T_APPMETA_DESKTOP_ID);

    a.registerCrashTrap();

    tStyleManager::setOverrideStyleForApplication(tStyleManager::ContemporaryDark);

    // Build the string to show in help about operations
    QStringList operationsString;
    for (int operation = DiskOperationManager::Erase; operation <= DiskOperationManager::LastOperation; operation++) {
        operationsString.append(QStringLiteral("    %1 - %2").arg(DiskOperationManager::operationForOperation(static_cast<DiskOperationManager::DiskOperation>(operation)), DiskOperationManager::descriptionForOperation(static_cast<DiskOperationManager::DiskOperation>(operation))));
    }

    QCommandLineParser parser;
    parser.setApplicationDescription(QApplication::translate("main", "Disk Utility"));
    QCommandLineOption helpOption = parser.addHelpOption();
    QCommandLineOption versionOption = parser.addVersionOption();
    parser.addPositionalArgument("operation", QStringLiteral("%1\n%2").arg(QApplication::translate("main", "The operation to perform. Valid operations:"), operationsString.join("\n")), "[operation");
    parser.addPositionalArgument("device", QApplication::translate("main", "The device to perform the operation on"), "device]");
    parser.process(a);

    MainWindow w;
    w.show();

    if (parser.positionalArguments().length() > 0) {
        QTextStream err(stderr);

        QString operation = parser.positionalArguments().at(0);
        if (!DiskOperationManager::isValidOperation(operation)) {
            // Error
            err << "thefrisbee: " + QApplication::translate("main", "invalid operation %1").arg(operation) + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        if (parser.positionalArguments().length() == 1) {
            // Error
            err << "thefrisbee: " + QApplication::translate("main", "missing device") + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        if (parser.positionalArguments().length() > 2) {
            // Error
            err << "thefrisbee: " + QApplication::translate("main", "too many arguments") + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        QString device = parser.positionalArguments().at(1);
        DiskObject* disk = nullptr;
        disk = DriveObjectManager::diskByBlockName(device);
        if (disk == nullptr) disk = DriveObjectManager::diskForPath(QDBusObjectPath(device));

        if (disk == nullptr) {
            // Error
            err << "thefrisbee: " + QApplication::translate("main", "invalid device") + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        DiskOperationManager::showDiskOperationUi(&w, DiskOperationManager::operationForString(operation), disk);
    }

    return a.exec();
}
