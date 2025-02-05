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

#include <QtWidgets>

#include <common_features/app_path.h>
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <common_features/util.h>
#include <script/scriptholder.h>
#include <main_window/global_settings.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <audio/music_player.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_dialogs/savingnotificationdialog.h>

#include "level_edit.h"
#include <ui_leveledit.h>

bool LevelEdit::newFile(dataconfigs &configs, LevelEditingSettings options)
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(QString(curFile).replace("&", "&&&"));
    FileFormats::CreateLevelData(LvlData);
    LvlData.metaData.script.reset(new ScriptHolder());
    LvlData.untitled = true;
    StartLvlData = LvlData;

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    scene = new LvlScene(ui->graphicsView, configs, LvlData, this);
    scene->opts = options;

    scene->InitSection(0);
    scene->setPlayerPoints();
    scene->drawSpace();
    scene->buildAnimators();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
        connect(scene, SIGNAL(screenshotSizeCaptured()), this, SLOT(ExportingReady()));
    }

    if(options.animationEnabled)
    {
        scene->startAnimation();
    } else {
        stopAutoUpdateTimer();
    }
    return true;
}

bool LevelEdit::save(bool savOptionsDialog)
{
    if (isUntitled) {
        return saveAs(savOptionsDialog);
    } else {
        return saveFile(curFile);
    }
}

