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

#include "lvl_base_object.h"
#include <scenes/scene_level.h>
#include <graphics/gl_renderer.h>
#include <common_features/maths.h>

#include <QVector>

#include "collision_checks.h"

const float PGE_Phys_Object::_smbxTickTime = 15.285f;
//1000.f/65.f; Thanks to Rednaxela for hint, 15.6 is a true frame time in SMBX Engine!
//BUT, Experimentally was found that in real is approximately is 15.285

float PGE_Phys_Object::SMBXTicksToTime(float ticks)
{
    return ticks*_smbxTickTime;
}

PGE_Phys_Object::PGE_Phys_Object(LevelScene *_parent) :
    _vizible_on_screen(false),
    _render_list(false),
    _scene(_parent),
    _is_registered(false),
    _posX(0.0),
    _posY(0.0),
    _width(1.0),
    _height(1.0),
    _width_half(0.5),
    _height_half(0.5),
    _realWidth(1.0),
    _realHeight(1.0),
    type(LVLUnknown)
{
    _width_half = 0.0f;
    _height_half = 0.0f;

    z_index = 0.0L;
    isRectangle = true;

    slippery_surface = false;
    collide_player = COLLISION_ANY;
    collide_npc = COLLISION_ANY;

    collided_slope=false;
    collided_slope_angle_ratio=0.0f;
    collided_slope_celling=false;
    collided_slope_angle_ratio_celling=0.0f;

    _parentSection=NULL;
    _width = 0.0;
    _height = 0.0;
    _realWidth=0.0;
    _realHeight=0.0;
    _posX=0.0;
    _posY=0.0;
    _velocityX=0.0;
    _velocityY=0.0;
    _velocityX_prev=0.0;
    _velocityY_prev=0.0;
    _velocityX_add=0.0;
    _velocityY_add=0.0;

    colliding_xSpeed=0.0;
    colliding_ySpeed=0.0;

    _paused=false;

    _is_visible=true;

    _accelX=0;
    _accelY=0;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if(_is_registered) _scene->unregisterElement(this);
}

