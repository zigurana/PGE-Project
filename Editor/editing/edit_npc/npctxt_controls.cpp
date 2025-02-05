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

#include <common_features/graphics_funcs.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/npctxt_manager.h>
#include <common_features/themes.h>

#include "npcedit.h"
#include <ui_npcedit.h>

void NpcEdit::on_ResetNPCData_clicked()
{
    NpcData = StartNPCData; //Restore first version
    setDataBoxes();
    if(!isUntitled) documentNotModified();
}



///////////SMBX64 Options switches//////////////////////////////
void NpcEdit::on_en_GFXOffsetX_clicked(bool checked)
{
    ui->GFXOffSetX->setEnabled(checked);
    ui->offsetx_label->setEnabled(checked);
    NpcData.en_gfxoffsetx=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_en_GFXOffsetY_clicked(bool checked)
{
    ui->GFXOffSetY->setEnabled(checked);
    ui->offsety_label->setEnabled(checked);
    NpcData.en_gfxoffsety=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_GFXw_clicked(bool checked)
{

    ui->GFXw->setEnabled(checked);
    ui->gwidth_label->setEnabled(checked);
    NpcData.en_gfxwidth=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_GFXh_clicked(bool checked)
{
    ui->GFXh->setEnabled(checked);
    ui->gheight_label->setEnabled(checked);
    NpcData.en_gfxheight=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Frames_clicked(bool checked)
{
    ui->Frames->setEnabled(checked);
    ui->frames_label->setEnabled(checked);
    NpcData.en_frames=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Framespeed_clicked(bool checked)
{
    ui->Framespeed->setEnabled(checked);
    ui->framespeed_label->setEnabled(checked);
    NpcData.en_framespeed=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Framestyle_clicked(bool checked)
{
    ui->FrameStyle->setEnabled(checked);
    ui->framestyle_label->setEnabled(checked);
    NpcData.en_framestyle=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_IsForeground_clicked(bool checked)
{
    ui->IsForeground->setEnabled(checked);
    NpcData.en_foreground=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_GrabSide_clicked(bool checked)
{
    ui->GrabSide->setEnabled(checked);
    NpcData.en_grabside=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_GrabTop_clicked(bool checked)
{
    ui->GrabTop->setEnabled(checked);
    NpcData.en_grabtop=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_JumpHurt_clicked(bool checked)
{
    ui->JumpHurt->setEnabled(checked);
    NpcData.en_jumphurt=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_DontHurt_clicked(bool checked)
{
    ui->DontHurt->setEnabled(checked);
    NpcData.en_nohurt=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Score_clicked(bool checked)
{
    ui->Score->setEnabled(checked);
    ui->score_label->setEnabled(checked);
    NpcData.en_score=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NoEat_clicked(bool checked)
{
    ui->NoEat->setEnabled(checked);
    NpcData.en_noyoshi=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NoFireball_clicked(bool checked)
{
    ui->NoFireball->setEnabled(checked);
    NpcData.en_nofireball=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NoIceball_clicked(bool checked)
{
    ui->NoIceball->setEnabled(checked);
    NpcData.en_noiceball=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Width_clicked(bool checked)
{
    ui->Width->setEnabled(checked);
    ui->width_label->setEnabled(checked);
    NpcData.en_width=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Height_clicked(bool checked)
{
    ui->Height->setEnabled(checked);
    ui->height_label->setEnabled(checked);
    NpcData.en_height=checked;

    updatePreview();
    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_Speed_clicked(bool checked)
{
    ui->Speed->setEnabled(checked);
    ui->speed_label->setEnabled(checked);
    NpcData.en_speed=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_PlayerBlock_clicked(bool checked)
{
    ui->PlayerBlock->setEnabled(checked);
    NpcData.en_playerblock=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_PlayerBlockTop_clicked(bool checked)
{
    ui->PlayerBlockTop->setEnabled(checked);
    NpcData.en_playerblocktop=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NPCBlock_clicked(bool checked)
{
    ui->NPCBlock->setEnabled(checked);
    NpcData.en_npcblock=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NPCBlockTop_clicked(bool checked)
{
    ui->NPCBlockTop->setEnabled(checked);
    NpcData.en_npcblocktop=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NoBlockCollision_clicked(bool checked)
{
    ui->NoBlockCollision->setEnabled(checked);
    NpcData.en_noblockcollision=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_NoGravity_clicked(bool checked)
{
    ui->NoGravity->setEnabled(checked);
    NpcData.en_nogravity=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_En_TurnCliff_clicked(bool checked)
{
    ui->TurnCliff->setEnabled(checked);
    NpcData.en_cliffturn=checked;

    documentWasModified();
}





/////////////////SMBX64 Params//////////////////////////////////
void NpcEdit::on_GFXOffSetX_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxoffsetx = arg1;
    updatePreview();
}


////////////////////////////////////////////////////////////////
void NpcEdit::on_GFXOffSetY_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxoffsety = arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_GFXw_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxwidth = arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_GFXh_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxheight = arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Frames_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.frames = arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Framespeed_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.framespeed=arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_FrameStyle_currentIndexChanged(int index)
{
    documentWasModified();
    NpcData.framestyle=index;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_IsForeground_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.foreground=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_GrabSide_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.grabside=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_GrabTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.grabtop=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_JumpHurt_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.jumphurt=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_DontHurt_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nohurt=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Score_currentIndexChanged(int index)
{
    documentWasModified();
    NpcData.score=index;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoEat_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noyoshi=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoFireball_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nofireball=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoIceball_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noiceball=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Width_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.width=arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Height_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.height=arg1;
    updatePreview();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_Speed_valueChanged(double arg1)
{
    documentWasModified();
    NpcData.speed = arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_PlayerBlock_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.playerblock=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_PlayerBlockTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.playerblocktop=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NPCBlock_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.npcblock=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NPCBlockTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.npcblocktop=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoBlockCollision_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noblockcollision=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoGravity_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nogravity=arg1;
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_TurnCliff_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.cliffturn=arg1;
}




//////////Exchanged options/////////////////////////////////////
void NpcEdit::on_En_NoHammer_clicked(bool checked)
{
    ui->NoHammer->setEnabled(checked);
    NpcData.en_nohammer=checked;

    documentWasModified();
}

////////////////////////////////////////////////////////////////
void NpcEdit::on_NoHammer_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nohammer=arg1;
}
////////////////////////////////////////////////////////////////



void NpcEdit::on_En_NoShell_clicked(bool checked)
{
    ui->NoShell->setEnabled(checked);
    NpcData.en_noshell=checked;

    documentWasModified();
}

void NpcEdit::on_NoShell_stateChanged(int checked)
{
    documentWasModified();
    NpcData.noshell=checked;
}
////////////////////////////////////////////////////////////////



void NpcEdit::on_En_Name_clicked(bool checked)
{
    ui->Name->setEnabled(checked);
    NpcData.en_name=checked;

    documentWasModified();
}
void NpcEdit::on_Name_textEdited(const QString &arg1)
{
    documentWasModified();
    NpcData.name=arg1;
}
////////////////////////////////////////////////////////////////

void NpcEdit::on_En_Health_clicked(bool checked)
{
    ui->Health->setEnabled(checked);
    ui->health_label->setEnabled(checked);
    NpcData.en_health=checked;
    documentNotModified();
}
void NpcEdit::on_Health_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.health=arg1;
}

////////////////////////////////////////////////////////////////

void NpcEdit::on_En_GridSize_clicked(bool checked)
{
    ui->GridSize->setEnabled(checked);
    ui->grid_label->setEnabled(checked);
    NpcData.en_grid=checked;
    documentNotModified();
}
void NpcEdit::on_GridSize_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.grid=arg1;
}
////////////////////////////////////////////////////////////////

void NpcEdit::on_En_GridOffsetX_clicked(bool checked)
{
    ui->GridOffsetX->setEnabled(checked);
    ui->gridoffsetx_label->setEnabled(checked);
    NpcData.en_grid_offset_x=checked;
    documentNotModified();
}
void NpcEdit::on_GridOffsetX_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.grid_offset_x=arg1;
}

////////////////////////////////////////////////////////////////

void NpcEdit::on_En_GridOffsetY_clicked(bool checked)
{
    ui->GridOffsetY->setEnabled(checked);
    ui->gridoffsety_label->setEnabled(checked);
    NpcData.en_grid_offset_y=checked;
    documentNotModified();
}
void NpcEdit::on_GridOffsetY_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.grid_offset_y=arg1;
}

////////////////////////////////////////////////////////////////

void NpcEdit::on_En_AlignAt_clicked(bool checked)
{
    ui->AlignAt->setEnabled(checked);
    ui->alignat_label->setEnabled(checked);
    NpcData.en_grid_align=checked;
    documentNotModified();
}
void NpcEdit::on_AlignAt_currentIndexChanged(int index)
{
    documentWasModified();
    NpcData.grid_align=index;
}

////////////////////////////////////////////////////////////////

void NpcEdit::on_DirectLeft_clicked()
{
    ui->DirectLeft->setChecked(true);
    ui->DirectRight->setChecked(false);
    direction = -1;
    updatePreview();
}

void NpcEdit::on_DirectRight_clicked()
{
    ui->DirectRight->setChecked(true);
    ui->DirectLeft->setChecked(false);
    direction = 1;
    updatePreview();
}
////////////////////////////////////////////////////////////////

void NpcEdit::loadPreview()
{
    if(PreviewScene==NULL) PreviewScene = new QGraphicsScene();
    if(physics==NULL) physics = new QGraphicsRectItem();
    PreviewScene->setSceneRect(0,0, ui->PreviewBox->width()-20, ui->PreviewBox->height()-20);
    if(npcPreview==NULL)
    {
        npcPreview = new ItemNPC(true);
        npcPreview->setScenePoint();
    }

    if((npc_id<=0)||(!pConfigs->main_npc.contains(npc_id)))
    {
        npc_id=1;
        LogWarning(QString("NPC-Edit Preview -> NPC Entry not found"));
        npcImage = Themes::Image(Themes::dummy_npc);
        pConfigs->main_npc[1].copyTo(defaultNPC);
        defaultNPC.frames=1;
        defaultNPC.cur_image=&npcImage;
    }
    else
    {
        pConfigs->main_npc[npc_id].copyTo(defaultNPC);
        LogDebug(QString("NPC-Edit Preview -> Detected NPC-%1 named as \"%2\"").arg(defaultNPC.id).arg(defaultNPC.name));
        if(isUntitled)
            npcImage = *defaultNPC.cur_image;
        else
            loadImageFile();
    }

    obj_npc targetNPC;
    targetNPC = mergeNPCConfigs(defaultNPC, NpcData, npcImage.size());

    LevelNPC npcData = FileFormats::CreateLvlNpc();
    npcData.id = npc_id;
    npcData.x = 10;
    npcData.y = 10;
    npcData.direct = direction;
    npcPreview->setNpcData(npcData, &targetNPC);
    npcPreview->setFlag(QGraphicsItem::ItemIsSelectable, false);
    npcPreview->setFlag(QGraphicsItem::ItemIsMovable, false);
    npcPreview->setZValue(1);
    PreviewScene->addItem(npcPreview);

    if(npcPreview->m_localProps.frames>1)
    {
        npcPreview->setData(4, "animated");
    }

    physics->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    physics->setBrush(Qt::transparent);
    physics->setRect(0,0, npcPreview->m_localProps.width, npcPreview->m_localProps.height);

    physics->setZValue(777);
    ui->PreviewBox->setScene(PreviewScene);
    ui->PreviewBox->setBackgroundBrush(Qt::white);

    PreviewScene->addItem(physics);

    npcPreview->setPos(
                (PreviewScene->width()/2)-(qreal(npcPreview->m_localProps.width)/qreal(2)) ,
                (PreviewScene->height()/2)-(qreal(npcPreview->m_localProps.height)/qreal(2))
                );
    physics->setPos(
                (PreviewScene->width()/2)-(qreal(npcPreview->m_localProps.width)/qreal(2)) ,
                (PreviewScene->height()/2)-(qreal(npcPreview->m_localProps.height)/qreal(2))
                );

    npcPreview->_internal_animator->connect(npcPreview->_internal_animator,
                                            SIGNAL(onFrame()),
                                            PreviewScene,
                                            SLOT(update()));
}

void NpcEdit::updatePreview()
{
    if(!physics || !npcPreview)
        return;

    obj_npc merged = mergeNPCConfigs(defaultNPC, NpcData, npcImage.size());
    LevelNPC npcData = FileFormats::CreateLvlNpc();
    npcData.id = npc_id;
    npcData.x = 10;
    npcData.y = 10;
    npcData.direct = direction;
    npcPreview->setNpcData(npcData, &merged);

    physics->setRect(0,0, npcPreview->m_localProps.width, npcPreview->m_localProps.height);
    npcPreview->setPos(
                (PreviewScene->width()/2)-(qreal(npcPreview->m_localProps.width)/qreal(2)) ,
                (PreviewScene->height()/2)-(qreal(npcPreview->m_localProps.height)/qreal(2))
                );
    physics->setPos(
                (PreviewScene->width()/2)-(qreal(npcPreview->m_localProps.width)/qreal(2)) ,
                (PreviewScene->height()/2)-(qreal(npcPreview->m_localProps.height)/qreal(2))
                );

    PreviewScene->update();
}


void NpcEdit::loadImageFile()
{
    QString imagePath = QFileInfo(curFile).dir().absolutePath()+"/";
    CustomDirManager fileDir(imagePath, "npcx");
    QString CustomImage=fileDir.getCustomFile(defaultNPC.image_n);
    QImage tempImg;

    if(!CustomImage.isEmpty())
    {
        if(!CustomImage.endsWith(".png", Qt::CaseInsensitive))
        {
            QString CustomMask=fileDir.getCustomFile(defaultNPC.mask_n);
            GraphicsHelps::loadQImage(tempImg, CustomImage, CustomMask);
        } else {
            GraphicsHelps::loadQImage(tempImg, CustomImage);
        }
        LogDebug(QString("Image size %1 %2").arg(npcImage.width()).arg(npcImage.height()));
        LogDebug(QString("Loaded custom NPC image: %1").arg(CustomImage));
        if(tempImg.isNull())
        {
            LogDebug(QString("Loading custom NPC Image was failed, using default image"));
            if(defaultNPC.cur_image)
                npcImage = *defaultNPC.cur_image;
        } else {
            npcImage=std::move(QPixmap::fromImage(tempImg));
            defaultNPC.cur_image = &npcImage;
        }
    }
    else
    {
        if(defaultNPC.cur_image)
            npcImage = *defaultNPC.cur_image;
        LogDebug(QString("System image size %1 %2").arg(npcImage.width()).arg(npcImage.height()));
    }
}

void NpcEdit::refreshImageFile()
{
    loadImageFile();
    //npcPreview->AnimationStop();
    npcPreview->setMainPixmap(npcImage);
}
