#include <scenes/scene_level.h>
#include "luafuncs_level.h"

void Binding_Level_CommonFuncs::Lua_ToggleSwitch(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->toggleSwitch(switchID);
}

bool Binding_Level_CommonFuncs::Lua_getSwitchState(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    return scene->lua_switchState(switchID);
}

void Binding_Level_CommonFuncs::Lua_triggerEvent(lua_State *L, std::string eventName)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->events.triggerEvent( QString::fromStdString(eventName) );
}

void Binding_Level_CommonFuncs::Lua_ShakeScreen(lua_State *L, double forceX, double forceY, double decX, double decY)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(QList<PGE_LevelCamera>::iterator it = scene->cameras.begin(); it != scene->cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreen(forceX, forceY, decX, decY);
    }
}

void Binding_Level_CommonFuncs::Lua_ShakeScreenX(lua_State *L, double forceX, double decX)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(QList<PGE_LevelCamera>::iterator it = scene->cameras.begin(); it != scene->cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreenX(forceX, decX);
    }
}

void Binding_Level_CommonFuncs::Lua_ShakeScreenY(lua_State *L, double forceY, double decY)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(QList<PGE_LevelCamera>::iterator it = scene->cameras.begin(); it != scene->cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreenY(forceY, decY);
    }
}

luabind::scope Binding_Level_CommonFuncs::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Level")[
            def("toggleSwitch", &Binding_Level_CommonFuncs::Lua_ToggleSwitch),
            def("getSwitchState", &Binding_Level_CommonFuncs::Lua_getSwitchState),
            def("triggerEvent", &Binding_Level_CommonFuncs::Lua_triggerEvent),

            def("shakeScreen", &Binding_Level_CommonFuncs::Lua_ShakeScreen),
            def("shakeScreenX", &Binding_Level_CommonFuncs::Lua_ShakeScreenX),
            def("shakeScreenY", &Binding_Level_CommonFuncs::Lua_ShakeScreenY)
        ];
}
