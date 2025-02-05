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

#ifndef ItemPath_H
#define ItemPath_H

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemPath : public WldBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemPath(QGraphicsItem *parent=0);
    ItemPath(WldScene *parentScene, QGraphicsItem *parent=0);
    ~ItemPath();

    void setPathData(WorldPaths inD, obj_w_path *mergedSet=0, long *animator_id=0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    //void setLayer(QString layer);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint sourcePos();

    WorldPaths m_data;
    obj_w_path m_localProps;

    bool itemTypeIsLocked();

    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // ItemPath_H
