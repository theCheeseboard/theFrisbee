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
#include <tsettings.h>
#include <tapplication.h>

int main(int argc, char* argv[]) {
    tApplication a(argc, argv);

    if (QDir("/usr/share/thefrisbee/").exists()) {
        a.setShareDir("/usr/share/thefrisbee/");
    } else if (QDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thefrisbee/")).exists()) {
        a.setShareDir(QDir::cleanPath(QApplication::applicationDirPath() + "/../share/thefrisbee/"));
    }
    a.installTranslators();

    a.setApplicationIcon(QIcon::fromTheme("thefrisbee", QIcon(":/icons/thefrisbee.svg")));
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
    a.setDesktopFileName("com.vicr123.thefrisbee-blueprint");
#else
    a.setApplicationName("theFrisbee");
    a.setDesktopFileName("com.vicr123.thefrisbee");
#endif

    a.registerCrashTrap();

    tSettings::registerDefaults(a.applicationDirPath() + "/defaults.conf");
    tSettings::registerDefaults("/etc/theSuite/thefrisbee/defaults.conf");

    MainWindow w;
    w.show();

    return a.exec();
}
