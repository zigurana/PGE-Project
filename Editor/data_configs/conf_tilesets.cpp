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

#include <tools/tilesets/tilesetgroupeditor.h>

#include "data_configs.h"

void dataconfigs::loadTilesets()
{
    main_tilesets.clear();
    main_tilesets_grp.clear();

    QString tileset_dir = config_dir + "tilesets/";
    QString tileset_grp_dir = config_dir + "group_tilesets/";
    QStringList filters;

    if(QDir(tileset_dir).exists())
    {
        emit progressPartNumber(10);
        emit progressMax(100);
        emit progressValue(0);
        emit progressTitle(QObject::tr("Loading Tilesets..."));

        filters.clear();
        filters << "*.tileset.ini";
        QDir tilesetDir(tileset_dir);
        tilesetDir.setSorting(QDir::Name);
        tilesetDir.setNameFilters(filters);
        QStringList files = tilesetDir.entryList(filters);

        emit progressMax(files.size());
        for(int i=0;i<files.size(); i++)
        {
            emit progressValue(i);
            SimpleTileset xxx;
            if(tileset::OpenSimpleTileset(tileset_dir + files[i], xxx))
            {
                main_tilesets.push_back(xxx);
            }
        }
    }

    if(QDir(tileset_grp_dir).exists())
    {
        emit progressPartNumber(11);
        emit progressMax(100);
        emit progressValue(0);
        emit progressTitle(QObject::tr("Loading Tileset groups..."));

        filters.clear();
        filters << "*.tsgrp.ini";
        QDir tilesetDir(tileset_grp_dir);
        tilesetDir.setSorting(QDir::Name);
        tilesetDir.setNameFilters(filters);

        QStringList files = tilesetDir.entryList(filters);
        emit progressMax(files.size());
        for(int i=0;i<files.size(); i++)
        {
            emit progressValue(i);
            SimpleTilesetGroup xxx;
            if(TilesetGroupEditor::OpenSimpleTilesetGroup(tileset_grp_dir + files[i], xxx))
            {
                main_tilesets_grp.push_back(xxx);
            }
        }
    }
}
