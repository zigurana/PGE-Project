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

#include "scene_level.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <settings/debugger.h>

#include <graphics/gl_renderer.h>

#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <data_configs/config_manager.h>
#include <fontman/font_manager.h>

#include <gui/pge_msgbox.h>
#include <gui/pge_textinputbox.h>

#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>
#include <settings/global_settings.h>

#include <QApplication>
#include <QtDebug>

#include <common_features/logger.h>

#include <script/lua_event.h>
#include <script/bindings/core/events/luaevents_core_engine.h>

#include <QElapsedTimer>

QElapsedTimer debug_TimeReal;
int           debug_TimeCounted=0;

LevelScene::LevelScene()
    : Scene(Level),
      isInit(false),
      isInitFailed(false),
      isWarpEntrance(false),
      cameraStartDirected(false),
      cameraStartDirection(0),
      NewPlayerID(1),
      /*********Exit*************/
      isLevelContinues(true),
      warpToLevelFile(""),
      lastWarpID(0),
      warpToArrayID(0),
      warpToWorld(false),
      exitLevelDelay(3000),
      exitLevelCode(LvlExit::EXIT_Closed),
      /**************************/
      character_switchers(this),
      data(FileFormats::CreateLevelData()),
      /*********Physics**********/
      globalGravity(1.0),
      /**************************/
      luaEngine(this)
{
    tree.RemoveAll();
    layers._scene=this;
    events._scene=this;

    data.ReadFileValid = false;

    zCounter = 0.0L;

    /**************************/
    placingMode=false;
    placingMode_item_type=0;
    placingMode_block=FileFormats::CreateLvlBlock();
    placingMode_bgo  =FileFormats::CreateLvlBgo();
    placingMode_npc  =FileFormats::CreateLvlNpc();
    placingMode_animatorID=0;
    placingMode_animated=false;
    placingMode_sizableBlock=false;
    placingMode_rect_draw=false;
    /**************************/

    /*********Default players number*************/
    numberOfPlayers=2;
    /*********Default players number*************/

    /*********Loader*************/
    IsLoaderWorks=false;
    /*********Loader*************/

    /*********Fader*************/
    fader.setFull();
    /*********Fader*************/

    /*********Controller********/
    player1Controller = g_AppSettings.openController(1);
    player2Controller = g_AppSettings.openController(2);
    /*********Controller********/

    /*********Pause menu*************/
    initPauseMenu1();
    /*********Pause menu*************/

    frameSkip=g_AppSettings.frameSkip;

    errorMsg = "";

    gameState = NULL;

    debug_player_jumping=false;
    debug_player_onground=false;
    debug_player_foots=0;
    debug_render_delay=0;
    debug_phys_delay=0;
    debug_event_delay=0;
}

void LevelScene::processPhysics(float ticks)
{
    //Iterate layer movement
    layers.processMoving(uTickf);

    //Iterate playable characters
    for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
    {
        LVL_Player*plr=(*it);
        plr->iterateStep(ticks);
        plr->_syncPosition();
    }
    //Iterate activated NPCs
    for(int i=0;i<active_npcs.size();i++)
    {
        active_npcs[i]->iterateStep(ticks);
        active_npcs[i]->_syncPosition();
    }

    //Process collision check and resolving for playable characters
    for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
    {
        LVL_Player*plr=(*it);
        plr->updateCollisions();
    }
    //Process collision check and resolving for activated NPC's
    for(int i=0;i<active_npcs.size();i++)
    {
        active_npcs[i]->updateCollisions();
    }
}




LevelScene::~LevelScene()
{
    GlRenderer::clearScreen();
    layers.members.clear();

    switch_blocks.clear();
    //destroy textures
    int i=0;
    qDebug() << "clear level textures";
    for(i=0; i<textures_bank.size(); i++)
    {
        GlRenderer::deleteTexture( textures_bank[i] );
    }
    textures_bank.clear();

    qDebug() << "Destroy cameras";
    cameras.clear();

    qDebug() << "Destroy players";
    for(i=0; i<players.size(); i++)
    {
        LVL_Player* tmp;
        tmp = players[i];
        player1Controller->removeFromControl(tmp);
        if(tmp)
        {
            tmp->unregister();
            if(!tmp->isLuaPlayer)
                delete tmp;
        }
    }

    qDebug() << "Destroy blocks";
    for(i=0; i<blocks.size(); i++)
    {
        LVL_Block* tmp;
        tmp = blocks[i];
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) { tmp->unregisterFromTree(); delete tmp;}
    }

    qDebug() << "Destroy BGO";
    for(i=0; i<bgos.size(); i++)
    {
        LVL_Bgo* tmp;
        tmp = bgos[i];
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) { tmp->unregisterFromTree(); delete tmp;}
    }

    qDebug() << "Destroy NPC";
    for(i=0; i<npcs.size(); i++)
    {
        LVL_Npc* tmp;
        tmp = npcs[i];
        if(tmp)
        {
            tmp->unregister();
            if(!tmp->isLuaNPC)
                delete tmp;
        }
    }
    //npcs.clear();
