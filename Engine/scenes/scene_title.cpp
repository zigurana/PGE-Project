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

#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <common_features/graphics_funcs.h>
#include <settings/global_settings.h>
#include <data_configs/config_manager.h>
#include <gui/pge_msgbox.h>

#include <fontman/font_manager.h>
#include <controls/controller_joystick.h>

#include <script/bindings/core/events/luaevents_core_engine.h>

#include "scene_title.h"
#include <QtDebug>

SDL_Thread *                     TitleScene::filefind_thread=NULL;
QString                          TitleScene::filefind_folder="";
QList<QPair<QString, QString > > TitleScene::filefind_found_files;
std::atomic_bool                 TitleScene::filefind_finished(false);

TitleScene::TitleScene() : Scene(Title), luaEngine(this)
{
    doExit=false;
    mousePos.setX(-300);
    mousePos.setY(-300);
    _cursorIsLoaded=false;

    ret=0;

    numOfPlayers=1;

    controller = NULL;

    bgcolor.r = float(ConfigManager::setup_TitleScreen.backgroundColor.red())/255.0f;
    bgcolor.g = float(ConfigManager::setup_TitleScreen.backgroundColor.green())/255.0f;
    bgcolor.b = float(ConfigManager::setup_TitleScreen.backgroundColor.blue())/255.0f;

    if(ConfigManager::setup_cursors.normal.isEmpty())
    {
        _cursorIsLoaded=false;
    }
    else
    {
        GlRenderer::loadTextureP(cursor, ConfigManager::setup_cursors.normal);
        _cursorIsLoaded=true;
    }

    if(!ConfigManager::setup_TitleScreen.backgroundImg.isEmpty())
    {
        GlRenderer::loadTextureP(background, ConfigManager::setup_TitleScreen.backgroundImg);
        _bgIsLoaded=true;
    }
    else
        _bgIsLoaded=false;

    imgs.clear();

    for(int i=0; i<ConfigManager::setup_TitleScreen.AdditionalImages.size(); i++)
    {
        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].imgFile.isEmpty()) continue;

        TitleScene_misc_img img;
        GlRenderer::loadTextureP(img.t, ConfigManager::setup_TitleScreen.AdditionalImages[i].imgFile);

        //Using of X-Y as offsets if aligning is enabled
        int x_offset=ConfigManager::setup_TitleScreen.AdditionalImages[i].x;
        int y_offset=ConfigManager::setup_TitleScreen.AdditionalImages[i].y;

        switch(ConfigManager::setup_TitleScreen.AdditionalImages[i].align_to)
        {
        case TitleScreenAdditionalImage::LEFT_ALIGN:
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        case TitleScreenAdditionalImage::TOP_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            break;
        case TitleScreenAdditionalImage::RIGHT_ALIGN:
            img.x = PGE_Window::Width-img.t.w - x_offset;
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        case TitleScreenAdditionalImage::BOTTOM_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            img.y = PGE_Window::Height-img.t.h - y_offset;
            break;
        case TitleScreenAdditionalImage::CENTER_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        default:
            img.x = ConfigManager::setup_TitleScreen.AdditionalImages[i].x;
            img.y = ConfigManager::setup_TitleScreen.AdditionalImages[i].y;
            break;
        }

        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].center_x)
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;

        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].center_y)
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;

        img.a.construct(ConfigManager::setup_TitleScreen.AdditionalImages[i].animated,
                        ConfigManager::setup_TitleScreen.AdditionalImages[i].frames,
                        ConfigManager::setup_TitleScreen.AdditionalImages[i].framespeed);

        img.frmH = (img.t.h / ConfigManager::setup_TitleScreen.AdditionalImages[i].frames);

        imgs.push_back(img);
    }

    debug_joy_keyval=0;
    debug_joy_keyid=0;
    debug_joy_keytype=0;

    filefind_thread =NULL;
}

TitleScene::~TitleScene()
{
    //Clear screen
    GlRenderer::clearScreen();

    if(_cursorIsLoaded)
    {
        GlRenderer::deleteTexture(  cursor  );
    }

    GlRenderer::deleteTexture( background );

    for(int i=0;i<imgs.size();i++)
    {
        GlRenderer::deleteTexture( imgs[i].t );
    }
    imgs.clear();

    if(controller)
        delete controller;
}

bool TitleScene::init()
{
    controller = g_AppSettings.openController(1);

    if(!ConfigManager::music_lastIniFile.isEmpty())
    {
        ConfigManager::music_lastIniFile.clear();
        ConfigManager::loadDefaultMusics();
    }
    if(!ConfigManager::sound_lastIniFile.isEmpty())
    {
        ConfigManager::sound_lastIniFile.clear();
        ConfigManager::loadDefaultSounds();
        ConfigManager::buildSoundIndex();
    }

    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    luaEngine.setCoreFile(":/script/maincore_title.lua");
    luaEngine.setUserFile(ConfigManager::setup_TitleScreen.luaFile);
    luaEngine.setErrorReporterFunc([this](const QString& errorMessage, const QString& stacktrace){
        qWarning() << "Lua-Error: ";
        qWarning() << "Error Message: " << errorMessage;
        qWarning() << "Stacktrace: \n" << stacktrace;
        PGE_MsgBox msgBox(this, QString("A lua error has been thrown: \n") + errorMessage + "\n\nMore details in the log!", PGE_MsgBox::msg_error);
        msgBox.exec();
    });
    qDebug() << "Attempt to init...";
    luaEngine.init();
    qDebug() << "done!";
    return true;
}

