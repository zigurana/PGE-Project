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

#include <main_window/global_settings.h>

#include "data_configs.h"

long dataconfigs::getSndI(unsigned long itemID)
{
    if((itemID>0) && main_sound.contains(itemID))
        return itemID;
    return 0;
}

void dataconfigs::loadSound()
{
    unsigned int i;

    obj_sound sound;

    unsigned long sound_total=0;

    QString sound_ini = config_dir + "sounds.ini";

    if(!QFile::exists(sound_ini))
    {
        addError(QString("ERROR LOADING sounds.ini: file does not exist"), PGE_LogLevel::Critical);
        return;
    }

    QSettings soundset(sound_ini, QSettings::IniFormat);
    soundset.setIniCodec("UTF-8");

    main_sound.clear();   //Clear old

    soundset.beginGroup("sound-main");
        sound_total = soundset.value("total", "0").toInt();

        total_data +=sound_total;
    soundset.endGroup();

    emit progressPartNumber(9);
    emit progressMax(sound_total);
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Sound..."));

    ConfStatus::total_sound = sound_total;


    if(ConfStatus::total_sound==0)
    {
        addError(QString("ERROR LOADING sounds.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }
    //////////////////////////////

    main_sound.allocateSlots(sound_total);
    //Sound
    for(i=1; i<=sound_total; i++)
    {
        emit progressValue(i);

        soundset.beginGroup( QString("sound-"+QString::number(i)) );
            sound.name = soundset.value("name", "").toString();
            if(sound.name.isEmpty())
            {
                addError(QString("Sound-%1 Item name isn't defined").arg(i));
                goto skipSoundFile;
            }
            sound.file = soundset.value("file", "").toString();
            if(sound.file.isEmpty())
            {
                addError(QString("Sound-%1 Item file isn't defined").arg(i));
                goto skipSoundFile;
            }
            sound.hidden = soundset.value("hidden", "0").toBool();
            sound.id = i;
            main_sound.storeElement(i, sound);
        skipSoundFile:
        soundset.endGroup();

        if( soundset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING sounds.ini N:%1 (sound %2)").arg(soundset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

}