void PGE_Phys_Object::registerInTree()
{
    if(!_is_registered) _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::unregisterFromTree()
{
    if(_is_registered) _scene->unregisterElement(this);
    _is_registered=false;
}

double PGE_Phys_Object::posX()
{
    return posRect.x();
}

double PGE_Phys_Object::posY()
{
    return posRect.y();
}

double PGE_Phys_Object::posCenterX()
{
    return posRect.center().x();
}

double PGE_Phys_Object::posCenterY()
{
    return posRect.center().y();
}

void PGE_Phys_Object::setCenterX(double x)
{
    setPosX(x-_width_half);
}

void PGE_Phys_Object::setCenterY(double y)
{
    setPosY(y-_height_half);
}

double PGE_Phys_Object::width()
{
    return posRect.width();
}

double PGE_Phys_Object::height()
{
    return posRect.height();
}

double PGE_Phys_Object::top()
{
    return posY();
}

void PGE_Phys_Object::setTop(double tp)
{
    posRect.setTop(tp);
    _realHeight=posRect.height();
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::bottom()
{
    return posY()+_height;
}

void PGE_Phys_Object::setBottom(double btm)
{
    posRect.setBottom(btm);
    _realHeight=posRect.height();
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::left()
{
    return posX();
}

void PGE_Phys_Object::setLeft(double lf)
{
    posRect.setLeft(lf);
    _realWidth=posRect.width();
    _height_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

double PGE_Phys_Object::right()
{
    return posX()+_width;
}

void PGE_Phys_Object::setRight(double rt)
{
    posRect.setRight(rt);
    _realWidth=posRect.width();
    _height_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setSize(float w, float h)
{
    posRect.setSize(w, h);
    _realWidth=w;
    _realHeight=h;
    _width_half = _realWidth/2.0f;
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setWidth(float w)
{
    posRect.setWidth(w);
    _realWidth=w;
    _width_half = _realWidth/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setHeight(float h)
{
    posRect.setHeight(h);
    _realHeight=h;
    _height_half = _realHeight/2.0f;
    _syncPositionAndSize();
}

void PGE_Phys_Object::setPos(double x, double y)
{
    posRect.setPos(x, y);
    _syncPosition();
}

void PGE_Phys_Object::setPosX(double x)
{
    posRect.setX(x);
    _syncPosition();
}

void PGE_Phys_Object::setPosY(double y)
{
    posRect.setY(y);
    _syncPosition();
}

void PGE_Phys_Object::setCenterPos(double x, double y)
{
    setPos(x-_width_half, y-_height_half);
}

double PGE_Phys_Object::speedX()
{
    return _velocityX;
}

double PGE_Phys_Object::speedY()
{
    return _velocityY;
}

double PGE_Phys_Object::speedXsum()
{
    return _velocityX+_velocityX_add;
}

double PGE_Phys_Object::speedYsum()
{
    return _velocityY+_velocityY_add;
}

void PGE_Phys_Object::setSpeed(double x, double y)
{
    _velocityX=x;
    _velocityY=y;
    updateSpeedAddingStack();
    _velocityX_prev=_velocityX;
    _velocityY_prev=_velocityY;
}

void PGE_Phys_Object::setSpeedX(double x)
{
    _velocityX=x;
    updateSpeedAddingStack();
    _velocityX_prev=_velocityX;
}

void PGE_Phys_Object::setSpeedY(double y)
{
    _velocityY=y;
    updateSpeedAddingStack();
    _velocityY_prev=_velocityY;
}

void PGE_Phys_Object::setDecelX(double x)
{
    phys_setup.decelerate_x = float(x);
}

void PGE_Phys_Object::applyAccel(double x, double y)
{
    _accelX=x;
    _accelY=y;
}

double PGE_Phys_Object::gravityScale()
{
    return phys_setup.gravityScale;
}

void PGE_Phys_Object::setGravityScale(double scl)
{
    phys_setup.gravityScale = float(scl);
}

float PGE_Phys_Object::gravityAccel()
{
    return phys_setup.gravityAccel;
}

void PGE_Phys_Object::setGravityAccel(float acl)
{
    phys_setup.gravityAccel = fabsf(acl);
}


void PGE_Phys_Object::_syncPosition()
{
    if(_is_registered) _scene->unregisterElement(this);
    _posX= posRect.x();
    _posY= posRect.y();
    _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::_syncPositionAndSize()
{
    if(_is_registered) _scene->unregisterElement(this);
    _posX= posRect.x();
    _posY= posRect.y();
    _width=_realWidth;
    _height=_realHeight;
    _scene->registerElement(this);
    _is_registered=true;
}

void PGE_Phys_Object::_syncSection(bool sync_position)
{
    int sID = _scene->findNearestSection(long(posX()), long(posY()));
    LVL_Section *sct = _scene->getSection(sID);
    if(sct)
    {
        setParentSection(sct);
    }
    if(sync_position) _syncPosition();
}

void PGE_Phys_Object::renderDebug(float _camX, float _camY)
{
    switch(type)
    {
        case LVLUnknown:    GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, false); break;
        case LVLBlock:      GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 0.0f, 1.0f, 0.0f, 1.0f, false); break;
        case LVLBGO:        GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 0.0f, 0.0f, 1.0f, 1.0f, false); break;
        case LVLNPC:        GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 0.0f, 1.0f, 1.0f, false); break;
        case LVLPlayer:     GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 0.5f, 0.5f, 1.0f, false); break;
        case LVLEffect:     GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 0.5f, 0.5f, 0.5f, 1.0f, false); break;
        case LVLWarp:       GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 0.0f, 0.0f, 0.5f, true);  break;
        case LVLSpecial:    GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, true);  break;
        case LVLPhysEnv:    GlRenderer::renderRect(float(posRect.x())-_camX, float(posRect.y())-_camY, float(posRect.width())-1.0f, float(posRect.height())-1.0f, 1.0f, 1.0f, 0.0f, 0.5f, true);  break;
    }
}