//    while(!npcs.isEmpty())
//    {
//        LVL_Npc* tmp;
//        tmp = npcs.last();
//        npcs.pop_back();
//    }


    qDebug() << "Destroy Warps";
    for(i=0; i<warps.size(); i++)
    {
        LVL_Warp* tmp;
        tmp = warps[i];
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) { tmp->unregisterFromTree(); delete tmp;}
    }

    qDebug() << "Destroy Physical Environment zones";
    for(i=0; i<physenvs.size(); i++)
    {
        LVL_PhysEnv* tmp;
        tmp = physenvs[i];
        layers.removeRegItem(tmp->data.layer, tmp);
        if(tmp) { tmp->unregisterFromTree(); delete tmp;}
    }

    qDebug() << "Destroy sections";
    sections.clear();

    luaEngine.shutdown();

    destroyLoaderTexture();

    delete player1Controller;
    delete player2Controller;
    textures_bank.clear();
}


void LevelScene::tickAnimations(float ticks)
{
    //tick animation
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_Blocks.begin();
        it!=ConfigManager::Animator_Blocks.end(); it++)
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_BGO.begin();
        it!=ConfigManager::Animator_BGO.end(); it++)
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_BG.begin();
        it!=ConfigManager::Animator_BG.end(); it++)
        it->manualTick(ticks);
    for(QList<AdvNpcAnimator>::iterator it=ConfigManager::Animator_NPC.begin();
        it!=ConfigManager::Animator_NPC.end(); it++)
        it->manualTick(ticks);
}


void LevelScene::update()
{
    if(luaEngine.shouldShutdown()){
        fader.setFade(10, 1.0f, 1.0f);
        setExiting(0, LvlExit::EXIT_Error);
    }
    Scene::update();
    tickAnimations(uTickf);

    if(!isLevelContinues)
    {
        //Level exit timeout
        exitLevelDelay -= uTickf;
        if(exitLevelDelay<=0.f)
        {
            doExit=true;
            if(fader.isNull())
            {
                if(PGE_MusPlayer::MUS_IsPlaying())
                    PGE_MusPlayer::MUS_stopMusicFadeOut(500);
                fader.setFade(10, 1.0f, 0.01f);
            }
        }
    }

    if(doExit)
    {
        if(exitLevelCode==LvlExit::EXIT_Closed)
        {
            fader.setFull();
            running=false;
        }
        else
        {
            if(fader.isFull())
                running=false;
        }
    } else if(isPauseMenu) {
        processPauseMenu();
    } else {//Update physics is not pause menu

        updateLua();//Process LUA code

        system_events.processEvents(uTickf);
        events.processTimers(uTickf);

        processEffects(uTickf);

        if(!isTimeStopped) //if activated Time stop bonus or time disabled by special event
        {
            //Make world step
            processPhysics(uTickf);
        }

        while(!block_transforms.isEmpty())
        {
            transformTask_block x = block_transforms.first();
            x.block->transformTo_x(x.id);
            block_transforms.pop_front();
        }

        //Update controllers
        player1Controller->sendControls();
        player2Controller->sendControls();

        //update players
        for(LVL_PlayersArray::iterator it=players.begin(); it!=players.end(); it++)
        {
            LVL_Player*plr=(*it);
            plr->update(uTickf);
            if(PGE_Window::showDebugInfo)
            {
                debug_player_jumping = plr->JumpPressed;
                debug_player_onground= plr->onGround();
                debug_player_foots   = plr->foot_contacts_map.size();
            }
        }

        for(int i=0;i<fading_blocks.size();i++)
        {
            if(fading_blocks[i]->tickFader(uTickf))
            {
                fading_blocks.removeAt(i); i--;
            }
        }

        //Process activated NPCs
        for(int i=0;i<active_npcs.size();i++)
        {
            active_npcs[i]->update(uTickf);
            if(active_npcs[i]->isKilled())
            {
                active_npcs.removeAt(i); i--;
            }
            else
            if(active_npcs[i]->activationTimeout<=0)
            {
                if(!active_npcs[i]->warpSpawing)
                    active_npcs[i]->deActivate();
                if(active_npcs[i]->wasDeactivated)
                {
                    if(!isVizibleOnScreen(active_npcs[i]->posRect)||!active_npcs[i]->isVisible() || !active_npcs[i]->is_activity)
                    {
                        active_npcs[i]->wasDeactivated=false;
                        active_npcs.removeAt(i); i--;
                    }
                }
            }
        }

        /***************Collect garbage****************/
        if(!dead_npcs.isEmpty())
            collectGarbageNPCs();
        if(!dead_players.isEmpty())
            collectGarbagePlayers();
        /**********************************************/

        //update cameras
        for(QList<PGE_LevelCamera>::iterator cam=cameras.begin();cam!=cameras.end(); cam++)
        {
            cam->update(uTickf);

            //! --------------DRAW HUD--------------------------------------
            LuaEngine* sceneLuaEngine = getLuaEngine();
            if(sceneLuaEngine)
            {
                if(sceneLuaEngine->isValid() && !sceneLuaEngine->shouldShutdown())
                {
                    LuaEvent drawHUDEvent = BindingCore_Events_Engine::createDrawLevelHUDEvent(sceneLuaEngine,
                                                                                              &(*cam),
                                                                                              &player_states[(*cam).playerID-1]);
                    sceneLuaEngine->dispatchEvent(drawHUDEvent);
                }
            }
            //! ------------------------------------------------------------
        }

        //Clear garbage (be careful!)
        //luaEngine.runGarbageCollector();
    }

    process_InterprocessCommands();

}

