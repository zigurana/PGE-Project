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

#include "lvl_block.h"
#include "../../data_configs/config_manager.h"

#include "../scene_level.h"
#include <audio/pge_audio.h>
#include <graphics/gl_renderer.h>

LVL_Block::LVL_Block(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    type = LVLBlock;

    animated=false;
    sizable=false;
    animator_ID=0;

    shape=0;
    shape_slope_angle_ratio=0.0;

    offset_x = 0.f;
    offset_y = 0.f;

    isHidden=false;
    destroyed=false;

    fadeOffset=0;
    targetOffset=0;
    fade_step=0;
    fadeSpeed=0;

    taskToTransform=-1;
    taskToTransform_t=0;

    transformedFromBlockID = -1;
    transformedFromNpcID = -1;

    _isInited=false;
}

LVL_Block::~LVL_Block()
{}


void LVL_Block::init()
{
    if(_isInited) return;
    _scene->layers.registerItem(data.layer, this);
    transformTo_x(signed(data.id));
    _isInited=true;
}

void LVL_Block::transformTo(long id, int type)
{
    if(id<=0) return;

    if(type==2)//Other block
    {
        transformTask_block t;
        t.block = this;
        t.id=int(id);
        t.type=type;

        _scene->block_transforms.push_back(t);
    }
    if(type==1)//Other NPC
    {
        LevelNPC def = FileFormats::CreateLvlNpc();
        def.id= unsigned(id);
        def.x = long(round(posX()));
        def.y = long(round(posY()));
        def.direct = 0;
        def.generator = false;
        def.layer = data.layer;
        def.attach_layer = "";
        def.event_activate = "";
        def.event_die = "";
        def.event_talk = "";
        def.event_emptylayer = "";
        LVL_Npc * npc = _scene->spawnNPC(def,
                            LevelScene::GENERATOR_APPEAR,
                            LevelScene::SPAWN_UP, true);
        if(npc)
        {
            npc->transformedFromBlock = this;
            npc->transformedFromBlockID = int(data.id);
            npc->setCenterPos(posRect.center().x(), posRect.center().y());
        }
        destroy( false );
    }
}

