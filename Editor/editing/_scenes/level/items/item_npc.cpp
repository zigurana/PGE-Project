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

#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../itemmsgbox.h"
#include "../newlayerbox.h"

ItemNPC::ItemNPC(bool noScene, QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    m_DisableScene = noScene;
    construct();
}

ItemNPC::ItemNPC(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    m_DisableScene=false;
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    setLocked(m_scene->lock_npc);
}

void ItemNPC::construct()
{
    m_offset_x=0;
    m_offset_y=0;

    m_generatorArrow = NULL;
    m_includedNPC = NULL;
    m_animated = false;
    m_direction = -1;
    m_gridSize = 1;

    m_extAnimator=false;
    m_animatorID=-1;
    m_imageSize = QRectF(0,0,10,10);

    m_imgOffsetX=0;
    m_imgOffsetY=0;

    _internal_animator = NULL;

    setData(ITEM_TYPE, "NPC");
}


ItemNPC::~ItemNPC()
{
    if(m_includedNPC!=NULL) delete m_includedNPC;
    if(m_grp!=NULL) delete m_grp;
    if(!m_DisableScene) m_scene->unregisterElement(this);
    if(_internal_animator) delete _internal_animator;
}

void ItemNPC::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_DisableScene) return;

    m_scene->contextMenuOpened = true; //bug protector

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    /*************Layers*******************/
    QMenu * LayerName =         ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(m_data.layer).replace("&", "&&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;
    QAction * newLayer =        LayerName->addAction(tr("Add to new layer..."));
    LayerName->addSeparator()->deleteLater();

    foreach(LevelLayer layer, m_scene->LvlData->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer =              LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?""+tr("[hidden]"):"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==m_data.layer );
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QString NPCpath1 = m_scene->LvlData->path+QString("/npc-%1.txt").arg( m_data.id );
    QString NPCpath2 = m_scene->LvlData->path+"/"+m_scene->LvlData->filename+QString("/npc-%1.txt").arg( m_data.id );

    QAction *newNpc;
        if( (!m_scene->LvlData->untitled)&&((QFile().exists(NPCpath2)) || (QFile().exists(NPCpath1))) )
            newNpc =            ItemMenu.addAction(tr("Edit NPC-Configuration"));
        else
            newNpc =            ItemMenu.addAction(tr("New NPC-Configuration"));
        newNpc->setEnabled(!m_scene->LvlData->untitled);
                                ItemMenu.addSeparator();

    /*************Direction*******************/
    QMenu * chDir =             ItemMenu.addMenu(tr("Set %1").arg( (!m_localProps.direct_alt_title.isEmpty()) ? m_localProps.direct_alt_title : tr("Direction") ) );
    QAction *setLeft =          chDir->addAction( (!m_localProps.direct_alt_left.isEmpty()) ? m_localProps.direct_alt_left : tr("Left"));
        setLeft->setCheckable(true);
        setLeft->setChecked(m_data.direct==-1);

    QAction *setRand =          chDir->addAction(tr("Random"));
        setRand->setVisible( !m_localProps.direct_disable_random );
        setRand->setCheckable(true);
        setRand->setChecked(m_data.direct==0);

    QAction *setRight =         chDir->addAction( (!m_localProps.direct_alt_right.isEmpty()) ? m_localProps.direct_alt_right : tr("Right") );
        setRight->setCheckable(true);
        setRight->setChecked(m_data.direct==1);
                                ItemMenu.addSeparator();
    /*************Direction**end**************/

    QAction *fri =              ItemMenu.addAction(tr("Friendly"));
        fri->setCheckable(true);
        fri->setChecked( m_data.friendly );

    QAction *stat =             ItemMenu.addAction(tr("Doesn't move"));
        stat->setCheckable(true);
        stat->setChecked( m_data.nomove );

    QAction *msg =              ItemMenu.addAction(tr("Set message..."));
                                ItemMenu.addSeparator();

    QAction *boss =             ItemMenu.addAction(tr("Set as Boss"));
        boss->setCheckable(true);
        boss->setChecked( m_data.is_boss );
                                ItemMenu.addSeparator();

    QAction *transform =        ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all_s =  ItemMenu.addAction(tr("Transform all %1 in this section into").arg("NPC-%1").arg(m_data.id));
    QAction *transform_all =    ItemMenu.addAction(tr("Transform all %1 into").arg("NPC-%1").arg(m_data.id));
                                ItemMenu.addSeparator();

    QAction *chNPC = NULL;
    if(m_localProps.container)
    {
        chNPC =                 ItemMenu.addAction(tr("Change included NPC..."));
                                ItemMenu.addSeparator();
    }

    /*************Copy Preferences*******************/
    QMenu * copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyItemID =       copyPreferences->addAction(tr("NPC-ID: %1").arg(m_data.id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    QAction *copyNpc =          ItemMenu.addAction(tr("Copy"));
    QAction *cutNpc =           ItemMenu.addAction(tr("Cut"));
                                ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));
                                ItemMenu.addSeparator();
    QAction *props =            ItemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected==cutNpc)
    {
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyNpc)
    {
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if((selected==transform)||(selected==transform_all)||(selected==transform_all_s))
    {
        LevelData oldData;
        LevelData newData;

        int transformTO;
        ItemSelectDialog * npcList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_NPC,0,0,0,0,0,0,0,0,0, m_scene->_edit);
        npcList->removeEmptyEntry(ItemSelectDialog::TAB_NPC);
        util::DialogToCenter(npcList, true);

        if(npcList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = npcList->npcID;
            unsigned long oldID = m_data.id;

            if(selected==transform)
                our_items=m_scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=m_scene->items();
                sameID=true;
            }
            else if(selected==transform_all_s)
            {
                bool ok=false;
                long mg = QInputDialog::getInt(m_scene->_edit, tr("Margin of section"),
                               tr("Please select, how far items out of section should be removed too (in pixels)"),
                               32, 0, 214948, 1, &ok);
                if(!ok) goto cancelTransform;
                LevelSection &s=m_scene->LvlData->sections[m_scene->LvlData->CurSection];
                QRectF section;
                section.setLeft(s.size_left-mg);
                section.setTop(s.size_top-mg);
                section.setRight(s.size_right+mg);
                section.setBottom(s.size_bottom+mg);
                our_items=m_scene->items(section, Qt::IntersectsItemShape);
                sameID=true;
            }

            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    if((!sameID)||(((ItemNPC *) SelItem)->m_data.id==oldID))
                    {
                        oldData.npc.push_back( ((ItemNPC *) SelItem)->m_data );
                        ((ItemNPC *) SelItem)->transformTo(transformTO);
                        newData.npc.push_back( ((ItemNPC *) SelItem)->m_data );
                    }
                }
            }
            cancelTransform:;
        }
        delete npcList;
        if(!newData.npc.isEmpty())
            m_scene->addTransformHistory(newData, oldData);
    }
    else
    if(selected==chNPC)
    {
        LevelData selData;
        ItemSelectDialog * npcList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_NPC, 0,0,0,
                                                          m_data.contents,
                                                          0,0,0,0,0, m_scene->_edit);
        npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
        if(npcList->exec()==QDialog::Accepted)
        {
            //apply to all selected items.
            int selected_npc=0;
            if(npcList->npcID!=0){
                selected_npc = npcList->npcID;
            }

            foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                    ((ItemNPC *) SelItem)->setIncludedNPC(selected_npc);
                }
            }
            m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }
        delete npcList;
    }
    else
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
    if(selected==newNpc)
    {
        LogDebug(QString("NPC.txt path 1: %1").arg(NPCpath1));
        LogDebug(QString("NPC.txt path 2: %1").arg(NPCpath2));
        if( (!m_scene->LvlData->untitled) && (QFileInfo( NPCpath2 ).exists()) )
        {
            MainWinConnect::pMainWin->OpenFile( NPCpath2 );
        }
        else
        if( (!m_scene->LvlData->untitled) && (QFileInfo( NPCpath1 ).exists()) )
        {
            MainWinConnect::pMainWin->OpenFile( NPCpath1 );
        }
        else
        {
            NpcEdit *child = MainWinConnect::pMainWin->createNPCChild();
            child->newFile(m_data.id);
            child->show();
        }
    }
    else
    if(selected==fri)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_FRIENDLY, QVariant(fri->isChecked()));
    }
    else
    if(selected==stat)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_NOMOVEABLE, QVariant(stat->isChecked()));
    }
    else
    if(selected==msg)
    {
        LevelData selData;

        ItemMsgBox msgBox(Opened_By::NPC, m_data.msg, m_data.friendly, "", "", MainWinConnect::pMainWin);
        util::DialogToCenter(&msgBox, true);
        if(msgBox.exec()==QDialog::Accepted)
        {
            //apply to all selected items.
            foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                    ((ItemNPC *) SelItem)->setMsg( msgBox.currentText );
                    ((ItemNPC *) SelItem)->setFriendly( msgBox.isFriendlyChecked() );
                }
            }
            m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_MESSAGE, QVariant(msgBox.currentText));
            m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_FRIENDLY, QVariant(msgBox.isFriendlyChecked()));
        }
    }
    else
    if(selected==boss)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BOSS, QVariant(boss->isChecked()));
    }
    else
    if(selected==setLeft)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(-1);
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(-1));
    }
    if(selected==setRand)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(0);
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(0));
    }
    if(selected==setRight)
    {
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(1);
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_DIRECTION, QVariant(1));
    }
    else
    if(selected==remove)
    {
        m_scene->removeSelectedLvlItems();
    }
    else
    if(selected==props)
    {
        m_scene->openProps();
    }
    else
    if(selected==newLayer)
    {
        m_scene->setLayerToSelected();
    }
    else
    {
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                break;
            }//Find selected layer's item
        }
    }
}

