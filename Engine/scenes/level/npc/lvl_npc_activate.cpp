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

#include "../lvl_npc.h"
#include "../../scene_level.h"

void LVL_Npc::Activate()
{
    if(is_activity)
        activationTimeout = setup->deactivetionDelay;
    else
        activationTimeout = 150;

    if(isActivated) return;
    setPaused(false);

    deActivatable = ((setup->deactivation)||(!setup->activity)); //!< Allow deactivation of this NPC when it going offscreen
    wasDeactivated=false;

    animator.start();
    isActivated=true;

    if(!data.event_activate.isEmpty())
        _scene->events.triggerEvent(data.event_activate);

    if(isLuaNPC){
        try{
            lua_onActivated();
        } catch (luabind::error& e) {
            _scene->getLuaEngine()->postLateShutdownError(e);
        }
    }
    lua_activate_neighbours();//Also activate neighours
}

void LVL_Npc::deActivate()
{
    if(!wasDeactivated)
    {
        wasDeactivated=true;
    }
    if(!isActivated) return;

    isActivated=false;
    if(!is_shared_animation)
        animator.stop();

    if(!keep_position_on_despawn)
    {
        if(!reSpawnable)
        {
            unregister();
        } else {
            if((signed)data.id!=_npc_id)
            {
                transformTo_x(data.id); //Transform NPC back into initial form
            }
            setDefaults();
            setPos(data.x, data.y);
            setDirection(data.direct);
            setPaused(true);
        }
    }
}