void LVL_Block::transformTo_x(long id)
{
    obj_block *newSetup=NULL;
    if(_isInited)
    {
        if(data.id == unsigned(labs(id))) return;

        if(!ConfigManager::lvl_block_indexes.contains(int(id))) return;

        newSetup = &ConfigManager::lvl_block_indexes[int(id)];

        //Remove registration of switch block
        if(setup->switch_Block &&
                ( ((setup->switch_ID != newSetup->switch_ID) && (newSetup->switch_Block)) || (!newSetup->switch_Block) ) )
        {
            if(_scene->switch_blocks.contains(setup->switch_ID))
                _scene->switch_blocks[setup->switch_ID].removeAll(this);
        }
        transformedFromBlockID = int(data.id);//Remember transform source
    } else
        newSetup = &ConfigManager::lvl_block_indexes[int(data.id)];
    data.id = unsigned(id);

    setup = newSetup;

    if(setup->sizable)
    {
        z_index = LevelScene::Z_blockSizable +
                (static_cast<long double>(data.y)/1000000000.0L) + 1 -
                (static_cast<long double>(data.w) *0.00000000000001L);
    }
    else
    {

        if(setup->view==1)
            z_index = LevelScene::Z_BlockFore;
        else
            z_index = LevelScene::Z_Block;
        _scene->zCounter += 0.0000000000001L;
        z_index += _scene->zCounter;
        if( _scene->zCounter >= 1.0L )
            _scene->zCounter=0.0L;
    }

    bool do_init_player_switch=((setup->animator_ID==0)&&(setup->plSwitch_Button));
    bool do_init_player_filter=((setup->animator_ID==0)&&(setup->plFilter_Block));

    long tID = ConfigManager::getBlockTexture(int(data.id));
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[int(tID)].texture;
        texture = ConfigManager::level_textures[int(tID)];
        animated = setup->animated;
        animator_ID = setup->animator_ID;
    }

    if(!setup->sizable)
    {
        data.w = texture.w;
        data.h = (unsigned(texture.h)/setup->frames);
    }

    if(!_isInited)
    {
        posRect.setPos(data.x, data.y);
    }
    setSize(data.w, data.h);

    sizable = setup->sizable;
    isHidden = data.invisible;
    collide_player = COLLISION_ANY;
    slippery_surface = data.slippery;
    if((setup->sizable) || (setup->collision==2))
    {
        collide_player = COLLISION_TOP;
    }
    else
    if(setup->collision==0)
    {
        collide_player = COLLISION_NONE;
    }

    this->shape = setup->phys_shape;
    if(shape==shape_tr_top_right)
        shape_slope_angle_ratio=-(height()/width());
    if(shape==shape_tr_top_left)
        shape_slope_angle_ratio=(height()/width());
    if(shape==shape_tr_bottom_right)
        shape_slope_angle_ratio=(height()/width());
    if(shape==shape_tr_bottom_left)
        shape_slope_angle_ratio=-(height()/width());

    isRectangle=(setup->phys_shape==0);
    if(setup->algorithm==3)
         ConfigManager::Animator_Blocks[int(animator_ID)].setFrames(1, -1);

    // Register switch block
    if(setup->switch_Block)
    {
        if(!_scene->switch_blocks.contains(setup->switch_ID) )
            _scene->switch_blocks[setup->switch_ID].clear();
        _scene->switch_blocks[setup->switch_ID].push_back(this);
        //Fill switch states until it will be fited to defined SwitchID
        while( _scene->switch_states.size() <= setup->switch_ID )
            _scene->switch_states.push_back(false);
    }

    //Register player switch block if needed
    if(do_init_player_switch)
    {
        _scene->character_switchers.buildSwitch(*setup);
    }

    //Register player filter block if needed
    if(do_init_player_filter)
    {
        _scene->character_switchers.buildBrick(*setup);
    }

    collide_npc=collide_player;

}


