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

#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/npc_filedata.h>

#include "npcedit.h"
#include <ui_npcedit.h>

void NpcEdit::setDataBoxes()
{

    //Apply NPC data
    ui->en_GFXOffsetX->setChecked(StartNPCData.en_gfxoffsetx);
    ui->offsetx_label->setEnabled(StartNPCData.en_gfxoffsetx);
    ui->GFXOffSetX->setEnabled(StartNPCData.en_gfxoffsetx);
    if(!StartNPCData.en_gfxoffsetx)
    {
        ui->GFXOffSetX->setValue(DefaultNPCData.gfxoffsetx);
        NpcData.gfxoffsetx=DefaultNPCData.gfxoffsetx;
    }
    else
        ui->GFXOffSetX->setValue(StartNPCData.gfxoffsetx);


    ui->en_GFXOffsetY->setChecked(StartNPCData.en_gfxoffsety);
    ui->offsety_label->setEnabled(StartNPCData.en_gfxoffsety);
    ui->GFXOffSetY->setEnabled(StartNPCData.en_gfxoffsety);
    if(!StartNPCData.en_gfxoffsety)
    {
        ui->GFXOffSetY->setValue(DefaultNPCData.gfxoffsety);
        NpcData.gfxoffsety=DefaultNPCData.gfxoffsety;
    }
    else
        ui->GFXOffSetY->setValue(StartNPCData.gfxoffsety);


    ui->En_IsForeground->setChecked(StartNPCData.en_foreground);
    ui->IsForeground->setEnabled(StartNPCData.en_foreground);
    if(!StartNPCData.en_foreground)
    {
        ui->IsForeground->setChecked(DefaultNPCData.foreground);
        NpcData.foreground=DefaultNPCData.foreground;
    }
    else
        ui->IsForeground->setChecked(StartNPCData.foreground);


    ui->En_GFXh->setChecked(StartNPCData.en_gfxheight);
    ui->gheight_label->setEnabled(StartNPCData.en_gfxheight);
    ui->GFXh->setEnabled(StartNPCData.en_gfxheight);
    if(!StartNPCData.en_gfxheight)
    {        
        ui->GFXh->setValue(DefaultNPCData.gfxheight);
        NpcData.gfxheight=DefaultNPCData.gfxheight;
    }
    else
        ui->GFXh->setValue(StartNPCData.gfxheight);


    ui->En_GFXw->setChecked(StartNPCData.en_gfxwidth);
    ui->gwidth_label->setEnabled(StartNPCData.en_gfxwidth);
    ui->GFXw->setEnabled(StartNPCData.en_gfxwidth);
    if(!StartNPCData.en_gfxwidth)
    {
        ui->GFXw->setValue(DefaultNPCData.gfxwidth);
        NpcData.gfxwidth=DefaultNPCData.gfxwidth;
    }
    else
        ui->GFXw->setValue(StartNPCData.gfxwidth);


    ui->En_Framestyle->setChecked(StartNPCData.en_framestyle);
    ui->framestyle_label->setEnabled(StartNPCData.en_framestyle);
    ui->FrameStyle->setEnabled(StartNPCData.en_framestyle);
    if(!StartNPCData.en_framestyle)
    {        
        ui->FrameStyle->setCurrentIndex(DefaultNPCData.framestyle);
        NpcData.framestyle=DefaultNPCData.framestyle;
    }
    else
        ui->FrameStyle->setCurrentIndex(StartNPCData.framestyle);


    ui->En_Framespeed->setChecked(StartNPCData.en_framespeed);
    ui->framespeed_label->setEnabled(StartNPCData.en_framespeed);
    ui->Framespeed->setEnabled(StartNPCData.en_framespeed);
    if(!StartNPCData.en_framespeed)
    {        
        ui->Framespeed->setValue(DefaultNPCData.framespeed);
        NpcData.framespeed=DefaultNPCData.framespeed;
    }
    else
        ui->Framespeed->setValue(StartNPCData.framespeed);


    ui->En_Frames->setChecked(StartNPCData.en_frames);
    ui->frames_label->setEnabled(StartNPCData.en_frames);
    ui->Frames->setEnabled(StartNPCData.en_frames);
    if(!StartNPCData.en_frames)
    {
        ui->Frames->setValue(DefaultNPCData.frames);
        NpcData.frames=DefaultNPCData.frames;
    }
    else
        ui->Frames->setValue(StartNPCData.frames);


    ui->En_Score->setChecked(StartNPCData.en_score);
    ui->score_label->setEnabled(StartNPCData.en_score);
    ui->Score->setEnabled(StartNPCData.en_score);
    if(!StartNPCData.en_score)
    {        
        ui->Score->setCurrentIndex(DefaultNPCData.score);
        NpcData.score=DefaultNPCData.score;
    }
    else
        ui->Score->setCurrentIndex(StartNPCData.score);


    ui->En_Health->setChecked(StartNPCData.en_health);
    ui->health_label->setEnabled(StartNPCData.en_health);
    ui->Health->setEnabled(StartNPCData.en_health);
    if(!StartNPCData.en_health)
    {
        ui->Health->setValue(DefaultNPCData.health);
        NpcData.health=DefaultNPCData.health;
    }
    else
        ui->Health->setValue(StartNPCData.health);


    ui->En_JumpHurt->setChecked(StartNPCData.en_jumphurt);
    ui->JumpHurt->setEnabled(StartNPCData.en_jumphurt);
    if(!StartNPCData.en_jumphurt)
    {        
        ui->JumpHurt->setChecked(DefaultNPCData.jumphurt);
        NpcData.jumphurt=DefaultNPCData.jumphurt;
    }
    else
        ui->JumpHurt->setChecked(StartNPCData.jumphurt);


    ui->En_GrabTop->setChecked(StartNPCData.en_grabtop);
    ui->GrabTop->setEnabled(StartNPCData.en_grabtop);
    if(!StartNPCData.en_grabtop)
    {        
        ui->GrabTop->setChecked(DefaultNPCData.grabtop);
        NpcData.grabtop=DefaultNPCData.grabtop;
    }
    else
        ui->GrabTop->setChecked(StartNPCData.grabtop);


    ui->En_GrabSide->setChecked(StartNPCData.en_grabside);
    ui->GrabSide->setEnabled(StartNPCData.en_grabside);
    if(!StartNPCData.en_grabside)
    {        
        ui->GrabSide->setChecked(DefaultNPCData.grabside);
        NpcData.grabside=DefaultNPCData.grabside;
    }
    else
        ui->GrabSide->setChecked(StartNPCData.grabside);


    ui->En_NoFireball->setChecked(StartNPCData.en_nofireball);
    ui->NoFireball->setEnabled(StartNPCData.en_nofireball);
    if(!StartNPCData.en_nofireball)
    {
        ui->NoFireball->setChecked(DefaultNPCData.nofireball);
        NpcData.nofireball=DefaultNPCData.nofireball;
    }
    else
        ui->NoFireball->setChecked(StartNPCData.nofireball);


    ui->En_DontHurt->setChecked(StartNPCData.en_nohurt);
    ui->DontHurt->setEnabled(StartNPCData.en_nohurt);
    if(!StartNPCData.en_nohurt)
    {        
        ui->DontHurt->setChecked(DefaultNPCData.nohurt);
        NpcData.nohurt=DefaultNPCData.nohurt;
    }
    else
        ui->DontHurt->setChecked(StartNPCData.nohurt);


    ui->En_NoEat->setChecked(StartNPCData.en_noyoshi);
    ui->NoEat->setEnabled(StartNPCData.en_noyoshi);
    if(!StartNPCData.en_noyoshi)
    {
        ui->NoEat->setChecked(DefaultNPCData.noyoshi);
        NpcData.noyoshi=DefaultNPCData.noyoshi;
    }
    else
        ui->NoEat->setChecked(StartNPCData.noyoshi);


    ui->En_NoIceball->setChecked(StartNPCData.en_noiceball);
    ui->NoIceball->setEnabled(StartNPCData.en_noiceball);
    if(!StartNPCData.en_noiceball)
    {
        ui->NoIceball->setChecked(DefaultNPCData.noiceball);
        NpcData.noiceball=DefaultNPCData.noiceball;
    }
    else
        ui->NoIceball->setChecked(StartNPCData.noiceball);


    ui->En_Height->setChecked(StartNPCData.en_height);
    ui->height_label->setEnabled(StartNPCData.en_height);
    ui->Height->setEnabled(StartNPCData.en_height);
    if(!StartNPCData.en_height)
    {
        ui->Height->setValue(DefaultNPCData.height);
        NpcData.height=DefaultNPCData.height;
    }
    else
        ui->Height->setValue(StartNPCData.height);


    ui->En_Width->setChecked(StartNPCData.en_width);
    ui->width_label->setEnabled(StartNPCData.en_width);
    ui->Width->setEnabled(StartNPCData.en_width);
    if(!StartNPCData.en_width)
    {
        ui->Width->setValue(DefaultNPCData.width);
        NpcData.width=DefaultNPCData.width;
    }
    else
        ui->Width->setValue(StartNPCData.width);


    ui->En_NPCBlock->setChecked(StartNPCData.en_npcblock);
    ui->NPCBlock->setEnabled(StartNPCData.en_npcblock);
    if(!StartNPCData.en_npcblock)
    {
        ui->NPCBlock->setChecked(DefaultNPCData.npcblock);
        NpcData.npcblock=DefaultNPCData.npcblock;
    }
    else
        ui->NPCBlock->setChecked(StartNPCData.npcblock);


    ui->En_NPCBlockTop->setChecked(StartNPCData.en_npcblocktop);
    ui->NPCBlockTop->setEnabled(StartNPCData.en_npcblocktop);
    if(!StartNPCData.en_npcblocktop)
    {
        ui->NPCBlockTop->setChecked(DefaultNPCData.npcblocktop);
        NpcData.npcblocktop=DefaultNPCData.npcblocktop;
    }
    else
        ui->NPCBlockTop->setChecked(StartNPCData.npcblocktop);


    ui->En_Speed->setChecked(StartNPCData.en_speed);
    ui->speed_label->setEnabled(StartNPCData.en_speed);
    ui->Speed->setEnabled(StartNPCData.en_speed);
    if(!StartNPCData.en_speed)
    {
        NpcData.speed=DefaultNPCData.speed;
        ui->Speed->setValue(DefaultNPCData.speed);
    }
    else
        ui->Speed->setValue(StartNPCData.speed);


    ui->En_PlayerBlock->setChecked(StartNPCData.en_playerblock);
    ui->PlayerBlock->setEnabled(StartNPCData.en_playerblock);
    if(!StartNPCData.en_playerblock)
    {
        ui->PlayerBlock->setChecked(DefaultNPCData.playerblock);
        NpcData.playerblock=DefaultNPCData.playerblock;
    }
    else
        ui->PlayerBlock->setChecked(StartNPCData.playerblock);


    ui->En_PlayerBlockTop->setChecked(StartNPCData.en_playerblocktop);
    ui->PlayerBlockTop->setEnabled(StartNPCData.en_playerblocktop);
    if(!StartNPCData.en_playerblocktop)
    {
        ui->PlayerBlockTop->setChecked(DefaultNPCData.playerblocktop);
        NpcData.playerblocktop=DefaultNPCData.playerblocktop;
    }
    else
        ui->PlayerBlockTop->setChecked(StartNPCData.playerblocktop);


    ui->En_NoBlockCollision->setChecked(StartNPCData.en_noblockcollision);
    ui->NoBlockCollision->setEnabled(StartNPCData.en_noblockcollision);
    if(!StartNPCData.en_noblockcollision)
    {        
        ui->NoBlockCollision->setChecked(DefaultNPCData.noblockcollision);
        NpcData.noblockcollision=DefaultNPCData.noblockcollision;
    }
    else
        ui->NoBlockCollision->setChecked(StartNPCData.noblockcollision);


    ui->En_NoGravity->setChecked(StartNPCData.en_nogravity);
    ui->NoGravity->setEnabled(StartNPCData.en_nogravity);
    if(!StartNPCData.en_nogravity)
    {
        ui->NoGravity->setChecked(DefaultNPCData.nogravity);
        NpcData.nogravity=DefaultNPCData.nogravity;
    }
    else
        ui->NoGravity->setChecked(StartNPCData.nogravity);


    ui->En_TurnCliff->setChecked(StartNPCData.en_cliffturn);
    ui->TurnCliff->setEnabled(StartNPCData.en_cliffturn);
    if(!StartNPCData.en_cliffturn)
    {
        ui->TurnCliff->setChecked(DefaultNPCData.cliffturn);
        NpcData.cliffturn=DefaultNPCData.cliffturn;
    }
    else
        ui->TurnCliff->setChecked(StartNPCData.cliffturn);

    //Extended

    ui->En_NoHammer->setChecked(StartNPCData.en_nohammer);
    ui->NoHammer->setEnabled(StartNPCData.en_nohammer);
    if(!StartNPCData.en_nohammer)
    {
        ui->NoHammer->setChecked(DefaultNPCData.nohammer);
        NpcData.nohammer=DefaultNPCData.nohammer;
    }
    else
        ui->NoHammer->setChecked(StartNPCData.nohammer);

    ui->En_NoShell->setChecked(StartNPCData.en_noshell);
    ui->NoShell->setEnabled(StartNPCData.en_noshell);
    if(!StartNPCData.en_noshell)
    {
        ui->NoShell->setChecked(DefaultNPCData.noshell);
        NpcData.noshell=DefaultNPCData.noshell;
    }
    else
        ui->NoShell->setChecked(StartNPCData.noshell);


    ui->En_Name->setChecked(StartNPCData.en_name);
    ui->Name->setEnabled(StartNPCData.en_name);
    if(!StartNPCData.en_name)
    {
        ui->Name->setText(DefaultNPCData.name);
        NpcData.name=DefaultNPCData.name;
    }
    else
        ui->Name->setText(StartNPCData.name);

    ui->En_Health->setChecked(StartNPCData.en_health);
    ui->Health->setEnabled(StartNPCData.en_health);
    if(!StartNPCData.en_health)
    {
        ui->Health->setValue(DefaultNPCData.health);
        NpcData.health=DefaultNPCData.health;
    }
    else
        ui->Health->setValue(StartNPCData.health);

    ui->En_GridSize->setChecked(StartNPCData.en_grid);
    ui->GridSize->setEnabled(StartNPCData.en_grid);
    if(!StartNPCData.en_grid)
    {
        ui->GridSize->setValue(DefaultNPCData.grid);
        NpcData.grid=DefaultNPCData.grid;
    }
    else
        ui->GridSize->setValue(StartNPCData.grid);

    int offsetMin = -1*(abs(ui->GridSize->value())/2)+1;
    int offsetMax = (abs(ui->GridSize->value())/2)-1;
    ui->GridOffsetX->setMinimum(offsetMin);
    ui->GridOffsetX->setMaximum(offsetMax);
    ui->GridOffsetY->setMinimum(offsetMin);
    ui->GridOffsetY->setMaximum(offsetMax);

    ui->En_GridOffsetX->setChecked(StartNPCData.en_grid_offset_x);
    ui->GridOffsetX->setEnabled(StartNPCData.en_grid_offset_x);
    if(!StartNPCData.en_grid_offset_x)
    {
        ui->GridOffsetX->setValue(DefaultNPCData.grid_offset_x);
        NpcData.grid_offset_x=DefaultNPCData.grid_offset_x;
    }
    else
        ui->GridOffsetX->setValue(StartNPCData.grid_offset_x);

    ui->En_GridOffsetY->setChecked(StartNPCData.en_grid_offset_y);
    ui->GridOffsetY->setEnabled(StartNPCData.en_grid_offset_y);
    if(!StartNPCData.en_grid_offset_y)
    {
        ui->GridOffsetY->setValue(DefaultNPCData.grid_offset_y);
        NpcData.grid_offset_y=DefaultNPCData.grid_offset_y;
    }
    else
        ui->GridOffsetY->setValue(StartNPCData.grid_offset_y);

    ui->En_AlignAt->setChecked(StartNPCData.en_grid_align);
    ui->AlignAt->setEnabled(StartNPCData.en_grid_align);
    if(!StartNPCData.en_grid_align)
    {
        ui->AlignAt->setCurrentIndex(DefaultNPCData.grid_align);
        NpcData.grid_align=DefaultNPCData.grid_align;
    }
    else
        ui->AlignAt->setCurrentIndex(StartNPCData.grid_align);
}