void LevelScene::processEvents()
{
    #ifndef __APPLE__
    if(g_AppSettings.interprocessing)
        qApp->processEvents();
    #endif
    //SDL_PumpEvents();
    Scene::processEvents();
    player1Controller->update();
    player2Controller->update();
}

void LevelScene::render()
{
    GlRenderer::clearScreen();

    int c=0;
    if(!isInit) goto renderBlack;

    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
    for(QList<PGE_LevelCamera>::iterator it=cameras.begin();it!=cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);

        if(numberOfPlayers>1)
            GlRenderer::setViewport(cam->renderX(), cam->renderY(), cam->w(), cam->h());

        cam->drawBackground();

        int render_sz = cam->renderObjects_count();
        PGE_Phys_Object** render_obj = cam->renderObjects_arr();
        for(int i=0; i<render_sz; i++)
        {
            switch(render_obj[i]->type)
            {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLNPC:
            case PGE_Phys_Object::LVLPlayer:
                render_obj[i]->render(cam->posX(), cam->posY());
                break;
            default:
                break;
            }
        }

        for(SceneEffectsArray::iterator it=WorkingEffects.begin();it!=WorkingEffects.end(); it++ )
        {
             Scene_Effect &item=(*it);
             item.render(cam->posX(), cam->posY());
        }

        if(PGE_Window::showPhysicsDebug)
        {
            for(int i=0; i<render_sz; i++)
            {
                render_obj[i]->renderDebug(cam->posX(), cam->posY());
                if(render_obj[i]->type==PGE_Phys_Object::LVLNPC)
                {
                    LVL_Npc *npc=static_cast<LVL_Npc*>(render_obj[i]);
                    for(int i=0; i<npc->detectors_inarea.size(); i++)
                    {
                        PGE_RectF trapZone = npc->detectors_inarea[i].trapZone();
                        GlRenderer::renderRect(trapZone.x()-cam->posX(), trapZone.y()-cam->posY(),
                                                 trapZone.width(), trapZone.height(),
                                                 1.0f, 0.0, 0.0f, 1.0f, false);
                    }
                }
            }
        }

        cam->drawForeground();

        if(numberOfPlayers>1)
            GlRenderer::resetViewport();
        c++;
    }

    //Draw camera separators
    for(c=1; c<cameras.size(); c++)
    {
        GlRenderer::renderRect(0, cameras[c].h()*c-1, cameras[c].w(), 2, 0.f, 0.f, 0.f, 1.f);
    }


    if(PGE_Window::showDebugInfo)
    {
        //FontManager::printText(QString("Camera X=%1 Y=%2").arg(cam_x).arg(cam_y), 200,10);

        int dpos=60;
        FontManager::printText(QString("Player J=%1 G=%2 F=%3; TICK-SUB: %4\n"
                                       "NPC's: %5, Active %6; BLOCKS: %7")
                               .arg(debug_player_jumping)
                               .arg(debug_player_onground)
                               .arg(debug_player_foots)
                               .arg(uTickf)
                               .arg(npcs.size())
                               .arg(active_npcs.size())
                               .arg(blocks.size()), 10, dpos);
        dpos+=35;
        FontManager::printText(QString("Vizible objects: %1")
                               .arg(!cameras.isEmpty() ? cameras[0].renderObjects_count() : 0 ), 10, dpos);

        dpos+=35;

        FontManager::printText(QString("Delays E=%1 R=%2 P=%3")
                               .arg(debug_event_delay, 3, 10, QChar('0'))
                               .arg(debug_render_delay, 3, 10, QChar('0'))
                               .arg(debug_phys_delay, 3, 10, QChar('0')), 10,dpos);
        dpos+=35;

        FontManager::printText(QString("Time Real:%1\nTime Loop:%2")
                               .arg(debug_TimeReal.elapsed(), 10, 10, QChar('0'))
                               .arg(debug_TimeCounted, 10, 10, QChar('0')), 10,dpos);
        dpos+=35;

        if(!isLevelContinues)
        {
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitLevelDelay)
                                   .arg(uTickf), 10, dpos, 0, 1.0, 0, 0, 1.0);
            dpos+=35;
        }

        if(placingMode)
            FontManager::printText(QString("Placing! %1 X=%2 Y=%3")
                        .arg(placingMode_item_type)
                        .arg(placingMode_renderAt.x())
                        .arg(placingMode_renderAt.y()), 10, 10, 0);
        else
            FontManager::printText(QString("%1")
                        .arg(PGE_MusPlayer::MUS_Title()), 10, 10, 0);
    }
    renderBlack:
    Scene::render();
    if(placingMode) drawPlacingItem();

    if(IsLoaderWorks) drawLoader();

    if(isPauseMenu) _pauseMenu.render();
}

