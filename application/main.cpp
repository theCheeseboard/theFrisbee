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

#include <QDir>
#include <QCommandLineParser>
#include <tsettings.h>
#include <tapplication.h>
#include <tstylemanager.h>
#include <driveobjectmanager.h>
#include "operationmanager.h"

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);

    if (QDir("/usr/share/thefrisbee/").exists()) {
        a.setShareDir("/usr/share/thefrisbee/");
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thefrisbee/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thefrisbee/"));
    }
    a.installTranslators();

    a.setApplicationVersion("1.0");
    a.setGenericName(QApplication::translate("main", "Disk Utility"));
    a.setAboutDialogSplashGraphic(a.aboutDialogSplashGraphicFromSvg(":/icons/aboutsplash.svg"));
    a.setApplicationLicense(tApplication::Gpl3OrLater);
    a.setCopyrightHolder("Victor Tran");
    a.setCopyrightYear("2020");
    a.setOrganizationName("theSuite");
//    a.setApplicationUrl(tApplication::HelpContents, QUrl("https://help.vicr123.com/docs/thefrisbee/intro"));
    a.setApplicationUrl(tApplication::Sources, QUrl("http://github.com/vicr123/thefrisbee"));
    a.setApplicationUrl(tApplication::FileBug, QUrl("http://github.com/vicr123/thefrisbee/issues"));
#ifdef T_BLUEPRINT_BUILD
    a.setApplicationName("theFrisbee Blueprint");
    a.setDesktopFileName("com.vicr123.thefrisbee_blueprint");
    a.setApplicationIcon(QIcon(":/icons/thefrisbee-blueprint.svg"));
#else
    a.setApplicationName("theFrisbee");
    a.setDesktopFileName("com.vicr123.thefrisbee");
    a.setApplicationIcon(QIcon::fromTheme("com.vicr123.thefrisbee", QIcon(":/icons/thefrisbee.svg")));
#endif

    a.registerCrashTrap();

    tStyleManager::setOverrideStyleForApplication(tStyleManager::ContemporaryDark);

    tSettings::registerDefaults(a.applicationDirPath() + "/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/thefrisbee/defaults.conf");

    //Build the string to show in help about operations
    QStringList operationsString;
    for (int operation = OperationManager::Erase; operation <= OperationManager::LastOperation; operation++) {
        operationsString.append(QStringLiteral("    %1 - %2").arg(OperationManager::operationForOperation(static_cast<OperationManager::DiskOperation>(operation)), OperationManager::descriptionForOperation(static_cast<OperationManager::DiskOperation>(operation))));
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
        if (!OperationManager::isValidOperation(operation)) {
            //Error
            err << "thefrisbee: " + QApplication::translate("main", "invalid operation %1").arg(operation) + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        if (parser.positionalArguments().length() == 1) {
            //Error
            err << "thefrisbee: " + QApplication::translate("main", "missing device") + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        if (parser.positionalArguments().length() > 2) {
            //Error
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
            //Error
            err << "thefrisbee: " + QApplication::translate("main", "invalid device") + "\n";
            err << QApplication::translate("main", "Usage: %1 [options] [operation device].").arg(a.arguments().first()) + "\n";
            err << "       " + QApplication::translate("main", "%1 -h for more information.").arg(a.arguments().first()) + "\n";
            return 1;
        }

        OperationManager::showDiskOperationUi(&w, OperationManager::operationForString(operation), disk);
    }

    return a.exec();
}
