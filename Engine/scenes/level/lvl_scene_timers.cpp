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

#include "../scene_level.h"
#include "../../common_features/simple_animator.h"
#include "../../common_features/graphics_funcs.h"

#include <fontman/font_manager.h>
#include <networking/intproc.h>
#include <graphics/gl_renderer.h>


/**************************LoadAnimation*******************************/
namespace lvl_scene_loader
{
    SimpleAnimator * loading_Ani = NULL;
    PGE_Texture loading_texture;
}

void LevelScene::drawLoader()
{
    using namespace lvl_scene_loader;

    if(!loading_Ani) return;

    GlRenderer::clearScreen();

    GlRenderer::renderRect(0,0,PGE_Window::Width, PGE_Window::Height, 0.f, 0.f, 0.f, 1.0f);

    PGE_RectF loadAniG;
    loadAniG.setRect(PGE_Window::Width/2 - loading_texture.w/2,
                     PGE_Window::Height/2 - (loading_texture.h/4)/2,
                     loading_texture.w,
                     loading_texture.h/4);

    GlRenderer::resetViewport();
    AniPos x(0,1);
            x = loading_Ani->image();
    GlRenderer::renderTexture(&loading_texture, loadAniG.left(), loadAniG.top(), loadAniG.width(), loadAniG.height(), x.first, x.second);

    if(IntProc::isEnabled())
        FontManager::printText(QString("%1")
                               .arg(IntProc::state), 10,10);
}


void LevelScene::setLoaderAnimation(int speed)
{
    using namespace lvl_scene_loader;
    loaderSpeed = speed;

    if(IntProc::isEnabled())
    {
        if(!loading_texture.inited)
            GlRenderer::loadTextureP(loading_texture, ":/images/coin.png");
    }
    else
    {
        if(!loading_texture.inited)
            GlRenderer::loadTextureP(loading_texture, ":/images/shell.png");
    }

    loading_Ani = new SimpleAnimator(true,
                                     4,
                                     128,
                                     0, -1, false, false);
    loading_Ani->start();

    loader_timer_id = SDL_AddTimer(speed, &LevelScene::nextLoadAniFrame, this);
    IsLoaderWorks = true;
}

void LevelScene::stopLoaderAnimation()
{
    using namespace lvl_scene_loader;

    doLoaderStep = false;
    IsLoaderWorks = false;
    SDL_RemoveTimer(loader_timer_id);

    render();
    if(loading_Ani)
    {
        loading_Ani->stop();
        delete loading_Ani;
        loading_Ani = NULL;
    }

}

void LevelScene::destroyLoaderTexture()
{
    using namespace lvl_scene_loader;
    GlRenderer::deleteTexture( loading_texture );
}

unsigned int LevelScene::nextLoadAniFrame(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->loaderTick();
    return 0;
}

void LevelScene::loaderTick()
{
    doLoaderStep = true;
}

void LevelScene::loaderStep()
{
    if(!IsLoaderWorks) return;
    if(!doLoaderStep) return;

    SDL_Event event; //  Events of SDL
    while ( SDL_PollEvent(&event) )
    {
        PGE_Window::processEvents(event);
        switch(event.type)
        {
            case SDL_QUIT:
                //Give able to quit from game even loading process is not finished
                isLevelContinues=false;
                doExit=true;
            break;
        }
    }

    drawLoader();

    GlRenderer::flush();
    GlRenderer::repaint();

    loader_timer_id = SDL_AddTimer(loaderSpeed, &LevelScene::nextLoadAniFrame, this);
    doLoaderStep = false;
}

LevelData *LevelScene::levelData()
{
    return &data;
}

/**************************LoadAnimation**end**************************/