bool LevelEdit::saveAs(bool savOptionsDialog)
{
    bool makeCustomFolder = false;

    if(savOptionsDialog)
    {
        SavingNotificationDialog* sav = new SavingNotificationDialog(false, SavingNotificationDialog::D_QUESTION, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("Please enter a level title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving") + " " + userFriendlyCurrentFile());
        QLineEdit* lvlNameBox = new QLineEdit();
        QCheckBox* mkDirCustom = new QCheckBox();
        mkDirCustom->setText(QString(""));
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->addUserItem(tr("Make custom folder"), mkDirCustom);
        sav->setAdjustSize(400,120);
        lvlNameBox->setText(LvlData.LevelName);
        if(sav->exec() == QDialog::Accepted){
            LvlData.LevelName = lvlNameBox->text();
            makeCustomFolder = mkDirCustom->isChecked();
            lvlNameBox->deleteLater();
            mkDirCustom->deleteLater();
            sav->deleteLater();
            if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE){
                return false;
            }
        }else{
            return false;
        }
    }

    bool isNotDone=true;
    bool isSMBX64limit=false;
    QString fileName = (isUntitled)?GlobalSettings::savePath+QString("/")+
                                    (LvlData.LevelName.isEmpty()?curFile:util::filePath(LvlData.LevelName)):curFile;

    QString fileSMBX64  = "SMBX64 (1.3) Level file (*.lvl)";
    QString fileSMBXany = "SMBX0...64 Level file [choose version] (*.lvl)";
    QString fileSMBX38A = "SMBX-38a Level file (*.lvl)";
    QString filePGEX    = "Extended Level file (*.lvlx)";

    QString selectedFilter;
    if(isUntitled)
    {
        selectedFilter = fileSMBX64;
    }
    else
    {
        switch(LvlData.RecentFormat)
        {
        case LevelData::PGEX:
            selectedFilter = filePGEX;
            break;
        case LevelData::SMBX64:
            if( LvlData.RecentFormatVersion >= 64 )
                selectedFilter = fileSMBX64;
            else
                selectedFilter = fileSMBXany;
            break;
        case LevelData::SMBX38A:
            selectedFilter = fileSMBX38A;
            break;
        }
    }

    QString filter =
            fileSMBX64+";;"+
            fileSMBXany+";;"+
            fileSMBX38A+";;"+
            filePGEX;

    bool ret;

    RetrySave:

    isSMBX64limit = false;
    isNotDone     = true;

    while(isNotDone)
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"), fileName, filter, &selectedFilter);

        if (fileName.isEmpty())
            return false;

        if(selectedFilter==fileSMBXany)
        {
            int file_format=LvlData.RecentFormatVersion;

            bool ok=true;
            file_format = QInputDialog::getInt(this, tr("SMBX file version"),
                                    tr("Which version you wish to save? (from 0 to 64)\n"
                                       "List of known SMBX versions and format codes:\n%1\n"
                                       "(To allow level file work in specific SMBX version,\n"
                                       "version code must be less or equal specific code)"
                                       ).arg(" 1 - SMBX 1.0.0\n"
                                             " 2 - SMBX 1.0.2\n"
                                             "18 - SMBX 1.1.1\n"
                                             "20 - SMBX 1.1.2\n"
                                             "28 - SMBX 1.2.0 Beta 3\n"
                                             "51 - SMBX 1.2.1\n"
                                             "58 - SMBX 1.2.2\n"
                                             "59 - SMBX 1.2.2 (with some patch)\n"
                                             "64 - SMBX 1.3\n"),
                                        LvlData.RecentFormatVersion, 0, 64, 1, &ok);
            if( !ok )
                return false;
            LvlData.RecentFormatVersion = file_format;
        } else {
            LvlData.RecentFormatVersion = 64;
        }

        if(selectedFilter==filePGEX)
            LvlData.RecentFormat=LevelData::PGEX;
        else if((selectedFilter==fileSMBX64) || (selectedFilter==fileSMBXany))
            LvlData.RecentFormat=LevelData::SMBX64;
        else if((selectedFilter==fileSMBX38A))
        {
            QMessageBox::information(this, "====THIS FILE FORMAT IS EXPERIMENTAL====",
                                     "Saving into SMBX-38A Level file format is experimental!\n"
                                     "Some values are may be lost or distorted. Please also save same file into another file format until reload or close it!");

            LvlData.RecentFormat=LevelData::SMBX38A;
        }

        if( (selectedFilter==fileSMBXany) || (selectedFilter==fileSMBX64) || (selectedFilter==fileSMBX38A) )
        {
            if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.remove(fileName.size()-1, 1);
            if(!fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append(".lvl");
        }
        else if(selectedFilter==filePGEX)
        {
            if(fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append("x");
            if(!fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.append(".lvlx");
        }

        if( (!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)) )
        {
            QMessageBox::warning(this, tr("Extension is not set"),
               tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
            continue;
        }

        if(makeCustomFolder)
        {
            QFileInfo finfo(fileName);
            finfo.absoluteDir().mkpath(finfo.absoluteDir().absolutePath()+
                                      "/"+util::getBaseFilename(finfo.fileName())
                                      );
        }
        isNotDone=false;
    }

    ret = saveFile(fileName, true, &isSMBX64limit );
    if(isSMBX64limit) goto RetrySave;

    return ret;
}

bool LevelEdit::saveFile(const QString &fileName, const bool addToRecent, bool *out_WarningIsAborted)
{
    if( (!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)) )
    {
        QMessageBox::warning(this, tr("Extension is not set"),
           tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //Mark stars
    for(int q=0; q< LvlData.npc.size(); q++)
    {
        if(LvlData.npc[q].id<=0) continue;
        LvlData.npc[q].is_star = MainWinConnect::pMainWin->configs.main_npc[LvlData.npc[q].id].is_star;
        if((LvlData.npc[q].is_star) && (LvlData.npc[q].friendly))
            LvlData.npc[q].is_star=false;
    }

    // ////////////////// Write Extended LVL file (LVLX)/////////////////////
    if(LvlData.RecentFormat==LevelData::PGEX)
    {
        if(!savePGEXLVL(fileName, false))
            return false;
        LvlData.smbx64strict = false; //Disable strict mode
    }
    // //////////////////////////////////////////////////////////////////////
    // ////////////////////// Write SMBX64 LVL //////////////////////////////
    else if(LvlData.RecentFormat==LevelData::SMBX64)
    {
        if(!saveSMBX64LVL(fileName, false, out_WarningIsAborted))
            return false;
        LvlData.smbx64strict = true; //Enable SMBX64 standard strict mode
    }
    // //////////////////////////////////////////////////////////////////////
    // ////////////////////// Write SMBX-38A LVL //////////////////////////////
    else if(LvlData.RecentFormat==LevelData::SMBX38A)
    {
        if(!saveSMBX38aLVL(fileName, false))
            return false;
        LvlData.smbx64strict = false; //Disable strict mode
    }
    // //////////////////////////////////////////////////////////////////////

    QFileInfo finfo(fileName);
    GlobalSettings::savePath = finfo.path();
    LvlData.path = finfo.path();
    LvlData.filename = util::getBaseFilename(finfo.fileName());

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);


    LvlData.modified = false;
    LvlData.untitled = false;
    if(addToRecent)
    {
        MainWinConnect::pMainWin->AddToRecentFiles(fileName);
        MainWinConnect::pMainWin->SyncRecentFiles();
    }

    //Refresh Strict SMBX64 flag
    emit MainWinConnect::pMainWin->setSMBX64Strict(LvlData.smbx64strict);

    return true;
}

bool LevelEdit::savePGEXLVL(QString fileName, bool silent)
{
    if(!FileFormats::SaveLevelFile(LvlData, fileName, FileFormats::LVL_PGEX))
    {
        if(!silent)
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(FileFormats::errorString));
        return false;
    }
    return true;
}


