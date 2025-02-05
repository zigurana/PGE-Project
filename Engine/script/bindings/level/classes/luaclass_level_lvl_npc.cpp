#include "luaclass_level_lvl_npc.h"
#include "luaclass_level_lvl_player.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_NPC::Binding_Level_ClassWrapper_LVL_NPC(LevelScene *_parent) : LVL_Npc(_parent)
{
    isLuaNPC = true;
}

Binding_Level_ClassWrapper_LVL_NPC::~Binding_Level_ClassWrapper_LVL_NPC()
{}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onActivated()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onActivated");
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onLoop(float tickTime)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onLoop", tickTime);
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onInit()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onInit");
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onKill(KillEvent *killEvent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKill", killEvent);
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onHarm(HarmEvent *harmEvent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onHarm", harmEvent);
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onTransform(long id)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTransform", id);
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc, PGE_Phys_Object, detail::null_type, Binding_Level_ClassWrapper_LVL_NPC>("BaseNPC")
            .enum_("DamageReason")
            [
                value("DAMAGE_NOREASON", LVL_Npc::DamageReason::DAMAGE_NOREASON),
                value("DAMAGE_STOMPED", LVL_Npc::DamageReason::DAMAGE_STOMPED),
                value("DAMAGE_BY_KICK", LVL_Npc::DamageReason::DAMAGE_BY_KICK),
                value("DAMAGE_BY_PLAYER_ATTACK", LVL_Npc::DamageReason::DAMAGE_BY_PLAYER_ATTACK),
                value("DAMAGE_TAKEN", LVL_Npc::DamageReason::DAMAGE_TAKEN),
                value("DAMAGE_LAVABURN", LVL_Npc::DamageReason::DAMAGE_LAVABURN),
                value("DAMAGE_PITFALL", LVL_Npc::DamageReason::DAMAGE_PITFALL),
                value("DAMAGE_CUSTOM_REASON", LVL_Npc::DamageReason::DAMAGE_CUSTOM_REASON)
            ]
            .enum_("WarpingSide")
            [
                value("WARP_TOP", LVL_Npc::WarpingSide::WARP_TOP),
                value("WARP_LEFT", LVL_Npc::WarpingSide::WARP_LEFT),
                value("WARP_BOTTOM", LVL_Npc::WarpingSide::WARP_BOTTOM),
                value("WARP_RIGHT", LVL_Npc::WarpingSide::WARP_RIGHT)
            ]
            .def(constructor<>())
            //Events
            .def("onActivated", &LVL_Npc::lua_onActivated, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onActivated)
            .def("onLoop", &LVL_Npc::lua_onLoop, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onLoop)
            .def("onInit", &LVL_Npc::lua_onInit, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onInit)

            .def("onKill", &LVL_Npc::lua_onKill, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onKill)
            .def("onHarm", &LVL_Npc::lua_onHarm, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onHarm)
            .def("onTransform", &LVL_Npc::lua_onTransform, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onTransform)

            .def("transformTo", &LVL_Npc::transformTo)
            .def_readonly("transformedFromBlockID", &LVL_Npc::transformedFromBlockID)
            .def_readonly("transformedFromNpcID", &LVL_Npc::transformedFromNpcID)

            //Functions
            .def("setSequenceLeft", &LVL_Npc::lua_setSequenceLeft)
            .def("setSequenceRight", &LVL_Npc::lua_setSequenceRight)
            .def("setSequence", &LVL_Npc::lua_setSequence)
            .def("setOnceAnimationMode", &LVL_Npc::lua_setOnceAnimation)
            .def("setGfxOffset", &LVL_Npc::lua_setGfxOffset)
            .def("setGfxOffsetX", &LVL_Npc::lua_setGfxOffsetX)
            .def("setGfxOffsetY", &LVL_Npc::lua_setGfxOffsetY)

            .def("installInAreaDetector", &LVL_Npc::lua_installInAreaDetector)
            .def("installPlayerPosDetector", &LVL_Npc::lua_installPlayerPosDetector)
            .def("installContactDetector", &LVL_Npc::lua_installContactDetector)
            .def("spawnNPC", &LVL_Npc::lua_spawnNPC)

            .def("setSpriteWarp", &LVL_Npc::setSpriteWarp)
            .def("resetSpriteWarp", &LVL_Npc::resetSpriteWarp)

            .def("activateNeighbours", &LVL_Npc::lua_activate_neighbours)

            .def("unregister", &LVL_Npc::unregister)         // Destroy NPC with no effects and no events
            .def("kill", &LVL_Npc::kill)
            .def("harm", &LVL_Npc::harm)

            //Properties
            .property("id", &LVL_Npc::getID)
            .property("direction", &LVL_Npc::direction, &LVL_Npc::setDirection)
            .property("not_movable", &LVL_Npc::not_movable, &LVL_Npc::setNot_movable)
            .property("contents", &LVL_Npc::contents, &LVL_Npc::setContents)
            .property("special1", &LVL_Npc::special1, &LVL_Npc::setSpecial1)
            .property("special2", &LVL_Npc::special2, &LVL_Npc::setSpecial2)
            .property("isBoss", &LVL_Npc::isBoss)

            .property("collideWithBlocks", &LVL_Npc::getCollideWithBlocks, &LVL_Npc::setCollideWithBlocks)
            .def_readwrite("collideWithPlayers", &LVL_Npc::enablePlayerCollision)

            .property("health", &LVL_Npc::getHealth, &LVL_Npc::setHealth)

            .property("frameDelay", &LVL_Npc::lua_frameDelay, &LVL_Npc::lua_setFrameDelay)

            //Parameters
            .def_readonly("killableByFire", &LVL_Npc::getKillByFire)
            .def_readonly("killableByIce", &LVL_Npc::getKillByIce)
            .def_readonly("killableByHammer", &LVL_Npc::getKillByHammer)
            .def_readonly("killableByForcejump", &LVL_Npc::getKillByForcejump)
            .def_readonly("killableByStatue", &LVL_Npc::getKillByStatue)
            .def_readonly("killableByVehicle", &LVL_Npc::getKillByVehicle)

            //States
            .def_readonly("onGround", &LVL_Npc::onGround)
            .def_readwrite("motionSpeed", &LVL_Npc::motionSpeed)
            .def_readonly("animationIsFinished", &LVL_Npc::lua_animationIsFinished)
            .def_readonly("onCliff", &LVL_Npc::onCliff);

}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::HarmEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc::HarmEvent>("NpcHarmEvent")
            .enum_("killedBy")
            [
                value("self", LVL_Npc::HarmEvent::killedBy::self),
                value("player", LVL_Npc::HarmEvent::killedBy::player),
                value("otherNPC", LVL_Npc::HarmEvent::killedBy::otherNPC)
            ]
            .def(constructor<>())
            .def_readwrite("cancel", &LVL_Npc::HarmEvent::cancel)
            .def_readwrite("damage", &LVL_Npc::HarmEvent::damage)
            .def_readwrite("reason_code", &LVL_Npc::HarmEvent::reason_code)
            .def_readwrite("killed_by", &LVL_Npc::HarmEvent::killed_by)
            .def_readwrite("killer_p", &LVL_Npc::HarmEvent::killer_p)
            .def_readwrite("killer_n", &LVL_Npc::HarmEvent::killer_n);
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::KillEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc::KillEvent>("NpcKillEvent")
            .enum_("killedBy")
            [
                value("self", LVL_Npc::KillEvent::killedBy::self),
                value("player", LVL_Npc::KillEvent::killedBy::player),
                value("otherNPC", LVL_Npc::KillEvent::killedBy::otherNPC)
            ]
            .def(constructor<>())
            .def_readwrite("cancel", &LVL_Npc::KillEvent::cancel)
            .def_readwrite("reason_code", &LVL_Npc::KillEvent::reason_code)
            .def_readwrite("killed_by", &LVL_Npc::KillEvent::killed_by)
            .def_readwrite("killer_p", &LVL_Npc::KillEvent::killer_p)
            .def_readwrite("killer_n", &LVL_Npc::KillEvent::killer_n);
}