//Change arrtibutes
void ItemNPC::setFriendly(bool fri)
{
    m_data.friendly=fri;
    arrayApply(); //Apply changes into array
}

void ItemNPC::setMsg(QString message)
{
    m_data.msg=message;
    arrayApply();//Apply changes into array
}

void ItemNPC::setNoMovable(bool stat)
{
    m_data.nomove=stat;
    arrayApply();//Apply changes into array
}

void ItemNPC::setLegacyBoss(bool boss)
{
    m_data.is_boss=boss;
    arrayApply();//Apply changes into array
}

void ItemNPC::changeDirection(int dir)
{
    m_data.direct = dir;
    if(dir==0) //if direction=random
    {
        dir=((0==qrand()%2)?-1:1); //set randomly 1 or -1
    }
    m_direction = dir;
    refreshOffsets();
    update();

    arrayApply();
}

void ItemNPC::transformTo(long target_id)
{
    if(!m_scene)
        return;

    if(target_id<1) return;
    if((!m_scene->uNPCs.contains(target_id))) return;

    obj_npc &mergedSet = m_scene->uNPCs[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;

    setNpcData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->opts.animationEnabled)
        m_scene->update();
}

void ItemNPC::setIncludedNPC(int npcID, bool init)
{
    if(m_DisableScene)
        return;

    if(m_includedNPC!=NULL)
    {
        m_grp->removeFromGroup(m_includedNPC);
        m_scene->removeItem(m_includedNPC);
        delete m_includedNPC;
        m_includedNPC = NULL;
    }
    if(npcID==0 || !m_localProps.container)
    {
        if( !init && (m_data.contents != 0) )
        {
            m_data.contents = 0;
            arrayApply();
        }
        return;
    }

    QPixmap npcImg = QPixmap( m_scene->getNPCimg( npcID ) );
    m_includedNPC = m_scene->addPixmap( npcImg );

    //Default included NPC pos
    m_includedNPC->setPos(
                (
                    this->scenePos().x()+qreal((qreal(m_localProps.width)-qreal(npcImg.width()))/qreal(2))
                 ),
                (
                    (m_scene->pConfigs->marker_npc.buried == m_data.id)?
                       this->scenePos().y()
                      :this->scenePos().y()+qreal((qreal(m_localProps.height)-qreal(npcImg.height()))/qreal(2))
                 ));

    if(m_scene->pConfigs->marker_npc.bubble != m_data.id)
    {
        m_includedNPC->setOpacity(qreal(0.4));
        m_includedNPC->setZValue(this->zValue() + 0.0000010);
    }
    else
        m_includedNPC->setZValue(this->zValue() - 0.0000010);
    m_grp->addToGroup(m_includedNPC);

    if(!init) m_data.contents = npcID;
    if(!init) arrayApply();
}