void PGE_Phys_Object::iterateStep(float ticks)
{
    if(_paused) return;

    bool updateSpeedAdding=false;

    posRect.setX(posRect.x()+(_velocityX+_velocityX_add) * (ticks/_smbxTickTime));
    _velocityX_prev=_velocityX;

    if(collided_slope)
    {
        _velocityY_prev = (_velocityY+_velocityY_add+(_velocityX*collided_slope_angle_ratio));
        posRect.setY(posRect.y()+ _velocityY_prev * (ticks/_smbxTickTime));
    }
    else
    {
        posRect.setY(posRect.y()+(_velocityY+_velocityY_add) * (ticks/_smbxTickTime));
        _velocityY_prev=_velocityY;
    }

    colliding_xSpeed = Maths::max(fabs(_velocityX+_velocityX_add), fabs(_velocityX_prev+_velocityX_add))
            * Maths::sgn(speedX()+_velocityX_add)*(ticks/_smbxTickTime);
    colliding_ySpeed = Maths::max(fabs(_velocityY+_velocityY_add), fabs(_velocityY_prev+_velocityY_add))
            * Maths::sgn(speedY()+_velocityY_add)*(ticks/_smbxTickTime);

    float G = phys_setup.gravityScale * _scene->globalGravity;
    float accelCof=ticks/1000.0f;

    if(_accelX != 0.0f)
    {
        _velocityX+= _accelX*accelCof;
        updateSpeedAdding=true;
        _accelX=0;
    }
    else
    if(phys_setup.decelerate_x != 0.0f)
    {
        float decX=phys_setup.decelerate_x*accelCof;
        if(_velocityX>0)
        {
            if(_velocityX-decX>0.0)
                _velocityX-=decX;
            else
                _velocityX=0;
        } else if(_velocityX<0) {
            if(_velocityX+decX<0.0)
                _velocityX+=decX;
            else
                _velocityX=0;
        }
        updateSpeedAdding=true;
    }

    if(_accelY != 0.0f)
    {
        _velocityY+= _accelY*accelCof*G;
        updateSpeedAdding=true;
        _accelY = 0.0f;
    }

    if(phys_setup.decelerate_y != 0.0f)
    {
        float decY=phys_setup.decelerate_y*accelCof;
        if(_velocityY>0)
        {
            if(_velocityY-decY>0.0)
                _velocityY-=decY;
            else
                _velocityY=0;
        } else if(_velocityY<0) {
            if(_velocityY+decY<0.0)
                _velocityY+=decY;
            else
                _velocityY=0;
        }
        updateSpeedAdding=true;
    }

    if(phys_setup.gravityAccel != 0.0f)
    {
        _velocityY+= (G*phys_setup.gravityAccel)*accelCof;
        updateSpeedAdding=true;
    }

    if((phys_setup.max_vel_x != 0.0f)&&(_velocityX>phys_setup.max_vel_x)) { _velocityX-=phys_setup.grd_dec_x*accelCof;updateSpeedAdding=true;}
    if((phys_setup.min_vel_x != 0.0f)&&(_velocityX<phys_setup.min_vel_x)) { _velocityX+=phys_setup.grd_dec_x*accelCof;updateSpeedAdding=true;}
    if((phys_setup.max_vel_y != 0.0f)&&(_velocityY>phys_setup.max_vel_y)) {_velocityY=phys_setup.max_vel_y;updateSpeedAdding=true;}
    if((phys_setup.min_vel_y != 0.0f)&&(_velocityY<phys_setup.min_vel_y)) {_velocityY=phys_setup.min_vel_y;updateSpeedAdding=true;}

    if(updateSpeedAdding)
        updateSpeedAddingStack();
}

void PGE_Phys_Object::updateCollisions()
{
    if(_paused) return;

    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = posRect.withMargin(2.0);
    _scene->queryItems(posRectC, &bodies);

    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*body=*it;
        if(body==this) continue;
        if(body->_paused) continue;
        if(!body->_is_visible) continue;

        detectCollisions(body);
    }
}

void PGE_Phys_Object::detectCollisions(PGE_Phys_Object *)
{}



bool PGE_Phys_Object::isWall(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.isEmpty())
        return false;
    double higher = blocks.first()->posRect.top();
    double lower  = blocks.first()->posRect.bottom();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->posRect.bottom()>lower)
            lower = blocks[i]->posRect.bottom();
        if(blocks[i]->posRect.top()<higher)
            higher = blocks[i]->posRect.top();
    }
    if(posRect.top() >= lower) return false;
    if(posRect.bottom() <= higher) return false;
    return true;
}

