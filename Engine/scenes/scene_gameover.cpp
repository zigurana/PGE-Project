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

#include "scene_gameover.h"

#include "../gui/pge_questionbox.h"
#include <data_configs/config_manager.h>
#include <gui/pge_menubox.h>
#include <settings/global_settings.h>

#include <QApplication>

GameOverScene::GameOverScene(): Scene(GameOver)
{
    player1Controller = g_AppSettings.openController(1);
}

GameOverScene::~GameOverScene()
{
    delete player1Controller;
}

void GameOverScene::update()
{
    Scene::update();
}

void GameOverScene::render()
{
    GlRenderer::clearScreen();
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int GameOverScene::exec()
{
    PGE_QuestionBox continueOrQuit(this,
                                   //% "Game Over!"
                                   qtTrId("GAMEOVER_TITLE"),
                                   PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                   ConfigManager::setup_menu_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
    QStringList items;
    //% "Continue"
    items << qtTrId("GAMEOVER_CONTINUE");
    //% "Quit"
    items << qtTrId("GAMEOVER_EXIT");
    continueOrQuit.addMenuItems(items);
    continueOrQuit.setRejectSnd(obj_sound_role::BlockSmashed);
    continueOrQuit.exec();

    if (continueOrQuit.answer() >= 0)
        if (continueOrQuit.answer() == 0)
            return GameOverSceneResult::CONTINUE;

    return GameOverSceneResult::QUIT;
}

void GameOverScene::processEvents()
{
    Scene::processEvents();
}
