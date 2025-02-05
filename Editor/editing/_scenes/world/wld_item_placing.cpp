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

#include <common_features/items.h>
#include <PGE_File_Formats/file_formats.h>
#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"
#include "wld_item_placing.h"

/*
    static LevelNPC dummyLvlNpc();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelWater dummyLvlWater();
    static LevelEvents dummyLvlEvent();
*/

//Default dataSets

WorldTiles WldPlacingItems::TileSet=FileFormats::CreateWldTile();
WorldScenery WldPlacingItems::SceneSet=FileFormats::CreateWldScenery();
WorldPaths WldPlacingItems::PathSet=FileFormats::CreateWldPath();
WorldLevels WldPlacingItems::LevelSet=FileFormats::CreateWldLevel();
WorldMusic WldPlacingItems::MusicSet=FileFormats::CreateWldMusicbox();

int WldPlacingItems::c_offset_x=0;
int WldPlacingItems::c_offset_y=0;

int WldPlacingItems::itemW=0;
int WldPlacingItems::itemH=0;

int WldPlacingItems::gridSz=1;
QPoint WldPlacingItems::gridOffset=QPoint(0,0);

bool WldPlacingItems::overwriteMode=false;
WldPlacingItems::PlaceMode WldPlacingItems::placingMode=WldPlacingItems::PMODE_Brush;

QList<QPair<int, QVariant > > WldPlacingItems::flags;