void NpcEdit::setDefaultData(unsigned long npc_id)
{

    DefaultNPCData = FileFormats::CreateEmpytNpcTXT();

    if((npc_id==0)||(npc_id > (unsigned long)pConfigs->main_npc.stored()))
    {
        DefaultNPCData.gfxoffsetx=0;
        DefaultNPCData.gfxoffsety=0;
        DefaultNPCData.width=0;
        DefaultNPCData.height=0;
        DefaultNPCData.gfxwidth=0;
        DefaultNPCData.gfxheight=0;
        DefaultNPCData.score=0;
        DefaultNPCData.playerblock=0;
        DefaultNPCData.playerblocktop=0;
        DefaultNPCData.npcblock=0;
        DefaultNPCData.npcblocktop=0;
        DefaultNPCData.grabside=0;
        DefaultNPCData.grabtop=0;
        DefaultNPCData.jumphurt=0;
        DefaultNPCData.nohurt=0;
        DefaultNPCData.noblockcollision=0;
        DefaultNPCData.cliffturn=0;
        DefaultNPCData.noyoshi=0;
        DefaultNPCData.foreground=0;
        DefaultNPCData.speed=1;
        DefaultNPCData.nofireball=0;
        DefaultNPCData.nogravity=0;
        DefaultNPCData.frames=0;
        DefaultNPCData.framespeed=8;
        DefaultNPCData.framestyle=0;
        DefaultNPCData.noiceball=0;
        DefaultNPCData.nohammer=0;
        DefaultNPCData.name="";
        DefaultNPCData.health=1;
        DefaultNPCData.script="";
        DefaultNPCData.image="";
        DefaultNPCData.grid=32;
        DefaultNPCData.grid_offset_x=0;
        DefaultNPCData.grid_offset_y=0;
        DefaultNPCData.grid_align=0;
    }
    else
    {
        obj_npc &t_npc = pConfigs->main_npc[npc_id];

        DefaultNPCData.gfxoffsetx=t_npc.gfx_offset_x;
        DefaultNPCData.gfxoffsety=t_npc.gfx_offset_y;
        DefaultNPCData.width=t_npc.width;
        DefaultNPCData.height=t_npc.height;
        DefaultNPCData.gfxwidth=t_npc.gfx_w;
        DefaultNPCData.gfxheight=t_npc.gfx_h;
        DefaultNPCData.score=t_npc.score;
        DefaultNPCData.playerblock=(int)t_npc.block_player;
        DefaultNPCData.playerblocktop=(int)t_npc.block_player_top;
        DefaultNPCData.npcblock=(int)t_npc.block_npc;
        DefaultNPCData.npcblocktop=(int)t_npc.block_player_top;
        DefaultNPCData.grabside=(int)t_npc.grab_side;
        DefaultNPCData.grabtop=(int)t_npc.grab_top;
        DefaultNPCData.jumphurt=(int)(
                    (t_npc.hurt_player)
                                      &&
                                      (!t_npc.kill_on_jump));
        DefaultNPCData.nohurt=(int)(!t_npc.hurt_player);
        DefaultNPCData.noblockcollision=(int)(!t_npc.collision_with_blocks);
        DefaultNPCData.cliffturn=(int)t_npc.turn_on_cliff_detect;
        DefaultNPCData.noyoshi=(int)(!t_npc.can_be_eaten);
        DefaultNPCData.foreground=(int)t_npc.foreground;
        DefaultNPCData.speed=1;
        DefaultNPCData.nofireball=(int)(!t_npc.kill_by_fireball);
        DefaultNPCData.nogravity=(int)(!t_npc.gravity);
        DefaultNPCData.frames=t_npc.frames;
        DefaultNPCData.framespeed=8;
        DefaultNPCData.framestyle=t_npc.framestyle;
        DefaultNPCData.noiceball=(int)(!t_npc.freeze_by_iceball);
        //Extended
        DefaultNPCData.nohammer=(int)(!t_npc.kill_hammer);
        DefaultNPCData.noshell=(int)(!t_npc.kill_by_npc);
        DefaultNPCData.name=t_npc.name;
        DefaultNPCData.health=t_npc.health;
        DefaultNPCData.script=t_npc.algorithm_script;
        DefaultNPCData.image=t_npc.image_n;
        DefaultNPCData.grid=t_npc.grid;
        DefaultNPCData.grid_offset_x=0;
        DefaultNPCData.grid_offset_y=0;
        DefaultNPCData.grid_align=t_npc.grid_attach_style;
    }
}