bool slowTimeMode=false;
bool OneStepMode = false;
bool OneStepMode_doStep = false;

void LevelScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(doExit || isExit()) return;

    if(isPauseMenu) _pauseMenu.processKeyEvent(sdl_key);

    switch(sdl_key)
    { // Check which
      case SDLK_ESCAPE:
      case SDLK_RETURN:     // Toggle pause mode
          {
              if(doExit || isPauseMenu) break;
              isPauseMenu = true;
          }
      break;
      case SDLK_BACKQUOTE:
      {
          PGE_Debugger::executeCommand(this);
          break;
      }
      case SDLK_1:
      {
        if(!players.isEmpty())
        launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 0, 0, 0);
      }
      break;
      case SDLK_2:
      {
         if(!players.isEmpty())
          launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 2000, 3, -6, 12);
      }
      break;
      case SDLK_3:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.decelerate_x=0.02;
            p.max_vel_y=12;
            launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -3, -6, 5, 0, p);
            launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, -4, -7, 5, 0, p);
            launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 3, -6, 5, 0, p);
            launchEffect(1, players.first()->posX(), players.first()->posY(), 0, 5000, 4, -7, 5, 0, p);
         }
      }
      break;
      case SDLK_4:
      {
         if(!players.isEmpty())
         {
            Scene_Effect_Phys p;
            p.max_vel_y=12;
            launchEffect(11, players.first()->posX(), players.first()->posY(), 0, 5000, 0, -3, 12, 0, p);
         }
      }
      break;
      case SDLK_5:
      {
         if(!players.isEmpty())
         {
            launchEffect(10, players.first()->posX(), players.first()->posY(), 1, 0, 0, 0, 0);
         }
      }
      break;

        case SDLK_7:
        {
            if(players.size()>=1)
                players[0]->setCharacterSafe(players[0]->characterID-1, players[0]->stateID);
        }
        break;
        case SDLK_8:
        {
            if(players.size()>=1)
                players[0]->setCharacterSafe(players[0]->characterID+1, players[0]->stateID);
        }
        break;
        case SDLK_9:
        {
           if(players.size()>=2)
            players[1]->setCharacterSafe(2, 1);
           if(players.size()>=1)
               players[0]->setCharacterSafe(players[0]->characterID, players[0]->stateID-1);
        }
        break;
        case SDLK_0:
        {
           if(players.size()>=2)
            players[1]->setCharacterSafe(2, 2);
           else if(players.size()>=1)
            players[0]->setCharacterSafe(players[0]->characterID, players[0]->stateID+1);
        }
        break;

        case SDLK_F5:
        {
          PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
          isTimeStopped=!isTimeStopped;
        }
        break;
        case SDLK_F6:
          {
            PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
            slowTimeMode=!slowTimeMode;
          }
        break;
        case SDLK_F7:
          {
            PGE_Audio::playSoundByRole(obj_sound_role::WorldOpenPath);
            OneStepMode = !OneStepMode;
          }
        case SDLK_F8:
          if(OneStepMode)
          {
            PGE_Audio::playSoundByRole(obj_sound_role::WorldMove);
            OneStepMode_doStep = true;
          }
        break;
      default:
        break;
    }
}

