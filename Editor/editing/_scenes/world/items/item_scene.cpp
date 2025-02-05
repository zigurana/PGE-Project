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

#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/util.h>
#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>

#include "item_scene.h"

ItemScene::ItemScene(QGraphicsItem *parent)
    : WldBaseItem(parent)
{
    construct();

}

ItemScene::ItemScene(WldScene *parentScene, QGraphicsItem *parent)
    : WldBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    m_scene->addItem(this);
    setZValue(m_scene->sceneZ);
}


void ItemScene::construct()
{
    m_gridSize=16;
    setData(ITEM_TYPE, "SCENERY");
}

ItemScene::~ItemScene()
{
    m_scene->unregisterElement(this);
}

void ItemScene::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_scene->contextMenuOpened = true; //bug protector
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    setSelected(true);//minor, but so dumb mistake was here: "this" instead of "true"
    QMenu ItemMenu;

    QMenu * copyPreferences = ItemMenu.addMenu(tr("Copy preferences"));
        copyPreferences->deleteLater();
            QAction *copyItemID = copyPreferences->addAction(tr("Scenery-ID: %1").arg(m_data.id));
                copyItemID->deleteLater();
            QAction *copyPosXY = copyPreferences->addAction(tr("Position: X, Y"));
                copyPosXY->deleteLater();
            QAction *copyPosXYWH = copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
                copyPosXYWH->deleteLater();
            QAction *copyPosLTRB = copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
                copyPosLTRB->deleteLater();

    QAction *copyTile = ItemMenu.addAction(tr("Copy"));
    QAction *cutTile = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("SCENERY-%1").arg(m_data.id));
        ItemMenu.addSeparator();
    QAction *remove = ItemMenu.addAction(tr("Remove"));

QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        #endif
        return;
    }

    if(selected==copyItemID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.id));
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXY)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXYWH)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2; W=%3; H=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_imageSize.width())
                               .arg(m_imageSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosLTRB)
    {
        QApplication::clipboard()->setText(
                            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_data.x+m_imageSize.width())
                               .arg(m_data.y+m_imageSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==cutTile)
    {
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyTile)
    {
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if((selected==transform)||(selected==transform_all))
    {
        WorldData oldData;
        WorldData newData;
        int transformTO;

        ItemSelectDialog * itemList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_SCENERY,0,0,0,0,0,0,0,0,0, m_scene->_edit);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_SCENERY);
        util::DialogToCenter(itemList, true);

        if(itemList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = itemList->sceneryID;
            unsigned long oldID = m_data.id;

            if(selected==transform)
                our_items=m_scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=m_scene->items();
                sameID=true;
            }

            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="SCENERY")
                {
                    if((!sameID)||(((ItemScene *) SelItem)->m_data.id==oldID))
                    {
                        oldData.scenery.push_back( ((ItemScene *) SelItem)->m_data );
                        ((ItemScene *) SelItem)->transformTo(transformTO);
                        newData.scenery.push_back( ((ItemScene *) SelItem)->m_data );
                    }
                }
            }
        }
        delete itemList;
        if(!newData.scenery.isEmpty())
            m_scene->addTransformHistory(newData, oldData);
    }
    else
    if(selected==remove)
    {
        m_scene->removeSelectedWldItems();
    }
}


///////////////////MainArray functions/////////////////////////////
//void ItemScene::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            sceneData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemScene::transformTo(long target_id)
{
    if(target_id<1) return;

    if(!m_scene->uScenes.contains(target_id))
        return;

    obj_w_scenery &mergedSet = m_scene->uScenes[target_id];
    long animator=mergedSet.animator_id;

    m_data.id = target_id;
    setSceneData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->opts.animationEnabled)
        m_scene->update();
}

void ItemScene::arrayApply()
{
    bool found=false;

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.index < (unsigned int)m_scene->WldData->scenery.size())
    { //Check index
        if(m_data.array_id == m_scene->WldData->scenery[m_data.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->WldData->scenery[m_data.index] = m_data; //apply current sceneData
    }
    else
    for(int i=0; i<m_scene->WldData->scenery.size(); i++)
    { //after find it into array
        if(m_scene->WldData->scenery[i].array_id == m_data.array_id)
        {
            m_data.index = i;
            m_scene->WldData->scenery[i] = m_data;
            break;
        }
    }
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemScene::removeFromArray()
{
    bool found=false;
    if(m_data.index < (unsigned int)m_scene->WldData->scenery.size())
    { //Check index
        if(m_data.array_id == m_scene->WldData->scenery[m_data.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        m_scene->WldData->scenery.removeAt(m_data.index);
    }
    else
    for(int i=0; i<m_scene->WldData->scenery.size(); i++)
    {
        if(m_scene->WldData->scenery[i].array_id == m_data.array_id)
        {
            m_scene->WldData->scenery.removeAt(i); break;
        }
    }
}

void ItemScene::returnBack()
{
    setPos(m_data.x, m_data.y);
}

QPoint ItemScene::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}

bool ItemScene::itemTypeIsLocked()
{
    return m_scene->lock_scene;
}

void ItemScene::setSceneData(WorldScenery inD, obj_w_scenery *mergedSet, long *animator_id)
{
    m_data = inD;
    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.array_id) );

    setPos(m_data.x, m_data.y);
    if(mergedSet)
    {
        m_localProps = *mergedSet;
        m_gridSize = m_localProps.grid;
    }
    if(animator_id)
        setAnimator(*animator_id);

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

QRectF ItemScene::boundingRect() const
{
    return m_imageSize;
}

void ItemScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(m_scene->animates_Scenery.size()>m_animatorID)
        painter->drawPixmap(m_imageSize,
                            m_scene->animates_Scenery[m_animatorID]->wholeImage(),
                            m_scene->animates_Scenery[m_animatorID]->frameRect());
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::DotLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
}

////////////////Animation///////////////////

void ItemScene::setAnimator(long aniID)
{
    if(aniID<m_scene->animates_Scenery.size())
    {
        QRect frameRect = m_scene->animates_Scenery[aniID]->frameRect();
        m_imageSize = QRectF(0,0, frameRect.width(), frameRect.height() );
    }

    this->setData(ITEM_WIDTH, QString::number( m_gridSize ) ); //width
    this->setData(ITEM_HEIGHT, QString::number( m_gridSize ) ); //height
    m_animatorID = aniID;
}