bool PGE_Phys_Object::isFloor(QVector<PGE_Phys_Object*> &blocks, bool *isCliff)
{
    if(isCliff)
        *isCliff=false;

    if(blocks.isEmpty())
        return false;
    double lefter  = blocks.first()->posRect.left();
    double righter = blocks.first()->posRect.right();
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->posRect.right()>righter)
            righter=blocks[i]->posRect.right();
        if(blocks[i]->posRect.left()<lefter)
            lefter=blocks[i]->posRect.left();
    }

    if(posRect.left() >= righter) return false;
    if(posRect.right() <= lefter) return false;

    if(isCliff)
    {
        if((speedX() < 0.0) && ( lefter > posRect.center().x()) )
            *isCliff=true;
        else
        if((speedX() > 0.0) && (righter < posRect.center().x()) )
            *isCliff=true;
    }
    return true;
}

PGE_Phys_Object *PGE_Phys_Object::nearestBlock(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    PGE_Phys_Object*nearest=NULL;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            if( fabs(blocks[i]->posRect.center().x()-posRect.center().x())<
                fabs(nearest->posRect.center().x()-posRect.center().x()) )
                nearest=blocks[i];
        }
    }
    return nearest;
}

PGE_Phys_Object *PGE_Phys_Object::nearestBlockY(QVector<PGE_Phys_Object *> &blocks)
{
    if(blocks.size()==1)
        return blocks.first();

    PGE_Phys_Object*nearest=NULL;
    double nearest_blockY=0.0;
    double blockY=0.0;
    for(int i=0; i<blocks.size(); i++)
    {
        if(!nearest)
            nearest=blocks[i];
        else
        {
            //Check for a possible slope
            if(blocks[i]->type==PGE_Phys_Object::LVLBlock)
            {
                LVL_Block* b = static_cast<LVL_Block*>(blocks[i]);
                switch(b->shape)
                {
                    case LVL_Block::shape_tr_bottom_right:
                        blockY=nearest->posRect.top()+SL_HeightTopRight(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_bottom_left:
                        blockY=nearest->posRect.top()+SL_HeightTopLeft(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_top_right:
                        blockY=nearest->posRect.bottom()-SL_HeightTopRight(*this, nearest);
                        break;

                    case LVL_Block::shape_tr_top_left:
                        blockY=nearest->posRect.bottom()-SL_HeightTopLeft(*this, nearest);
                        break;

                    default:break;
                        blockY=blocks[i]->posRect.center().y();
                }
            } else blockY=blocks[i]->posRect.center().y();
            if( fabs(blockY-posRect.center().y())<
                fabs(nearest_blockY-posRect.center().y()) )
            {
                nearest=blocks[i];
                nearest_blockY=blockY;
            }
        }
    }
    return nearest;
}




void PGE_Phys_Object::setParentSection(LVL_Section *sct)
{
    _parentSection=sct;
}

LVL_Section *PGE_Phys_Object::sct()
{
    return _parentSection;
}

long double PGE_Phys_Object::zIndex()
{
    return z_index;
}

void PGE_Phys_Object::update() { _syncPosition(); }

void PGE_Phys_Object::update(float)
{
    _syncPosition();
}

void PGE_Phys_Object::render(double x, double y) {Q_UNUSED(x); Q_UNUSED(y);}

bool PGE_Phys_Object::isPaused()
{
    return _paused;
}

void PGE_Phys_Object::setPaused(bool p)
{
    _paused=p;
}


bool operator<(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index>rhs.z_index; }


bool operator>(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index<rhs.z_index; }


PGE_Phys_Object_Phys::PGE_Phys_Object_Phys()
{
    min_vel_x=0.f;
    min_vel_y=0.f;
    max_vel_x=0.f;
    max_vel_y=0.f;
    grd_dec_x=0.f;
    decelerate_x=0.f;
    decelerate_y=0.f;
    gravityScale=1.0f;
    gravityAccel=26.0f;
}




void PGE_Phys_Object::show()
{
    _is_visible=true;
}

void PGE_Phys_Object::hide()
{
    _is_visible=false;
}

bool PGE_Phys_Object::isVisible()
{
    return _is_visible;
}