void TitleScene::onKeyboardPressed(SDL_Scancode scancode)
{
    if(doExit) return;
    if(menu.isKeyGrabbing())
    {
        if(scancode!=SDL_SCANCODE_ESCAPE)
            menu.storeKey(scancode);
        else
            menu.storeKey(PGE_KEYGRAB_REMOVE_KEY);
        //If key was grabbed, reset controlls
        if(!menu.isKeyGrabbing()) resetController();
    /**************Control men via controllers*************/
    }
}

void TitleScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(doExit) return;
    if(menu.isKeyGrabbing()) return;

    if(controller->keys.up) {
        menu.selectUp();
    } else if(controller->keys.down) {
        menu.selectDown();
    } else if(controller->keys.left) {
        menu.selectLeft();
    } else if(controller->keys.right) {
        menu.selectRight();
    } else if(controller->keys.jump) {
        menu.acceptItem();
    } else if(controller->keys.alt_jump) {
        menu.acceptItem();
    } else if(controller->keys.run) {
        menu.rejectItem();
    } else
    switch(sdl_key)
    {
      case SDLK_UP:
        menu.selectUp();
      break;
      case SDLK_DOWN:
        menu.selectDown();
      break;
      case SDLK_LEFT:
        menu.selectLeft();
      break;
      case SDLK_RIGHT:
        menu.selectRight();
      break;
      case SDLK_RETURN:
        menu.acceptItem();
      break;
      case SDLK_ESCAPE:
        menu.rejectItem();
      break;
      default:
        break;
    }
}

void TitleScene::onMouseMoved(SDL_MouseMotionEvent &mmevent)
{
    mousePos = GlRenderer::MapToScr(mmevent.x, mmevent.y);
    if(!menu.isKeyGrabbing() && !doExit)
        menu.setMouseHoverPos(mousePos.x(), mousePos.y());
}

void TitleScene::onMousePressed(SDL_MouseButtonEvent &mbevent)
{
    if(doExit) return;

    if(menu.isKeyGrabbing())
        menu.storeKey(PGE_KEYGRAB_CANCEL); //Calcel Keygrabbing
    else
    switch(mbevent.button)
    {
        case SDL_BUTTON_LEFT:
            mousePos = GlRenderer::MapToScr(mbevent.x, mbevent.y);
            menu.setMouseClickPos(mousePos.x(), mousePos.y());
        break;
        case SDL_BUTTON_RIGHT:
            menu.rejectItem();
        break;
        default:
        break;
    }
}

void TitleScene::onMouseWheel(SDL_MouseWheelEvent &wheelevent)
{
    if(!menu.isKeyGrabbing() && !doExit)
    {
        if(wheelevent.y>0)
            menu.scrollUp();
        else
            menu.scrollDown();
    }
}

LuaEngine *TitleScene::getLuaEngine()
{
    return &luaEngine;
}

void TitleScene::processEvents()
{
    SDL_PumpEvents();
    if(PGE_Window::showDebugInfo)
    {
        if(g_AppSettings.joysticks.size()>0)
        {
            KM_Key jkey;
            JoystickController::bindJoystickKey(g_AppSettings.joysticks.first(), jkey);
            debug_joy_keyval    =jkey.val,
            debug_joy_keyid     =jkey.id;
            debug_joy_keytype   =jkey.type;
        }
    }

    if(menu.processJoystickBinder())
    {
        //If key was grabbed, reset controlls
        if(!menu.isKeyGrabbing()) resetController();
    }
    controller->update();

    Scene::processEvents();
}

void TitleScene::update()
{
    Scene::update();
    updateLua();
    for(int i=0;i<imgs.size(); i++)
        imgs[i].a.manualTick(uTickf);

    if(doExit)
    {
        if(fader.isNull()) fader.setFade(10, 1.0f, 0.02f);
        if(fader.isFull())
            running=false;
    }
}