void WldScene::setItemPlacer(int itemType, unsigned long itemID)
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    LogDebug(QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    switch(itemType)
    {
    case 0: //Tiles
    {
        obj_w_tile &tileConf = uTiles[itemID];
        Items::getItemGFX(&tileConf, tImg, false);
        if(tImg.isNull())
        {
            tImg=uTileImg;
        }
        if(!tileConf.isValid)
        {
            tileConf = pConfigs->main_wtiles[1];
            tileConf.image = uTileImg;
        }

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::TileSet.id = itemID;


        long w = tImg.width();
        long h = tImg.height();

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;


        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=ITEM_TYPE;
            flag.second="TILE";
        WldPlacingItems::flags.push_back(flag);

            flag.first=ITEM_ID;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_WIDTH;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_HEIGHT;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_IS_CURSOR;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer(); return;
        }
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg);

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Tile;
        WldPlacingItems::TileSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller(); return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }
    case 1: //Sceneries
    {
        obj_w_scenery &sceneConf = uScenes[itemID];
        Items::getItemGFX(&sceneConf, tImg, false);
        if(tImg.isNull())
        {
            tImg=uSceneImg;
        }
        if(!sceneConf.isValid)
        {
            sceneConf = pConfigs->main_wscene[1];
            sceneConf.image = uSceneImg;
        }

        WldPlacingItems::gridSz=qRound(qreal(pConfigs->default_grid)/2);
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::SceneSet.id = itemID;

        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=ITEM_TYPE;
            flag.second="SCENERY";
        WldPlacingItems::flags.push_back(flag);

            flag.first=ITEM_ID;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_WIDTH;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_HEIGHT;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_IS_CURSOR;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(tImg.width()) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(tImg.height()) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer(); return;
        }

        cursor = addPixmap(tImg);

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(ITEM_IS_CURSOR, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Scene;
        WldPlacingItems::SceneSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller(); return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }
    case 2: //Path
    {
        obj_w_path &pathConf = uPaths[itemID];
        Items::getItemGFX(&pathConf, tImg, false);
        if(tImg.isNull())
        {
            tImg=uPathImg;
        }
        if(!pathConf.isValid)
        {
            pathConf = pConfigs->main_wpaths[1];
            pathConf.image = uPathImg;
        }

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::PathSet.id = itemID;

        long w = tImg.width();
        long h = tImg.height();

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=ITEM_TYPE;
            flag.second="PATH";
        WldPlacingItems::flags.push_back(flag);

            flag.first=ITEM_ID;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_WIDTH;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_HEIGHT;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_IS_CURSOR;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer(); return;
        }
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg);

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(ITEM_IS_CURSOR, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Path;
        WldPlacingItems::PathSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller(); return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }

    case 3: //Level
    {
        obj_w_level &wlevelConf = uLevels[itemID];
        Items::getItemGFX(&wlevelConf, tImg, false);
        if(tImg.isNull())
        {
            tImg = uLevelImg;
        }
        if(!wlevelConf.isValid)
        {
            wlevelConf = pConfigs->main_wlevels[0];
            wlevelConf.image = uLevelImg;
        }

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::LevelSet.id = itemID;

        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=ITEM_TYPE;
            flag.second="LEVEL";
        WldPlacingItems::flags.push_back(flag);

            flag.first=ITEM_ID;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_WIDTH;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_HEIGHT;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = ITEM_IS_CURSOR;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer(); return;
        }
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer(); return;
        }

        WldPlacingItems::c_offset_x = qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y = qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg);

        int imgOffsetX = (int)qRound( -( qreal(tImg.width()) - qreal(WldPlacingItems::gridSz))  / 2 );
        int imgOffsetY = (int)qRound( -qreal(tImg.height()) + WldPlacingItems::gridSz);

        ((QGraphicsPixmapItem*)cursor)->setOffset(imgOffsetX, imgOffsetY );

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(ITEM_IS_CURSOR, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Level;
        WldPlacingItems::LevelSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller(); return;
        }
        SwitchEditingMode(MODE_PlacingNew);
        break;
    }

    case 4: //MusicBox
        placingItem=PLC_Musicbox;
        WldPlacingItems::MusicSet.id = itemID;

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0,0);

        WldPlacingItems::c_offset_x = pConfigs->default_grid/2;
        WldPlacingItems::c_offset_y = pConfigs->default_grid/2;

        cursor = addRect(0,0, pConfigs->default_grid, pConfigs->default_grid);
        cursor->setData(ITEM_TYPE, "MUSICBOX");
        cursor->setData(ITEM_ID, QString::number(itemID));
        cursor->setData(ITEM_WIDTH, QString::number(pConfigs->default_grid));
        cursor->setData(ITEM_HEIGHT, QString::number(pConfigs->default_grid));
        ((QGraphicsRectItem *)cursor)->setBrush(QBrush(Qt::yellow));
        ((QGraphicsRectItem *)cursor)->setPen(QPen(Qt::yellow, 2,Qt::SolidLine));
        cursor->setData(ITEM_IS_CURSOR, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        SwitchEditingMode(MODE_PlacingNew);

        break;
    case 5: //Get point from a world map
        placingItem=MODE_SetPoint;
        WldPlacingItems::MusicSet.id = itemID;

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0,0);

        WldPlacingItems::c_offset_x = pConfigs->default_grid/2;
        WldPlacingItems::c_offset_y = pConfigs->default_grid/2;

        cursor = addRect(0,0, pConfigs->default_grid, pConfigs->default_grid);
        cursor->setData(ITEM_TYPE, "WorldMapPoint");
        cursor->setData(ITEM_ID, QString::number(itemID));
        cursor->setData(ITEM_WIDTH, QString::number(pConfigs->default_grid));
        cursor->setData(ITEM_HEIGHT, QString::number(pConfigs->default_grid));
        ((QGraphicsRectItem *)cursor)->setBrush(QBrush(Qt::yellow));
        ((QGraphicsRectItem *)cursor)->setPen(QPen(Qt::yellow, 2,Qt::SolidLine));
        cursor->setData(ITEM_IS_CURSOR, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        SwitchEditingMode(MODE_SetPoint);

        // restore last point
        if(!selectedPointNotUsed) setPoint(selectedPoint);

        break;
        default: break;
    }
    if(itemType!=5) SwitchEditingMode(MODE_PlacingNew);
    contextMenuOpened=false;
}



