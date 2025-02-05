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

#include "lvl_event_engine.h"
#include "../scene_level.h"
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

LVL_EventAction::LVL_EventAction()
{
    timeDelayLeft=0.0f;
}

LVL_EventAction::LVL_EventAction(const LVL_EventAction &ea)
{
    action = ea.action;
    timeDelayLeft=ea.timeDelayLeft;
    eventName=ea.eventName;
}

LVL_EventAction::~LVL_EventAction()
{}




LVL_EventEngine::LVL_EventEngine()
{}

LVL_EventEngine::~LVL_EventEngine()
{
    events.clear();
}

void LVL_EventEngine::addSMBX64Event(LevelSMBX64Event &evt)
{
    LVL_EventAction evntAct;
        evntAct.eventName=evt.name;
        evntAct.timeDelayLeft=0;

        EventQueueEntry<LVL_EventAction> hideLayers;
        hideLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_hide)
                                   _scene->layers.hide(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(hideLayers);

        EventQueueEntry<LVL_EventAction> showLayers;
        showLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_show)
                                   _scene->layers.show(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(showLayers);

        EventQueueEntry<LVL_EventAction> toggleLayers;
        toggleLayers.makeCaller([this,evt]()->void{
                               foreach(QString ly, evt.layers_toggle)
                                   _scene->layers.toggle(ly, !evt.nosmoke);
                           }, 0);
        evntAct.action.events.push_back(toggleLayers);

        if(evt.sound_id>0)
        {
            EventQueueEntry<LVL_EventAction> playsnd;
            playsnd.makeCaller([this,evt]()->void{
                                       PGE_Audio::playSound(evt.sound_id);
                               }, 0);
            evntAct.action.events.push_back(playsnd);
        }

        for(int i=0;i<evt.sets.size(); i++)
        {
            if(evt.sets[i].background_id!=-1)
            {
                EventQueueEntry<LVL_EventAction> bgToggle;
                if(evt.sets[i].background_id<0)
                {
                    bgToggle.makeCaller([this,evt,i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].resetBG();
                            for(int j=0;j<_scene->cameras.size();j++)
                            {
                                if(_scene->cameras[j].cur_section==&_scene->sections[i])
                                {
                                    _scene->sections[i].initBG();
                                }
                            }
                        }

                    }, 0);
                } else {
                    bgToggle.makeCaller([this,evt,i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].setBG(evt.sets[i].background_id);
                            for(int j=0;j<_scene->cameras.size();j++)
                            {
                                if(_scene->cameras[j].cur_section==&_scene->sections[i])
                                {
                                    _scene->sections[i].initBG();
                                }
                            }
                        }
                    }, 0);
                }
                evntAct.action.events.push_back(bgToggle);
            }
            if(evt.sets[i].music_id!=-1)
            {
                EventQueueEntry<LVL_EventAction> musToggle;
                if(evt.sets[i].music_id<0)
                {
                    musToggle.makeCaller([this,evt,i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].resetMusic();
                            for(int j=0;j<_scene->cameras.size();j++)
                            {
                                if(_scene->cameras[j].cur_section==&_scene->sections[i])
                                {
                                    _scene->sections[i].playMusic();
                                }
                            }
                        }
                    }, 0);
                } else {
                    musToggle.makeCaller([this,evt, i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].setMusic(evt.sets[i].music_id);
                            for(int j=0;j<_scene->cameras.size();j++)
                            {
                                if(_scene->cameras[j].cur_section==&_scene->sections[i])
                                {
                                    _scene->sections[i].playMusic();
                                }
                            }
                        }
                    }, 0);
                }
                evntAct.action.events.push_back(musToggle);
            }
            if(evt.sets[i].position_left!=-1)
            {
                EventQueueEntry<LVL_EventAction> bordersToggle;
                if(evt.sets[i].position_left==-2)
                {
                    bordersToggle.makeCaller([this,evt,i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].resetLimits();
                        }
                    }, 0);
                } else {
                    bordersToggle.makeCaller([this,evt, i]()->void{
                        if(i<_scene->sections.size())
                        {
                            _scene->sections[i].changeLimitBorders(
                                        evt.sets[i].position_left,
                                        evt.sets[i].position_top,
                                        evt.sets[i].position_right,
                                        evt.sets[i].position_bottom);
                        }
                    }, 0);
                }
                evntAct.action.events.push_back(bordersToggle);
            }
        }

        if((evt.scroll_section<_scene->sections.size())&&((evt.move_camera_x!=0.0f)||(evt.move_camera_y!=0.0f)))
        {
            EventQueueEntry<LVL_EventAction> installAutoscroll;
            installAutoscroll.makeCaller([this,evt]()->void{
                                       _scene->sections[evt.scroll_section].isAutoscroll=true;
                                       _scene->sections[evt.scroll_section]._autoscrollVelocityX=evt.move_camera_x;
                                       _scene->sections[evt.scroll_section]._autoscrollVelocityY=evt.move_camera_y;
                                       for(int j=0;j<_scene->cameras.size();j++)
                                       {
                                           if(_scene->cameras[j].cur_section==&_scene->sections[evt.scroll_section])
                                           {
                                               _scene->cameras[j].isAutoscroll=true;
                                               _scene->cameras[j].resetAutoscroll();
                                           }
                                       }
                               }, 0);
            evntAct.action.events.push_back(installAutoscroll);
        }

        if(!evt.msg.isEmpty())
        {
            EventQueueEntry<LVL_EventAction> message;
            message.makeCaller([this,evt]()->void{
                EventQueueEntry<LevelScene > msgBox;
                msgBox.makeCaller(
                            [this,evt]()->void{

                                   PGE_MsgBox box(_scene, evt.msg,
                                   PGE_MsgBox::msg_info, PGE_Point(-1,-1), -1,
                                   ConfigManager::setup_message_box.sprite);
                                   box.exec();
                                }, 0);
                _scene->system_events.events.push_back(msgBox);
                               }, 0);
            evntAct.action.events.push_back(message);
        }

        if(!evt.movelayer.isEmpty())
        {
            EventQueueEntry<LVL_EventAction> movelayer;
            movelayer.makeCaller([this,evt]()->void{
                                       _scene->layers.installLayerMotion(evt.movelayer, evt.layer_speed_x, evt.layer_speed_y);
                               }, 0);
            evntAct.action.events.push_back(movelayer);
        }

    events[evt.name].push_back(evntAct);

    if(!evt.trigger.isEmpty())
    {
        LVL_EventAction trigger;
        trigger.eventName=evt.name;
        trigger.timeDelayLeft=0;
        EventQueueEntry<LVL_EventAction> triggerEvent;
        triggerEvent.makeCaller([this,evt]()->void{
                                _scene->events.triggerEvent(evt.trigger);
                               }, evt.trigger_timer*100);
        trigger.action.events.push_back(triggerEvent);
        events[evt.name].push_back(trigger);
    }

    //Automatically trigger events
    if( (evt.name=="Level - Start") || evt.autostart==LevelSMBX64Event::AUTO_LevelStart )
        workingEvents.push_back(events[evt.name]);
}

void LVL_EventEngine::processTimers(float tickTime)
{
    QHash<QString, bool > triggered;

    for(int i=0; i<workingEvents.size(); i++)
    {
        if(workingEvents[i].isEmpty())
        {
            workingEvents.removeAt(i); i--; continue;
        }

        for(int j=0; j<workingEvents[i].size(); j++)
        {
            if(workingEvents[i][j].timeDelayLeft<=0)
            {
                workingEvents[i][j].action.processEvents(tickTime);
                if(workingEvents[i][j].action.events.isEmpty())
                {
                    if(triggered.contains(workingEvents[i][j].eventName))
                    {
                        workingEvents[i].removeAt(j); j--;
                        break;
                    }
                    triggered[workingEvents[i][j].eventName]=true;
                    workingEvents[i].removeAt(j); j--;
                    continue;
                }
            }
            else
            {
                workingEvents[i][j].timeDelayLeft-=tickTime;
            }
        }
    }
}

void LVL_EventEngine::triggerEvent(QString event)
{
    if(event.isEmpty())
        return;
    if(events.contains(event))
        workingEvents.push_back(events[event]);
}