bool LevelEdit::saveSMBX38aLVL(QString fileName, bool silent)
{
    if(!FileFormats::SaveLevelFile(LvlData, fileName, FileFormats::LVL_SMBX38A))
    {
        if(!silent)
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(FileFormats::errorString));
        return false;
    }
    return true;
}



bool LevelEdit::saveSMBX64LVL(QString fileName, bool silent, bool *out_WarningIsAborted)
{
    //SMBX64 Standard check
    bool isSMBX64limit=false;

    if(out_WarningIsAborted)
        *out_WarningIsAborted=false;

    int ErrorCode=FileFormats::smbx64LevelCheckLimits(LvlData);
    if(ErrorCode!=FileFormats::SMBX64_FINE)
    {
        if(!silent)
            QMessageBox::warning(this,
                                 tr("The SMBX64 limit has been exceeded"),
                                 smbx64ErrMsgs(LvlData, ErrorCode),
                                 QMessageBox::Ok);
        isSMBX64limit=true;
    }

    if( isSMBX64limit )
    {
        if(!silent)
        {
            if(!silent)
                QApplication::restoreOverrideCursor();
            if(QMessageBox::question(this,
                tr("The SMBX64 limit has been exceeded"),
                tr("Do you want to save file anyway?\nExciting of SMBX64 limits may crash SMBX with 'Subscript out of range' error.\n\nInstalled LunaLUA partially extends than limits."),
                QMessageBox::Yes|QMessageBox::No)==QMessageBox::No)
            {
                if(out_WarningIsAborted)
                    *out_WarningIsAborted = isSMBX64limit;
                return false;
            } else {
                isSMBX64limit=false;
            }
            QApplication::setOverrideCursor(Qt::WaitCursor);
        }
    }

    if(!FileFormats::SaveLevelFile(LvlData, fileName, FileFormats::LVL_SMBX64, LvlData.RecentFormatVersion))
    {
        QApplication::restoreOverrideCursor();
        if(!silent)
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(FileFormats::errorString));
        return false;
    }
    QApplication::restoreOverrideCursor();
    return true;
}