void WldScene::setRectDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;
    QBrush brush;


    pen = QPen(Qt::gray, 2);
    brush = QBrush(Qt::darkGray);

    //Align width and height to fit into item aligning
    long addW=WldPlacingItems::gridSz-WldPlacingItems::itemW%WldPlacingItems::gridSz;
    long addH=WldPlacingItems::gridSz-WldPlacingItems::itemH%WldPlacingItems::gridSz;
    if(addW==WldPlacingItems::gridSz) addW=0;
    if(addH==WldPlacingItems::gridSz) addH=0;
    WldPlacingItems::itemW = WldPlacingItems::itemW+addW;
    WldPlacingItems::itemH = WldPlacingItems::itemH+addH;

    QPixmap oneCell(WldPlacingItems::itemW, WldPlacingItems::itemH);
    oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
    QPainter p(&oneCell);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
    p.drawRect(0,0, WldPlacingItems::itemW, WldPlacingItems::itemH);
    brush.setTexture(oneCell);

    cursor = addRect(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag_w flag, WldPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Square");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_DrawRect);
}

void WldScene::setCircleDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;
    QBrush brush;


    pen = QPen(Qt::gray, 2);
    brush = QBrush(Qt::darkGray);

    //Align width and height to fit into item aligning
    long addW=WldPlacingItems::gridSz-WldPlacingItems::itemW%WldPlacingItems::gridSz;
    long addH=WldPlacingItems::gridSz-WldPlacingItems::itemH%WldPlacingItems::gridSz;
    if(addW==WldPlacingItems::gridSz) addW=0;
    if(addH==WldPlacingItems::gridSz) addH=0;
    WldPlacingItems::itemW = WldPlacingItems::itemW+addW;
    WldPlacingItems::itemH = WldPlacingItems::itemH+addH;

    QPixmap oneCell(WldPlacingItems::itemW, WldPlacingItems::itemH);
    oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
    QPainter p(&oneCell);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
    p.drawRect(0,0, WldPlacingItems::itemW, WldPlacingItems::itemH);
    brush.setTexture(oneCell);

    cursor = addEllipse(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag_w flag, WldPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Circle");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_DrawCircle);
}

void WldScene::setLineDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;

    pen = QPen(Qt::transparent, 2);

    //Align width and height to fit into item aligning
    long addW=WldPlacingItems::gridSz-WldPlacingItems::itemW%WldPlacingItems::gridSz;
    long addH=WldPlacingItems::gridSz-WldPlacingItems::itemH%WldPlacingItems::gridSz;
    if(addW==WldPlacingItems::gridSz) addW=0;
    if(addH==WldPlacingItems::gridSz) addH=0;
    WldPlacingItems::itemW = WldPlacingItems::itemW+addW;
    WldPlacingItems::itemH = WldPlacingItems::itemH+addH;

    cursor = addLine(0,0,1,1, pen);

    //set data flags
    foreach(dataFlag_w flag, WldPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Line");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_Line);

}

void WldScene::setFloodFiller()
{
    SwitchEditingMode(MODE_Fill);
}

void WldScene::resetCursor()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    DrawMode=false;
    QPixmap cur(QSize(1,1));
    cur.fill(Qt::black);
    cursor = addPixmap(QPixmap(cur));
    cursor->setZValue(1000);
    cursor->hide();
}

void WldScene::setMessageBoxItem(bool show, QPointF pos, QString text)
{
    if(messageBox)
    {
        if(!show)
        {
            delete messageBox;
            messageBox = NULL;
            return;
        }

        if(text!=messageBox->text())
            messageBox->setText(text);
        messageBox->setPos(pos);
    }
    else
    {
        if(!show)
            return;

        QFont font;
        font.setFamily("Times");
        font.setWeight(99);
        font.setPointSize(25);
        messageBox = new QGraphicsSimpleTextItem(text);
        messageBox->setPen(QPen(QBrush(Qt::black), 2));
        messageBox->setBrush(QBrush(Qt::white));
        messageBox->setBoundingRegionGranularity(1);
        messageBox->setZValue(10000);
        messageBox->setFont(font);
        this->addItem(messageBox);
        messageBox->setPos(pos);
    }

}