void LVL_Block::render(double camX, double camY)
{
    //Don't draw hidden block before it will be hitten
    if(isHidden) return;
    if(destroyed) return;

    PGE_RectF blockG;
    blockG.setRect(posX()-camX+offset_x,
                         posY()-camY+offset_y,
                         _width,
                         _height);


    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_Blocks[int(animator_ID)].image();

    GlRenderer::BindTexture(&texture);
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    if(sizable)
    {
        int w = int(round(_width));
        int h = int(round(_height));

        int x,y, x2, y2, i, j;
        int hc, wc;

        x = qRound(qreal(texture.w)/3);  // Width of one piece
        y = qRound(qreal(texture.h)/3); // Height of one piece

        //Double size
        x2 = x<<1;
        y2 = y<<1;

        int pWidth = texture.w-x2;//Width of center piece
        int pHeight = texture.h-y2;//Height of center piece

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < x2) dX = (x2-w)>>1; else dX=0;
        if(h < y2) dY = (y2-h)>>1; else dY=0;

        int totalW=((w-x2)/pWidth);
        int totalH=((h-y2)/pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, PGE_RectF(0, x+hc, x-dX, pHeight), PGE_RectF(0, y, x-dX, pHeight));
                hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(0, x+hc, x-dX, fLnt), PGE_RectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, PGE_RectF(x+hc, 0, pWidth, y-dY), PGE_RectF(x, 0, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x+hc, 0, fLnt, y-dY), PGE_RectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, PGE_RectF(x+hc, h-y+dY, pWidth, y-dY), PGE_RectF(x, texture.h-y+dY, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x+hc, h-y+dY, fLnt, y-dY), PGE_RectF(x, texture.h-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, PGE_RectF(w-x+dX, y+hc, x-dX, pHeight), PGE_RectF(texture.w-x+dX, y, x-dX, pHeight));
                    hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(w-x+dX, y+hc, x-dX, fLnt), PGE_RectF(texture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( (w > (x2)) && (h > (y2)))
        {
            hc=0;
            wc=0;
            for(i=0; i< totalH; i++ )
            {
                hc=0;
                for(j=0; j< totalW; j++ )
                {
                    drawPiece(blockG, PGE_RectF(x+hc, y+wc, pWidth, pHeight), PGE_RectF(x, y, pWidth, pHeight));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, PGE_RectF(x+hc, y+wc, fLnt, pHeight), PGE_RectF(x, y, fLnt, pHeight));
                wc+=pHeight;
            }

            fWdt = (h-y2)%pHeight;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<totalW; j++ )
                {
                    drawPiece(blockG, PGE_RectF(x+hc, y+wc, pWidth, fWdt), PGE_RectF(x, y, pWidth, fWdt));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, PGE_RectF(x+hc, y+wc, fLnt, fWdt), PGE_RectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(blockG, PGE_RectF(0,0,x-dX,y-dY), PGE_RectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(blockG, PGE_RectF(w-x+dX, 0, x-dX, y-dY), PGE_RectF(texture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(blockG, PGE_RectF(w-x+dX, h-y+dY, x-dX, y-dY), PGE_RectF(texture.w-x+dX, texture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(blockG, PGE_RectF(0, h-y+dY, x-dX, y-dY), PGE_RectF(0, texture.h-y+dY, x-dX, y-dY));

    }
    else
    {
        GlRenderer::renderTextureCur( float(blockG.left()), float(blockG.top()), float(blockG.width()), float(blockG.height()), float(x.first), float(x.second) );
    }
    GlRenderer::UnBindTexture();
}

bool LVL_Block::isInited()
{
    return _isInited;
}

long LVL_Block::lua_getID()
{
    return long(data.id);
}

int LVL_Block::lua_contentID_old()
{
    return int(data.npc_id+1000);
}

void LVL_Block::lua_setContentID_old(int npcid)
{
    data.npc_id = npcid-1000;
}

int LVL_Block::lua_contentID()
{
    return int(data.npc_id);
}

void LVL_Block::lua_setContentID(int npcid)
{
    data.npc_id = npcid;
}

bool LVL_Block::lua_invisible()
{
    return data.invisible;
}

void LVL_Block::lua_setInvisible(bool iv)
{
    data.invisible=iv;
}

bool LVL_Block::lua_slippery()
{
    return data.slippery;
}

void LVL_Block::lua_setSlippery(bool sl)
{
    data.slippery=sl;
}

bool LVL_Block::lua_isSolid()
{
    return (collide_npc==COLLISION_ANY);
}

void LVL_Block::drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture)
{
    PGE_RectF tx;
    tx.setLeft(texture.left()/this->texture.w);
    tx.setRight(texture.right()/this->texture.w);
    tx.setTop(texture.top()/this->texture.h);
    tx.setBottom(texture.bottom()/this->texture.h);

    PGE_RectF blockG;
    blockG.setX(target.x()+block.x());
    blockG.setY(target.y()+block.y());
    blockG.setRight(target.x()+block.x()+block.width());
    blockG.setBottom(target.y()+block.y()+block.height());

    GlRenderer::renderTextureCur( float(blockG.left()), float(blockG.top()), float(blockG.width()), float(blockG.height()), float(tx.top()), float(tx.bottom()), float(tx.left()), float(tx.right()) );
}




void LVL_Block::hit(LVL_Block::directions _dir)
{
    hitDirection = _dir;
    isHidden=false;
    data.invisible=false;
    bool doFade=false, triggerEvent=false, playHitSnd=false;

    PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
    if((setup->destroyable)&&(data.npc_id==0))
    {
        destroy( true );
        return;
    }

    if(data.npc_id<0)
    {
        triggerEvent=true;
        playHitSnd=true;
        //Coin!
        PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin);
        data.npc_id++;
        doFade=true;
        if((!setup->bounce)&&(!setup->switch_Button))
        {
            if(data.npc_id==0)
                transformTo(long(setup->transfororm_on_hit_into), 2);
        }

        if(!_scene->player_states.isEmpty())
            _scene->player_states[0].appendCoins(1);

        //! TEMPORARY AND EXPERIMENTAL!, REPLACE THIS WITH LUA
        {
            SpawnEffectDef effect;
            effect.id = 11;
            effect.startX = float(posCenterX());
            effect.startY = float(top())-16.f;
            effect.gravity = 20.0f;
            effect.start_delay = 0.0f;
            effect.max_vel_y = 25.f;
            effect.velocityY = -7.5f;
            effect.delay = 710;
            effect.animationLoops = 0;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(0);
            effect.frame_sequence.append(1);
            effect.frame_sequence.append(2);
            effect.frame_sequence.append(3);
            effect.framespeed = 32;
            _scene->launchEffect(effect, true);

            effect.id = 11;
            effect.startX = (float)posCenterX();
            effect.startY = (float)top()-32.f;
            effect.gravity = 0.0f;
            effect.start_delay = 710.0f;
            effect.max_vel_y = 25.f;
            effect.velocityY = 0.f;
            effect.animationLoops = 1;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(4);
            effect.frame_sequence.append(5);
            effect.frame_sequence.append(6);
            effect.framespeed = 125;
            effect.delay = 0;
            _scene->launchEffect(effect, true);

            //Points!
            effect.id = 79;
            effect.startX = (float)posCenterX() + float( (rand()%16) * ( rand()%2 ? 1 : -1) );
            effect.startY = (float)top()-16.f + float( (rand()%16) * ( rand()%2 ? 1 : -1) );
            effect.gravity = 1.8f;
            effect.start_delay = 710.0f;
            effect.max_vel_y = 7.f;
            effect.velocityY = -1.8f;
            effect.animationLoops = 0;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(0);
            effect.framespeed = 125;
            effect.delay = 1000;
            _scene->launchEffect(effect, true);
        }

    }
    else
    if(data.npc_id>0)
    {
        triggerEvent=true;
        playHitSnd=true;
        //NPC!
        if(ConfigManager::lvl_npc_indexes.contains(int(data.npc_id)))
        {
            obj_npc &npcSet=ConfigManager::lvl_npc_indexes[int(data.npc_id)];

            if(npcSet.block_spawn_sound)
                PGE_Audio::playSoundByRole(obj_sound_role::BlockOpen);

            doFade=true;
            if((!setup->bounce)&&(!setup->switch_Button))
            {
                transformTo(long(setup->transfororm_on_hit_into), 2);
            }

            LevelNPC npcDef = FileFormats::CreateLvlNpc();
            npcDef.id = unsigned(data.npc_id);
              data.npc_id = 0;
            npcDef.direct = 0;
            npcDef.x=data.x;
            npcDef.y=data.y-(hitDirection==up?data.h:(-data.h*2));

            LVL_Npc * npc;
            npc = _scene->spawnNPC(npcDef,
                                        (npcSet.block_spawn_type==0)?
                                             LevelScene::GENERATOR_WARP:
                                             LevelScene::GENERATOR_APPEAR,
                                         hitDirection==up?LevelScene::SPAWN_UP:LevelScene::SPAWN_DOWN,
                                         false);
            if(npc)
            {
                npc->setCenterX(posRect.center().x());
                if(_dir==up)
                    npc->setPosY(posRect.top()-npc->height());
                else
                    npc->setPosY(posRect.bottom());

                if(npcSet.block_spawn_type==1)
                {
                    npc->setSpeedY(fabs(npcSet.block_spawn_speed)*((_dir==up)?-1:1));
                }
            }
        }
    }

    if(setup->switch_Button)
    {
        triggerEvent=true;
        _scene->toggleSwitch(setup->switch_ID);
    }

    if(setup->hitable)
    {
        triggerEvent=true;
        transformTo(long(setup->spawn_obj_id), setup->spawn_obj);
        doFade = true;
        playHitSnd=!destroyed;
    }

    if(playHitSnd && (setup->hit_sound_id>0))
    {
        PGE_Audio::playSound(setup->hit_sound_id);
    }

    if(triggerEvent && (!data.event_hit.isEmpty()))
    {
        _scene->events.triggerEvent(data.event_hit);
    }

    if(doFade)
    {
        if(!isFading())
            _scene->fading_blocks.push_back(this);
        fadeOffset=0.f;
        setFade(5, 1.0f, 0.07f);
    }
}

void LVL_Block::hit(bool isUp, LVL_Player *, int numHits)
{
    while(numHits>0)
    {
        hit(isUp?up:down);
        numHits--;
    }
}

void LVL_Block::destroy(bool playEffect)
{
    if( playEffect )
    {
        if( setup->destroy_sound_id == 0 )
            PGE_Audio::playSoundByRole(obj_sound_role::BlockSmashed);
        else
            PGE_Audio::playSound(setup->destroy_sound_id);

        Scene_Effect_Phys p;
        p.decelerate_x = 1.5f;
        p.max_vel_y    = 25.f;
        #define ROFFSET ((float(rand()%10))*0.1f)
        _scene->launchStaticEffectC(1, float(posCenterX()), float(posCenterY()), 0, 5000, -3.0f + ROFFSET, -6.0f + ROFFSET, 18.0f, 0, p);
        _scene->launchStaticEffectC(1, float(posCenterX()), float(posCenterY()), 0, 5000, -4.0f + ROFFSET, -7.0f + ROFFSET, 18.0f, 0, p);
        _scene->launchStaticEffectC(1, float(posCenterX()), float(posCenterY()), 0, 5000,  3.0f + ROFFSET, -6.0f + ROFFSET, 18.0f, 0, p);
        _scene->launchStaticEffectC(1, float(posCenterX()), float(posCenterY()), 0, 5000,  4.0f + ROFFSET, -7.0f + ROFFSET, 18.0f, 0, p);
    }
    destroyed = true;
    QString oldLayer=data.layer;
    _scene->layers.removeRegItem(data.layer, this);
    data.layer="Destroyed Blocks";
    _scene->layers.registerItem(data.layer, this);
    if(!data.event_destroy.isEmpty())
    {
        _scene->events.triggerEvent(data.event_destroy);
    }
    if(!data.event_emptylayer.isEmpty())
    {
        if(_scene->layers.isEmpty(oldLayer))
            _scene->events.triggerEvent(data.event_emptylayer);
    }
}

long double LVL_Block::zIndex()
{
    if( fadeOffset != 0.f )
        return z_index+10;
    else
        return z_index;
}


/**************************Fader*******************************/
void LVL_Block::setFade(int speed, float target, float step)
{
    fade_step = fabsf(step);
    targetOffset = target;
    fadeSpeed = speed;
    manual_ticks = speed;
}

bool LVL_Block::isFading()
{
    return (fadeOffset != targetOffset);
}

bool LVL_Block::tickFader(int ticks)
{
    if(fadeSpeed<1) return true; //Idling animation
    manual_ticks-=abs(ticks);
        while(manual_ticks<=0)
        {
            fadeStep();
            manual_ticks+=fadeSpeed;
        }
    return (fadeOffset == targetOffset);
}

void LVL_Block::fadeStep()
{
    if(fadeOffset < targetOffset)
        fadeOffset+=fade_step;
    else
        fadeOffset-=fade_step;

    if(fadeOffset>=1.0f)
        setFade(fadeSpeed, 0.0f, fade_step);

    if(fadeOffset>1.0f) fadeOffset = 1.0f;
    else
    if(fadeOffset<0.0f) fadeOffset = 0.0f;

    switch(hitDirection)
    {
        case up:      offset_y = -16*fadeOffset; break;
        case down:    offset_y = 16*fadeOffset; break;
        case left:    offset_x = -16*fadeOffset; break;
        case right:   offset_x = 16*fadeOffset; break;
        //default: break;
    }

}
/**************************Fader**end**************************/
