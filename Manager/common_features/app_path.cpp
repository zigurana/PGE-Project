/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

#include "app_path.h"
#include "../version.h"


QString ApplicationPath;
QString ApplicationPath_x;

QString AppPathManager::_settingsPath;

#ifndef __ANDROID__
#define UserDirName "/.PGE_Project"
#else
#define UserDirName "/PGE_Project"
#endif

void AppPathManager::initAppPath()
{
    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Content Manager");

    ApplicationPath = QApplication::applicationDirPath();
    ApplicationPath_x = QApplication::applicationDirPath();

    #ifdef __APPLE__
    //Application path relative bundle folder of application
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    if(ApplicationPath.endsWith(osX_bundle, Qt::CaseInsensitive))
        ApplicationPath.remove(ApplicationPath.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    #elif __ANDROID__
    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/PGE Project Data";
    QDir appPath(ApplicationPath);
    if(!appPath.exists())
        appPath.mkpath(ApplicationPath);

    /*
    QDir languagesFolder(ApplicationPath+"/languages");
    if(!languagesFolder.exists())
    {
        languagesFolder.mkpath(ApplicationPath+"/languages");
        DirCopy::copy("assets:/languages", languagesFolder.absolutePath());
    }

    QDir themesFolder(ApplicationPath+"/themes");
    if(!themesFolder.exists())
    {
        themesFolder.mkpath(ApplicationPath+"/themes");
        DirCopy::copy("assets:/themes", themesFolder.absolutePath());
    }*/
    #endif

    /*
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    QString test="/home/vasya/pge/"+osX_bundle;
    qDebug() << test << " <- before";
    if(test.endsWith(osX_bundle, Qt::CaseInsensitive))
        test.remove(test.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    qDebug() << test << " <- after";
    */

    if(isPortable())
        return;

    QSettings setup;
    bool userDir;
    userDir = setup.value("EnableUserDir", true).toBool();
//openUserDir:

    if(userDir)
    {
        #ifndef __ANDROID__
        QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        #else
        QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        #endif
        if(!path.isEmpty())
        {
            QDir appDir(path+UserDirName);
            if(!appDir.exists())
                if(!appDir.mkpath(path+UserDirName))
                    goto defaultSettingsPath;

            _settingsPath = appDir.absolutePath();
        }
        else
        {
            goto defaultSettingsPath;
        }
    }
    else
    {
        goto defaultSettingsPath;
    }

    return;
defaultSettingsPath:
    _settingsPath = ApplicationPath;
}

QString AppPathManager::settingsFile()
{
    return _settingsPath+"/pge_manager.ini";
}

QString AppPathManager::userAppDir()
{
    return _settingsPath;
}

void AppPathManager::install()
{
    #ifndef __ANDROID__
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    #else
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    #endif
    if(!path.isEmpty())
    {
        QDir appDir(path+UserDirName);
        if(!appDir.exists())
            if(!appDir.mkpath(path+UserDirName))
                return;

        QSettings setup;
        setup.setValue("EnableUserDir", true);
    }
}

bool AppPathManager::isPortable()
{
    if(_settingsPath.isNull())
        _settingsPath = ApplicationPath;
    if(!QFile(settingsFile()).exists()) return false;
    bool forcePortable=false;
    QSettings checkForPort(settingsFile(), QSettings::IniFormat);
    checkForPort.beginGroup("Main");
        forcePortable= checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    return forcePortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return (_settingsPath != ApplicationPath);
}


