#include "luaclass_level_playerstate.h"

#include "luaclass_level_lvl_player.h"
#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{
    isLuaPlayer = true;
}

Binding_Level_ClassWrapper_LVL_Player::~Binding_Level_ClassWrapper_LVL_Player()
{}

void Binding_Level_ClassWrapper_LVL_Player::lua_onInit()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onInit");
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop(float tickTime)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onLoop", tickTime);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onHarm(LVL_Player_harm_event *harmevent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onHarm", harmevent);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onTransform(long character, long state)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTransform", character, state);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onTakeNpc(LVL_Npc *npc)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTakeNpc", npc);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onKillNpc(LVL_Npc *npc)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKillNpc", npc);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onKeyPressed(ControllableObject::KeyType ktype)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKeyPressed", ktype);
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onKeyReleased(ControllableObject::KeyType ktype)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKeyReleased", ktype);
}


luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player, PGE_Phys_Object, detail::null_type, Binding_Level_ClassWrapper_LVL_Player>("BasePlayer")
            .def(constructor<>())
            .def("onInit", &LVL_Player::lua_onInit, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onInit)
            .def("onLoop", &LVL_Player::lua_onLoop, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onLoop)
            .def("onHarm", &LVL_Player::lua_onHarm, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onHarm)
            .def("onTransform", &LVL_Player::lua_onTransform, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onTransform)
            .def("onTakeNpc", &LVL_Player::lua_onTakeNpc, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onTakeNpc)
            .def("onKillNpc", &LVL_Player::lua_onKillNpc, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKillNpc)
            .def("onKeyPressed", &LVL_Player::lua_onKeyPressed, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKeyPressed)
            .def("onKeyReleased", &LVL_Player::lua_onKeyReleased, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKeyReleased)

            .def("spawnNPC", &LVL_Player::lua_spawnNPC)

            .def("getKeyState",  &LVL_Player::lua_getKeyState)
            .def("setAnimation", &LVL_Player::lua_setAnimation)
            .def("playAnimationOnce", &LVL_Player::lua_playAnimationOnce)
            .property("globalState",  &LVL_Player::global_state)

            .property("health", &LVL_Player::getHealth, &LVL_Player::setHealth)

            .def("bump", &LVL_Player::bump)

            .def("setState", &LVL_Player::setState)
            .def("setCharacter", &LVL_Player::setCharacterID)
            .def("setInvincible", &LVL_Player::setInvincible)
            .def_readonly("characterID", &LVL_Player::characterID)
            .def_readonly("stateID", &LVL_Player::stateID)
            .property("direction", &LVL_Player::direction)

            .def_readonly("onGround", &LVL_Player::onGround)
            .def_readonly("isDucking", &LVL_Player::ducking)
            ;

}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::HarmEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player_harm_event>("BasePlayerHarmEvent")
            .def(constructor<>())
            .def_readwrite("do_harm", &LVL_Player_harm_event::doHarm)
            .def_readwrite("damage", &LVL_Player_harm_event::doHarm_damage);
}
