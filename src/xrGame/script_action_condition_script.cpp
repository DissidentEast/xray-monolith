////////////////////////////////////////////////////////////////////////////
//	Module 		: script_action_condition_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script action condition class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_action_condition.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptActionCondition::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptActionCondition>("cond")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(u32)>()
			.addConstructor<void(*)(u32, double)>()
		.endClass();

	lua_getglobal(L, "cond");
	lua_createtable(L, 0, 7);
	lua_pushinteger(L, int(CScriptActionCondition::MOVEMENT_FLAG));	lua_setfield(L, -2, "move_end");
	lua_pushinteger(L, int(CScriptActionCondition::WATCH_FLAG));	lua_setfield(L, -2, "look_end");
	lua_pushinteger(L, int(CScriptActionCondition::ANIMATION_FLAG));	lua_setfield(L, -2, "anim_end");
	lua_pushinteger(L, int(CScriptActionCondition::SOUND_FLAG));	lua_setfield(L, -2, "sound_end");
	lua_pushinteger(L, int(CScriptActionCondition::OBJECT_FLAG));	lua_setfield(L, -2, "object_end");
	lua_pushinteger(L, int(CScriptActionCondition::TIME_FLAG));	lua_setfield(L, -2, "time_end");
	lua_pushinteger(L, int(CScriptActionCondition::ACT_FLAG));	lua_setfield(L, -2, "act_end");
	lua_setfield(L, -2, "cond");
	lua_pop(L, 1);
}