bool LevelEdit::loadFile(const QString &fileName, LevelData &FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    LvlData.metaData.script.reset(new ScriptHolder());
    bool modifystate = false;
    bool untitledstate = false;
    LvlData.modified = false;
    LvlData.untitled = false;
    QString curFName=fileName;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset
    setCurrentFile(curFName);

    //Restore internal information after crash
    if(LvlData.metaData.crash.used)
    {
        modifystate=LvlData.metaData.crash.modifyed;
        untitledstate=LvlData.metaData.crash.untitled;
        isUntitled = LvlData.metaData.crash.untitled;
        LvlData.filename = LvlData.metaData.crash.filename;
        LvlData.path = LvlData.metaData.crash.path;
        curFName = LvlData.metaData.crash.fullPath;
        setCurrentFile(LvlData.metaData.crash.fullPath);
        LvlData.metaData.crash.reset();
    }

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    LogDebug(QString(">>Starting to load file"));

    //Declaring of the scene
    scene = new LvlScene(ui->graphicsView, configs, LvlData, this);

    scene->opts = options;

    int DataSize=0;

    DataSize += 3;
    DataSize += 6; /*LvlData.sections.size()*2;
    DataSize += LvlData.bgo.size();
    DataSize += LvlData.blocks.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();*/

    QProgressDialog progress(tr("Loading level data"), tr("Abort"), 0, DataSize, MainWinConnect::pMainWin);
         progress.setWindowTitle(tr("Loading level data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(500);
         //progress.setCancelButton(0);

    if(! DrawObjects(progress) )
    {
        LvlData.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    QApplication::restoreOverrideCursor();

    if(options.animationEnabled)
    {
        scene->startAnimation();
    } else {
        stopAutoUpdateTimer();
    }

    setCurrentFile(curFName);
    LvlData.modified = modifystate;
    LvlData.untitled = untitledstate;

    progress.deleteLater();

    return true;
}


void LevelEdit::documentWasModified()
{
    //LvlData.modified = true;
}

bool LevelEdit::maybeSave()
{
    if (LvlData.modified)
    {
        SavingNotificationDialog* sav = new SavingNotificationDialog(true, SavingNotificationDialog::D_WARN, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile()+tr(" not saved"));
        QLineEdit* lvlNameBox = new QLineEdit();
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->setAdjustSize(400,130);
        lvlNameBox->setText(LvlData.LevelName);
        if(sav->exec() == QDialog::Accepted){
            LvlData.LevelName = lvlNameBox->text();
            lvlNameBox->deleteLater();
            sav->deleteLater();
            if(sav->savemode == SavingNotificationDialog::SAVE_SAVE){
                return save(false);
            }else if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE){
                return false;
            }
        }else{
            return false;
        }
    }

    return true;
}


//////// Common
QString LevelEdit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void LevelEdit::makeCrashState()
{
    this->isUntitled = true;
    this->LvlData.modified = true;
    this->LvlData.untitled = true;
}

void LevelEdit::closeEvent(QCloseEvent *event)
{
    if(!sceneCreated)
    {
        event->accept();
        goto clearScene;
        return;
    }
    else
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
    }

    if( maybeSave() )
    {
        goto clearScene;
    } else {
        event->ignore();
    }

    return;
clearScene:
    if(scene)
    {
        stopAutoUpdateTimer();
        scene->setMessageBoxItem(false);
        scene->clear();
        LogDebug("!<-Cleared->!");

        LogDebug("!<-Delete animators->!");
        while(! scene->animates_BGO.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_BGO.first();
            scene->animates_BGO.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_Blocks.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Blocks.first();
            scene->animates_Blocks.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_NPC.isEmpty() )
        {
            AdvNpcAnimator* tmp = scene->animates_NPC.first();
            scene->animates_NPC.pop_front();
            if(tmp!=NULL) delete tmp;
        }

        scene->uBGOs.clear();
        scene->uBGs.clear();
        scene->uBlocks.clear();
        scene->uNPCs.clear();

        LogDebug("!<-Delete scene->!");
        sceneCreated = false;
        delete scene;
        scene = NULL;
        LogDebug("!<-Deleted->!");
    }
}

void LevelEdit::setCurrentFile(const QString &fileName)
{
    QFileInfo info(fileName);
    curFile = info.canonicalFilePath();
    isUntitled = false;
    LvlData.path = info.absoluteDir().absolutePath();
    LvlData.filename = util::getBaseFilename(info.fileName());
    LvlData.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(QString(LvlData.LevelName=="" ? userFriendlyCurrentFile() : LvlData.LevelName).replace("&", "&&&"));
}

QString LevelEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

