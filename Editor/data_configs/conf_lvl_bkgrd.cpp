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

#include <common_features/app_path.h>
#include <common_features/themes.h>
#include <common_features/graphics_funcs.h>
#include <main_window/global_settings.h>

#include "data_configs.h"

obj_BG::obj_BG()
{
    isValid = false;
    animator_id = 0;
    cur_image = NULL;
    cur_image_second = NULL;
}

void obj_BG::copyTo(obj_BG &bg)
{
    /* for internal usage */
    bg.isValid         = isValid;
    bg.animator_id     = animator_id;
    bg.cur_image       = cur_image;
    if(cur_image==NULL)
        bg.cur_image   = &image;
    if(cur_image_second==NULL)
        bg.cur_image_second   = &image;
    bg.frame_h         = frame_h;
    /* for internal usage */
}

bool dataconfigs::loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString errStr, tmpstr, imgFile;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    setup->beginGroup( section );
        sbg.name = setup->value("name", (merge_with? merge_with->name : "") ).toString();
        if(sbg.name.isEmpty())
        {
            addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
            valid=false;
            goto abort;
        }
        tmpstr = setup->value("type", "-1").toString();
            if(tmpstr=="single-row")
               sbg.type = 0;
            else if(tmpstr=="double-row")
               sbg.type = 1;
            else if(tmpstr=="tiled")
               sbg.type = 2;
            else if(tmpstr=="-1")
               sbg.type = (merge_with ? merge_with->type : 0);
            else sbg.type = 0;


        sbg.repeat_h = qFabs(setup->value("repeat-h", (merge_with ? merge_with->repeat_h : 2.0f)).toFloat());

        tmpstr = setup->value("repeat-v", "-1").toString();
            if(tmpstr=="NR")
                sbg.repead_v = 0;
            else if(tmpstr=="ZR")
                sbg.repead_v = 1;
            else if(tmpstr=="RP")
                sbg.repead_v = 2;
            else if(tmpstr=="RZ")
                sbg.repead_v = 3;
            else if(tmpstr=="-1")
                sbg.repead_v = (merge_with ? merge_with->repead_v : 0);
            else sbg.repead_v = 0;

        sbg.image_n = setup->value("image", (merge_with ? merge_with->image_n : "") ).toString();
        if(!merge_with)
        {
            if( (sbg.image_n !="") )
            {
                GraphicsHelps::loadMaskedImage(BGPath,
                    sbg.image_n, imgFile,
                    sbg.image,
                    errStr);
                if(!errStr.isEmpty())
                {
                    addError(QString("%1 %2").arg(section).arg(errStr));
                    valid=false;
                    goto abort;
                }
            } else {
                addError(QString("%1 Image filename isn't defined").arg(section));
                valid=false;
                goto abort;
            }
        }

        sbg.attached =    (int)(setup->value("attached",
                                             (merge_with?(merge_with->attached==1?"top":"bottom"):"bottom") ).toString()=="top");
        sbg.editing_tiled =    setup->value("tiled-in-editor", merge_with?merge_with->editing_tiled:false ).toBool();

        sbg.magic =             setup->value("magic", (merge_with?merge_with->magic:false)).toBool();
        sbg.magic_strips =      setup->value("magic-strips", (merge_with? merge_with->magic_strips:1)).toInt();
        sbg.magic_splits =      setup->value("magic-splits", (merge_with? merge_with->magic_splits:"0")).toString();
        sbg.magic_speeds =      setup->value("magic-speeds", (merge_with? merge_with->magic_speeds:"0")).toString();

        sbg.animated =          setup->value("animated", (merge_with?merge_with->animated:false)).toBool();//animated
        sbg.frames =            setup->value("frames", (merge_with?merge_with->frames:1)).toInt();
        sbg.framespeed =        setup->value("framespeed", (merge_with?merge_with->framespeed:128)).toInt();
        sbg.display_frame =     setup->value("display-frame", (merge_with?merge_with->display_frame:0)).toInt();
        //frames

        if(sbg.type==1)
        {
            sbg.second_image_n = setup->value("second-image", (merge_with ? merge_with->second_image_n : "")).toString();
            if(!merge_with)
            {
                if( (sbg.second_image_n !="") )
                {
                    GraphicsHelps::loadMaskedImage(BGPath,
                       sbg.second_image_n, imgFile,
                       sbg.second_image,
                       errStr);
                } else {
                    sbg.second_image = Themes::Image(Themes::dummy_bg);
                }
            }

            sbg.second_repeat_h = qFabs(setup->value("second-repeat-h", (merge_with ? merge_with->second_repeat_h : 2.0f)).toFloat());

            tmpstr = setup->value("second-repeat-v", "-1").toString();
                if(tmpstr=="NR")
                    sbg.second_repeat_v = 0;
                else if(tmpstr=="ZR")
                    sbg.second_repeat_v = 1;
                else if(tmpstr=="RP")
                    sbg.second_repeat_v = 2;
                else if(tmpstr=="RZ")
                    sbg.second_repeat_v = 3;
                else if(tmpstr=="-1")
                    sbg.second_repeat_v = (merge_with ? merge_with->second_repeat_v : 0);
                else sbg.second_repeat_v = 0;

            tmpstr = setup->value("second-attached", "-1").toString();
                if(tmpstr=="overfirst")
                    sbg.second_attached = 0;
                else if(tmpstr=="bottom")
                    sbg.second_attached = 1;
                else if(tmpstr=="top")
                    sbg.second_attached = 2;
                else if(tmpstr=="-1")
                    sbg.second_attached = (merge_with ? merge_with->second_attached : 0);
                else sbg.second_repeat_v = 0;
        }

        if(sbg.animated)
        {
            sbg.image = sbg.image.copy(0, 0, sbg.image.width(), (int)round(sbg.image.height()/sbg.frames));
        }

        sbg.isValid = true;

    abort:
        setup->endGroup();
        if(internal) delete setup;
    return valid;
}


void dataconfigs::loadLevelBackgrounds()
{
    unsigned int i;
    obj_BG sbg;
    unsigned long bg_total=0;

    QString bg_ini = config_dir + "lvl_bkgrd.ini";

    if(!QFile::exists(bg_ini))
    {
        addError(QString("ERROR LOADING lvl_bkgrd.ini: file does not exist"), PGE_LogLevel::Critical);
          return;
    }

    QSettings bgset(bg_ini, QSettings::IniFormat);
    bgset.setIniCodec("UTF-8");

    main_bg.clear();   //Clear old

    bgset.beginGroup("background2-main");
        bg_total = bgset.value("total", "0").toInt();
        total_data +=bg_total;
    bgset.endGroup();

    emit progressPartNumber(0);
    emit progressMax(bg_total);
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Backgrounds..."));

    ConfStatus::total_bg = bg_total;
    main_bg.allocateSlots(ConfStatus::total_bg);

    if(ConfStatus::total_bg==0)
    {
        addError(QString("ERROR LOADING lvl_bkgrd.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=bg_total; i++)
    {
        emit progressValue(i);

        if( loadLevelBackground(sbg, QString("background2-"+QString::number(i)), 0, "", &bgset) )
        {
            sbg.id = i;
            main_bg.storeElement(i, sbg);
        }

        if( bgset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgrnd.ini N:%1 (background2-%2)").arg(bgset.status()).arg(i), PGE_LogLevel::Critical);
        }
    }
}
