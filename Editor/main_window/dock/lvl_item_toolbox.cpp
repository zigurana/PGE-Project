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

#include <common_features/util.h>
#include <common_features/items.h>
#include <common_features/graphics_funcs.h>
#include <data_configs/custom_data.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <PGE_File_Formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_item_toolbox.h"
#include "ui_lvl_item_toolbox.h"

LevelItemBox::LevelItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LevelItemBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    allLabel = "[all]";
    customLabel = "[custom]";

    lock_grp=false;
    lock_cat=false;

    grp_blocks = "";
    grp_bgo = "";
    grp_npc = "";

    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionLVLToolBox, SLOT(setChecked(bool)));

    mw()->docks_level.
          addState(this, &GlobalSettings::LevelItemBoxVis);
}

LevelItemBox::~LevelItemBox()
{
    delete ui;
}

QTabWidget *LevelItemBox::tabWidget()
{
    return ui->LevelToolBoxTabs;
}

void LevelItemBox::re_translate()
{
    ui->retranslateUi(this);
    setLvlItemBoxes();
}

void MainWindow::on_actionLVLToolBox_triggered(bool checked)
{
    dock_LvlItemBox->setVisible(checked);
    if(checked) dock_LvlItemBox->raise();
}

void LevelItemBox::setLvlItemBoxes(bool setGrp, bool setCat)
{
//    if( (mw()->activeChildWindow() != 1) )
//        return;
    if( (setGrp) && ( mw()->activeChildWindow() != 1 ) )
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if((edit==NULL) || (!edit->sceneCreated))
        return;

    LvlScene* scene = edit->scene;
    if(!scene)
        return;

    allLabel    = MainWindow::tr("[all]");
    customLabel = MainWindow::tr("[custom]");

    mw()->ui->menuNew->setEnabled(false);
    mw()->ui->actionNew->setEnabled(false);

    if(!setCat)
    {
        lock_cat=true;
        cat_blocks = allLabel;
        cat_bgos = allLabel;
        cat_npcs = allLabel;
        if(!setGrp)
        {
            lock_grp=true;
            grp_blocks = allLabel;
            grp_bgo = allLabel;
            grp_npc = allLabel;
        }
    }

    LogDebug("LevelTools -> Clear current");

    util::memclear(ui->BGOItemsList);
    util::memclear(ui->BlockItemsList);
    util::memclear(ui->NPCItemsList);

    LogDebug("LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    bool needToAdd = false;

    tmpList.clear();
    tmpGrpList.clear();

    LogDebug("LevelTools -> List ob blocks");
    //set custom Block items from loaded level
    if( (ui->BlockCatList->currentText()==customLabel) && (setCat) && (setGrp) )
    {
        for(int i=0; i<edit->scene->custom_Blocks.size(); i++)
        {
            obj_block &block = *scene->custom_Blocks[i];
            Items::getItemGFX(&block, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( block.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(block.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BlockItemsList->addItem( item );
        }
    }
    else
    //set Block item box from global configs
    for(int i=1; i < scene->uBlocks.size(); i++)
    {
        obj_block &blockItem =  scene->uBlocks[i];

        //Add Group
        needToAdd = true;
        if( blockItem.group.isEmpty() )
        {
            needToAdd=false;
        } //Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach( QString grp, tmpGrpList )
            {
                if(blockItem.group == grp)
                {
                    needToAdd=false; break;
                }
            }
        }
        if(needToAdd)
        {
            tmpGrpList.push_back(blockItem.group);
        }

        //Add category
        needToAdd = true;
        if( (blockItem.group != grp_blocks) && (grp_blocks != allLabel) )
        {
            needToAdd=false;
        }
        else if( !tmpList.isEmpty() )
        {
            foreach( QString cat, tmpList )
            {
                if( blockItem.category == cat )
                {
                    needToAdd=false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpList.push_back(blockItem.category);
        }

        if(
            ( (blockItem.group == grp_blocks) || ( grp_blocks == allLabel) || (grp_blocks == "") )&&
            ( (blockItem.category == cat_blocks) || ( cat_blocks == allLabel) )
          )
        {
            Items::getItemGFX(&blockItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BlockItemsList->addItem( item );
        }

    }

    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->BlockGroupList->clear();
        ui->BlockGroupList->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->BlockCatList->clear();
        ui->BlockCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();

    LogDebug("LevelTools -> List ob BGOs");
    //set custom BGO items from loaded level
    if((ui->BGOCatList->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        for(int i=0; i < scene->custom_BGOs.size(); i++)
        {
            obj_bgo &bgo = *scene->custom_BGOs[i];
            Items::getItemGFX(&bgo, tmpI, false, QSize(48,48));

            item = new QListWidgetItem( bgo.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgo.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->BGOItemsList->addItem( item );
        }
    }
    else
    //set BGO item box from global array
    for(int i=1; i<scene->uBGOs.size(); i++)
    {
        obj_bgo &bgoItem = scene->uBGOs[i];

        //Add Group
        needToAdd = true;
        if(bgoItem.group.isEmpty())
        {
            needToAdd=false;
        }//Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString grp, tmpGrpList)
            {
                if(bgoItem.group==grp)
                {
                    needToAdd=false;
                    break;
                }
            }
        }
        if(needToAdd)
        {
            tmpGrpList.push_back(bgoItem.group);
        }

        //Add category
        needToAdd = true;
        if( (bgoItem.group != grp_bgo) && (grp_bgo != allLabel) )
        {
            needToAdd = false;
        }
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString cat, tmpList)
            {
                if(bgoItem.category==cat)
                {
                    needToAdd = false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpList.push_back(bgoItem.category);
        }

        if(
            ( (bgoItem.group==grp_bgo) || (grp_bgo==allLabel) || (grp_bgo=="") )&&
            ( (bgoItem.category==cat_bgos) || (cat_bgos==allLabel) )
          )
        {
            Items::getItemGFX(&bgoItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BGOItemsList->addItem( item );
        }
    }

    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->BGOGroupList->clear();
        ui->BGOGroupList->addItems(tmpGrpList);
    }
    //apply category list
    if(!setCat)
    {
        ui->BGOCatList->clear();
        ui->BGOCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();


    LogDebug("LevelTools -> List ob NPCs");
    //set custom NPC items from loaded level
    if( (ui->NPCCatList->currentText()==customLabel) && (setCat) && (setGrp) )
    {
        for(int i=0; i< scene->custom_NPCs.size(); i++)
        {
            obj_npc &npc = *edit->scene->custom_NPCs[i];

            Items::getItemGFX(&npc, tmpI, false, QSize(48,48));

            item = new QListWidgetItem( npc.name.isEmpty() ? QString("npc-%1").arg(npc.id) : npc.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npc.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->NPCItemsList->addItem( item );
        }
    }
    else
    //set NPC item box from global config
    for(int i=1; i < scene->uNPCs.size(); i++)
    {
        obj_npc &npcItem = scene->uNPCs[i];

        //Add Group
        needToAdd = true;
        if( npcItem.group.isEmpty() )
        {
            needToAdd=false;
        }//Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString grp, tmpGrpList)
            {
                if( npcItem.group==grp )
                {
                    needToAdd=false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpGrpList.push_back(npcItem.group);
        }

        //Add category
        needToAdd = true;
        if( (npcItem.group != grp_npc) && (grp_npc != allLabel) )
        {
            needToAdd = false;
        }
        else
        if( !tmpList.isEmpty() )
        {
            foreach(QString cat, tmpList)
            {
                if(npcItem.category==cat)
                {
                    needToAdd = false; break;
                }
            }
        }
        if(needToAdd)
        {
            tmpList.push_back(npcItem.category);
        }

        if(
            ( (npcItem.group==grp_npc) || (grp_npc==allLabel) || (grp_npc=="") )&&
            ( (npcItem.category==cat_npcs) || (cat_npcs==allLabel) )
          )
        {
            Items::getItemGFX(&npcItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->NPCItemsList->addItem( item );
        }
    }
    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->NPCGroupList->clear();
        ui->NPCGroupList->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->NPCCatList->clear();
        ui->NPCCatList->addItems(tmpList);
    }

    lock_grp=false;
    lock_cat=false;

    updateFilters();

    mw()->ui->menuNew->setEnabled(true);
    mw()->ui->actionNew->setEnabled(true);
}

// ///////////////////////////////////
void LevelItemBox::on_BlockGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_blocks=arg1;
    setLvlItemBoxes(true);
}

void LevelItemBox::on_BGOGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_bgo=arg1;
    setLvlItemBoxes(true);
}

void LevelItemBox::on_NPCGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_npc=arg1;
    setLvlItemBoxes(true);
}

// ///////////////////////////////////
void LevelItemBox::on_BlockCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_blocks=arg1;
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_BGOCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_bgos=arg1;
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_NPCCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_npcs=arg1;
    setLvlItemBoxes(true, true);
}




// ///////////////////////////////////

void LevelItemBox::on_BGOUniform_clicked(bool checked)
{
    ui->BGOItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}

void LevelItemBox::on_BlockUniform_clicked(bool checked)
{
    ui->BlockItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_NPCUniform_clicked(bool checked)
{
    ui->NPCItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}


// ///////////////////////////////////

void LevelItemBox::on_BlockItemsList_itemClicked(QListWidgetItem *item)
{
    //placeBlock
    if ((mw()->activeChildWindow()==1) && (ui->BlockItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_Block, item->data(3).toInt());
    }
}

void LevelItemBox::on_BGOItemsList_itemClicked(QListWidgetItem *item)
{
    //placeBGO
    if ((mw()->activeChildWindow()==1) && (ui->BGOItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_BGO, item->data(3).toInt());
    }
}

void LevelItemBox::on_NPCItemsList_itemClicked(QListWidgetItem *item)
{
    //placeNPC
    if ((mw()->activeChildWindow()==1) && (ui->NPCItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_NPC, item->data(3).toInt());
    }
}

void LevelItemBox::updateFilters()
{
    int current = ui->LevelToolBoxTabs->currentIndex();
    if(current == 0){
        util::updateFilter(ui->BlockFilterField, ui->BlockItemsList, ui->BlockFilterType);
    }else if(current == 1){
        util::updateFilter(ui->BGOFilterField, ui->BGOItemsList, ui->BGOFilterType);
    }else if(current == 2){
        util::updateFilter(ui->NPCFilterField, ui->NPCItemsList, ui->NPCFilterType);
    }
}

void LevelItemBox::clearFilter()
{
    ui->BlockFilterField->setText("");
    ui->BGOFilterField->setText("");
    ui->NPCFilterField->setText("");
    updateFilters();
}

void LevelItemBox::on_BlockFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_BlockFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void LevelItemBox::on_BGOFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_BGOFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void LevelItemBox::on_NPCFilterField_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_NPCFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