void TitleScene::render()
{
    GlRenderer::clearScreen();

    GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, bgcolor.r, bgcolor.g, bgcolor.b, 1.0);

    if(_bgIsLoaded)
    {
        GlRenderer::setTextureColor(1.0f, 1.0f,1.0f,1.0f);
        GlRenderer::renderTexture(&background, PGE_Window::Width/2 - background.w/2,
                                  PGE_Window::Height/2 - background.h/2);
    }

    for(int i=0;i<imgs.size();i++)
    {
        AniPos x(0,1); x = imgs[i].a.image();
        GlRenderer::setTextureColor(1.0f, 1.0f,1.0f,1.0f);
        GlRenderer::renderTexture(&imgs[i].t,
                                  imgs[i].x,
                                  imgs[i].y,
                                  imgs[i].t.w,
                                  imgs[i].frmH, x.first, x.second);
    }

    if(g_AppSettings.showDebugInfo)
    {
        FontManager::printText(QString("Joystick key: val=%1, id=%2, type=%3\nfader ratio %4 N%5 F%6 TKS-%7\nTICK: %8")
                               .arg(debug_joy_keyval)
                               .arg(debug_joy_keyid)
                               .arg(debug_joy_keytype)
                               .arg(fader.fadeRatio())
                               .arg(fader.isNull())
                               .arg(fader.isFull())
                               .arg(fader.ticksLeft())
                               .arg(uTickf)
                               ,10, 10);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
//                               "abcdefghijklmnopqrstuvwxyz\n"
//                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
//                               "абвгдеёжзийклмнопрстуфхцчшщъыьэюя\n"
//                               "Ich bin glücklich!",10, 60, 2, 1.0, 1.0, 1.0, 1.0);
        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
                               "{|}\\¡¢£€¥Š§š©ª«¬®¯°±²³Žµ¶·ž¹º»ŒŸ¿\n"
                               "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×\n"
                               "˘Ł¤ĽŚ§¨ŠŞŤŹŽŻ°ą˛ł´ľśˇ¸šşťź˝žżŔÁÂĂÄ\n"
                               "ĹĆÇČÉĘËĚÍÎĎĐŃŇÓÔŐÖ×ŘŮÚŰÜÝŢßŕáâăäĺć\n"
                               "çčéęëěíîďđńňóôőö÷řůúűüýţ˙\n"
                               "ØÙÚÛÜÝÞß÷ © ®\n\n"
                               "Ich bin glücklich!\n\n"
                               "Как хорошо, что всё работает!\n"
                               "Живіть всі дружно!", 10, 100, 0, 0.75, 1.0, 0.7, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 90, 1, 0, 1.0, 0, 1.0);
//        FontManager::printText("0123456789\n"
//                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
//                               "abcdefghijklmnopqrstuvwxyz\n"
//                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
//                               "абвгдеёжзийклмнопрстуфхцчшщъыьэюя\n"
//                               "Ich bin glücklich!", 10, 130, 2, 1.0, 1.0, 1.0, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 250, 3, 1.0, 1.0, 1.0, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 290, FontManager::DefaultTTF_Font, 1.0, 0.5, 1.0, 1.0);
    }


    menu.render();

    Scene::render();
}

void TitleScene::renderMouse()
{
    int posX=mousePos.x();
    int posY=mousePos.y();

    if(_cursorIsLoaded)
    {
        GlRenderer::setTextureColor(1.0f, 1.0f,1.0f,1.0f);
        GlRenderer::renderTexture(&cursor, posX, posY);
    }
    else
    {
        GlRenderer::renderRect(posX, posY, 10,10, 0.f, 1.f, 0.f, 1.0f);
    }
}



int TitleScene::exec()
{
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    bool frameSkip = g_AppSettings.frameSkip;

    menustates.clear();
    menuChain.clear();

    //Set black color clearer
    GlRenderer::setClearColor(0.f, 0.f, 0.f, 1.0f);

    for(int i=menuFirst; i<menuLast;i++)
        menustates[(CurrentMenu)i] = menustate(0, 0);

    setMenu(menu_main);

    //Hide mouse cursor
    PGE_Window::setCursorVisibly(false);

    while(running)
    {
        //Refresh frameskip flag
        frameSkip = g_AppSettings.frameSkip;

        times.start_common = SDL_GetTicks();

        processEvents();
        processMenu();
        update();        

        times.stop_render=0;
        times.start_render=0;
        /**********************Process rendering of stuff****************************/
        if((PGE_Window::vsync)||(times.doUpdate_render<=0.f))
        {
            times.start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            renderMouse();
            GlRenderer::flush();
            GlRenderer::repaint();
            times.stop_render=SDL_GetTicks();
            times.doUpdate_render = frameSkip ? uTickf+(times.stop_render-times.start_render) : 0;
        }
        times.doUpdate_render -= uTickf;
        if(times.stop_render < times.start_render) { times.stop_render=0; times.start_render=0; }
        /****************************************************************************/

        if( (!PGE_Window::vsync) && ( uTick > (signed)times.passedCommonTime() ) )
        {
            SDL_Delay( uTick - times.passedCommonTime() );
        }
    }
    menu.clear();

    PGE_Window::clean();

    //Show mouse cursor
    PGE_Window::setCursorVisibly(true);
    return ret;
}

void TitleScene::resetController()
{
    if(controller)
        delete controller;
    controller = g_AppSettings.openController(1);
}