LuaEngine *LevelScene::getLuaEngine()
{
    return &luaEngine;
}

int LevelScene::exec()
{
    isLevelContinues=true;
    doExit=false;
    running=true;

    LoopTiming times;
    times.start_common = SDL_GetTicks();

    //Set black color clearer
    GlRenderer::setClearColor(0.f, 0.f, 0.f, 1.0f);

    /****************Initial update***********************/
    //Apply musics and backgrounds
    for(int i=0; i<cameras.size(); i++)
    {
        //Play music from first camera only
        if(i==0) cameras[i].cur_section->playMusic();
        cameras[i].cur_section->initBG();
    }

    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    player1Controller->resetControls();
    player2Controller->resetControls();
    if(running) update();

    debug_TimeCounted=0;
    debug_TimeReal.restart();
    /*****************************************************/

    while(running)
    {
        times.start_common = SDL_GetTicks();

        debug_TimeCounted += uTickf;

        if(PGE_Window::showDebugInfo) times.start_events = SDL_GetTicks();
        /**********************Update common events and controllers******************/
        processEvents();
        /****************************************************************************/
        if(PGE_Window::showDebugInfo) times.stop_events = SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_event_delay = (times.stop_events-times.start_events);

        times.start_physics=SDL_GetTicks();
        /**********************Update physics and game progess***********************/
        if(!OneStepMode || OneStepMode_doStep)
        {
            update();
            OneStepMode_doStep = false;
        }
        /****************************************************************************/
        times.stop_physics=SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_phys_delay  = (times.stop_physics-times.start_physics);

        times.stop_render=0;
        times.start_render=0;
        /**********************Process rendering of stuff****************************/
        if((PGE_Window::vsync)||(times.doUpdate_render<=0.f))
        {
            times.start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            GlRenderer::flush();
            GlRenderer::repaint();
            times.stop_render=SDL_GetTicks();
            times.doUpdate_render = frameSkip? uTickf+(times.stop_render-times.start_render) : 0;
            if(PGE_Window::showDebugInfo) debug_render_delay = times.stop_render-times.start_render;
        }
        times.doUpdate_render -= uTickf;
        if(times.stop_render < times.start_render) { times.stop_render=0; times.start_render=0; }
        /****************************************************************************/

        if( (!PGE_Window::vsync) && (uTick > (signed)times.passedCommonTime()) )
        {
            if(!slowTimeMode)
                SDL_Delay( uTick-times.passedCommonTime() );
            else
                SDL_Delay( uTick - times.passedCommonTime() + 300 );
        } else if(slowTimeMode) SDL_Delay( uTick - times.passedCommonTime() + 300 );
    }
    return exitLevelCode;
}



QString LevelScene::getLastError()
{
    return errorMsg;
}



bool LevelScene::isExit()
{
    return !isLevelContinues;
}

void LevelScene::setExiting(int delay, int reason)
{
    exitLevelDelay   = delay;
    exitLevelCode    = reason;
    isLevelContinues = false;
}

QString LevelScene::toAnotherLevel()
{
    if(!warpToLevelFile.isEmpty())
    if(!warpToLevelFile.endsWith(".lvl", Qt::CaseInsensitive) &&
       !warpToLevelFile.endsWith(".lvlx", Qt::CaseInsensitive))
        warpToLevelFile.append(".lvl");

    return warpToLevelFile;
}

int LevelScene::toAnotherEntrance()
{
    return warpToArrayID;
}

PGE_Point LevelScene::toWorldXY()
{
    return warpToWorldXY;
}

int LevelScene::exitType()
{
    return exitLevelCode;
}



void LevelScene::setGameState(EpisodeState *_gameState)
{
    gameState = _gameState;
    numberOfPlayers = 1;
    if(gameState)
    {
        numberOfPlayers = gameState->numOfPlayers;
        if(gameState->isEpisode && !gameState->isHubLevel)
            initPauseMenu2();
        else
            initPauseMenu1();
    }
}

LevelScene::LVL_NpcsArray &LevelScene::getNpcs()
{
    return npcs;
}

LevelScene::LVL_NpcsArray &LevelScene::getActiveNpcs()
{
    return active_npcs;
}

LevelScene::LVL_BlocksArray &LevelScene::getBlocks()
{
    return blocks;
}

LevelScene::LVL_BgosArray &LevelScene::getBGOs()
{
    return bgos;
}

LevelScene::LVL_PlayersArray& LevelScene::getPlayers()
{
    return players;
}