void ItemNPC::setGenerator(bool enable, int direction, int type, bool init)
{
    if(m_DisableScene)
        return;

    if(!init) m_data.generator = enable;

    if(m_generatorArrow!=NULL)
    {
        m_grp->removeFromGroup(m_generatorArrow);
        m_scene->removeItem(m_generatorArrow);
        delete m_generatorArrow;
        m_generatorArrow = NULL;
    }

    if(!enable)
    {
        if(!init) arrayApply();
        m_gridSize = m_localProps.grid;
        return;
    }
    else
    {
        m_generatorArrow = new QGraphicsPixmapItem;
        switch(type)
        {
        case 2:
            m_generatorArrow->setPixmap( QPixmap(":/npc/proj.png") );
            break;
        case 1:
        default:
            m_generatorArrow->setPixmap( QPixmap(":/npc/warp.png") );
            break;
        }
        if(!init) m_data.generator_type = type;

        m_scene->addItem( m_generatorArrow );

        m_gridSize = 16;

        m_generatorArrow->setOpacity(qreal(0.6));

        QPointF offset=QPoint(0,0);

        switch(direction)
        {
        case LevelNPC::NPC_GEN_LEFT:
            m_generatorArrow->setRotation(270);
            offset.setY(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_LEFT;
            break;
        case LevelNPC::NPC_GEN_DOWN:
            m_generatorArrow->setRotation(180);
            offset.setX(32);
            offset.setY(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_DOWN;
            break;
        case LevelNPC::NPC_GEN_RIGHT:
            m_generatorArrow->setRotation(90);
            offset.setX(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_RIGHT;
            break;
        case LevelNPC::NPC_GEN_UP:
        default:
            m_generatorArrow->setRotation(0);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_UP;
            break;
        }

        m_generatorArrow->setZValue(this->zValue() + 0.0000015);

        //Default Generator arrow NPC pos
        m_generatorArrow->setPos(
                    (
                        offset.x()+this->scenePos().x()+qreal((qreal(m_localProps.width) - qreal(32))/qreal(2))
                     ),
                    (
                        offset.y()+this->scenePos().y()+qreal((qreal(m_localProps.height) - qreal(32))/qreal(2))
                     ));

        m_grp->addToGroup( m_generatorArrow );

        if(!init) arrayApply();
    }
}

void ItemNPC::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            m_data.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

///////////////////MainArray functions/////////////////////////////
void ItemNPC::arrayApply()
{
    if(m_DisableScene)
        return;

    bool found=false;

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.index < (unsigned int)m_scene->LvlData->npc.size())
    {   //Check index
        if(m_data.array_id == m_scene->LvlData->npc[m_data.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    {   //directlry
        m_scene->LvlData->npc[m_data.index] = m_data; //apply current npcData
    }
    else
    for(int i=0; i<m_scene->LvlData->npc.size(); i++)
    { //after find it into array
        if(m_scene->LvlData->npc[i].array_id == m_data.array_id)
        {
            m_data.index = i;
            m_scene->LvlData->npc[i] = m_data;
            break;
        }
    }

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


void ItemNPC::removeFromArray()
{
    if(m_DisableScene)
        return;

    bool found=false;
    if(m_data.index < (unsigned int)m_scene->LvlData->npc.size())
    { //Check index
        if(m_data.array_id == m_scene->LvlData->npc[m_data.index].array_id)
            found=true;
    }
    if(found)
    { //directlry
        m_scene->LvlData->npc.removeAt(m_data.index);
    }
    else
    for(int i=0; i<m_scene->LvlData->npc.size(); i++)
    {
        if(m_scene->LvlData->npc[i].array_id == m_data.array_id)
        {
            m_scene->LvlData->npc.removeAt(i); break;
        }
    }
}


void ItemNPC::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemNPC::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}


void ItemNPC::setMainPixmap(const QPixmap &pixmap)
{
    if(_internal_animator) {
        _internal_animator->buildAnimator((QPixmap&)pixmap, m_localProps);
    }
    m_imageSize = pixmap.rect();
    refreshOffsets();
}

void ItemNPC::setNpcData(LevelNPC inD, obj_npc *mergedSet, long *animator_id)
{
    m_data = inD;

    if(m_DisableScene)
    {
        if(mergedSet)
        {
            m_localProps = (*mergedSet);
        }
        if(m_localProps.cur_image)
        {
            if(!_internal_animator)
                _internal_animator = new AdvNpcAnimator(*m_localProps.cur_image, m_localProps);
            else
                _internal_animator->buildAnimator(*m_localProps.cur_image, m_localProps);
            _internal_animator->start();
            QRect frameRect = _internal_animator->frameRect(-1);
            m_imageSize = QRectF(0, 0, frameRect.width(), frameRect.height() );
            m_animated = true;
            m_extAnimator=false;
        }
        changeDirection(m_data.direct);
        return;
    }

    if(!m_scene) return;

    if(mergedSet)
    {
        m_localProps = (*mergedSet);
        if(m_localProps.foreground)
            setZValue(m_scene->Z_npcFore);
        else
        if(m_localProps.background)
            setZValue(m_scene->Z_npcBack);
        else
            setZValue(m_scene->Z_npcStd);

        if((m_localProps.container)&&(m_data.contents>0))
            setIncludedNPC(m_data.contents, true);

        m_data.is_star = m_localProps.is_star;

        m_gridOffsetX=m_localProps.grid_offset_x;
        m_gridOffsetY=m_localProps.grid_offset_y;
        m_gridSize =  m_localProps.grid;
    }

    if(animator_id)
        setAnimator(*animator_id);

    setPos( m_data.x, m_data.y );

    changeDirection(m_data.direct);

    setGenerator(m_data.generator,
                 m_data.generator_direct,
                 m_data.generator_type, true);

    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.array_id) );

    setData(ITEM_NPC_BLOCK_COLLISION,  QString::number((int)m_localProps.collision_with_blocks) );
    setData(ITEM_NPC_NO_NPC_COLLISION, QString::number((int)m_localProps.no_npc_collions) );

    setData(ITEM_WIDTH,  QString::number(m_localProps.width) ); //width
    setData(ITEM_HEIGHT, QString::number(m_localProps.height) ); //height

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemNPC::boundingRect() const
{
    if(!m_animated)
        return QRectF(0+m_imgOffsetX+(-((double)m_localProps.gfx_offset_x)*m_direction), 0+m_imgOffsetY, m_imageSize.width(), m_imageSize.height());
    else
        return QRectF(0+m_imgOffsetX+(-((double)m_localProps.gfx_offset_x)*m_direction), 0+m_imgOffsetY, m_localProps.gfx_w, m_localProps.gfx_h);
}


void ItemNPC::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(!m_extAnimator)
    {
        if(_internal_animator)
            painter->drawPixmap(m_offseted,
                                _internal_animator->wholeImage(),
                                _internal_animator->frameRect(m_direction));
        else
            painter->drawRect(QRect(0,0,32,32));
    } else {
        if(m_animatorID<0)
        {
            painter->drawRect(QRect(m_imgOffsetX,m_imgOffsetY,1,1));
            return;
        }

        if(m_scene->animates_NPC.size()>m_animatorID)
            painter->drawPixmap(m_offseted,
                                m_scene->animates_NPC[m_animatorID]->wholeImage(),
                                m_scene->animates_NPC[m_animatorID]->frameRect(m_direction));
        else
            painter->drawRect(QRect(0,0,32,32));

        if(this->isSelected())
        {
            painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
            painter->drawRect(m_offseted.x()+1,m_imgOffsetY+1,m_imageSize.width()-2,m_imageSize.height()-2);
            painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
            painter->drawRect(m_offseted.x()+1/*imgOffsetX+1+(offset().x()/2)*/,m_imgOffsetY+1,m_imageSize.width()-2,m_imageSize.height()-2);
        }
    }
}

void ItemNPC::setScenePoint(LvlScene *theScene)
{
    LvlBaseItem::setScenePoint(theScene);
    m_grp = new QGraphicsItemGroup(this);
}


void ItemNPC::setAnimator(long aniID)
{
    if(m_DisableScene) return;

    if(aniID<m_scene->animates_NPC.size())
    {
        QRect frameRect = m_scene->animates_NPC[aniID]->frameRect(-1);
        m_imageSize = QRectF(0,0, frameRect.width(), frameRect.height() );
    }

    this->setData(ITEM_WIDTH, QString::number(qRound(m_imageSize.width())) ); //width
    this->setData(ITEM_HEIGHT, QString::number(qRound(m_imageSize.height())) ); //height

    m_animatorID = aniID;
    m_extAnimator = true;
    m_animated = true;

    //setPixmap(QPixmap(imageSize.width(), imageSize.height()));

    refreshOffsets();
}

bool ItemNPC::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->lock_npc;
}

void ItemNPC::refreshOffsets()
{
    m_imgOffsetX = (int)round( - ( ( (double)m_localProps.gfx_w - (double)m_localProps.width ) / 2 ) );
    m_imgOffsetY = (int)round( - (double)m_localProps.gfx_h + (double)m_localProps.height + (double)m_localProps.gfx_offset_y);

    m_offset_x=m_imgOffsetX+(-((double)m_localProps.gfx_offset_x)*m_direction);
    m_offset_y=m_imgOffsetY;

    m_offseted = m_imageSize;
    m_offseted.setLeft(m_offseted.left()+m_offset_x);
    m_offseted.setTop(m_offseted.top()+m_offset_y);
    m_offseted.setRight(m_offseted.right()+m_offset_x);
    m_offseted.setBottom(m_offseted.bottom()+m_offset_y);
}
