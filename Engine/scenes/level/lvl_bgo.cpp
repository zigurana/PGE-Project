/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_bgo.h"
#include "../../data_configs/config_manager.h"
#include <graphics/gl_renderer.h>
#include "../scene_level.h"

LVL_Bgo::LVL_Bgo(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    type = LVLBGO;
    data=FileFormats::CreateLvlBgo();
    animated=false;
    animator_ID=0;
    _isInited=false;
}

LVL_Bgo::~LVL_Bgo()
{}

void LVL_Bgo::init()
{
    if(_isInited) return;
    transformTo_x(data.id);
    collide_player=COLLISION_NONE;
    collide_npc = COLLISION_NONE;
    _isInited=true;
    _scene->layers.registerItem(data.layer, this);
}

void LVL_Bgo::transformTo_x(long id)
{
    if(_isInited)
    {
        if(data.id==(unsigned)labs(id)) return;
        if(!ConfigManager::lvl_bgo_indexes.contains(id))
            return;
    }
    data.id=id;
    setup = &ConfigManager::lvl_bgo_indexes[data.id];
    long double targetZ = 0.0L;
    long double zOffset = (long double)setup->zOffset;
    int zMode = data.z_mode;

    if(zMode==LevelBGO::ZDefault)
    {
        switch(setup->view)
        {
            case -1: zMode = LevelBGO::Background2;break;
            case 0: zMode = LevelBGO::Background1;break;
            case 1: zMode = LevelBGO::Foreground1;break;
            case 2: zMode = LevelBGO::Foreground2;break;
        }
    }

    switch(zMode)
    {
        case LevelBGO::Background2:
            targetZ = LevelScene::Z_BGOBack2 + zOffset + (long double)data.z_offset; break;
        case LevelBGO::Background1:
            targetZ = LevelScene::Z_BGOBack1 + zOffset + (long double)data.z_offset; break;
        case LevelBGO::Foreground1:
            targetZ = LevelScene::Z_BGOFore1 + zOffset + (long double)data.z_offset; break;
        case LevelBGO::Foreground2:
            targetZ = LevelScene::Z_BGOFore2 + zOffset + (long double)data.z_offset; break;
        default:
            targetZ = LevelScene::Z_BGOBack1 + zOffset + (long double)data.z_offset; break;
    }

    z_index = targetZ;

    _scene->zCounter += 0.0000000000001L;
    z_index += _scene->zCounter;
    if( _scene->zCounter >= 1.0L )
        _scene->zCounter=0.0L;

    long tID = ConfigManager::getBgoTexture(data.id);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = setup->animated;
        animator_ID = setup->animator_ID;
    }
    if(!_isInited)
        posRect.setPos(data.x, data.y);
    setSize(texture.frame_w, texture.frame_h);
}

void LVL_Bgo::render(double camX, double camY)
{
    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_BGO[animator_ID].image();
    GlRenderer::renderTexture(&texture, posX()-camX, posY()-camY, _width, _height, x.first, x.second);
}

bool LVL_Bgo::isInited()
{
    return _isInited;
}

long LVL_Bgo::lua_getID()
{
    return data.id;
}
