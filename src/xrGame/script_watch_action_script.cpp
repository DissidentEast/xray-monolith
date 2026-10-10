////////////////////////////////////////////////////////////////////////////
//	Module 		: script_watch_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script watch action class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_watch_action.h"
#include "script_game_object.h"
#include "sight_manager_space.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptWatchAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptWatchAction>("look")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(SightManager::ESightType)>()
			.addConstructor<void(*)(SightManager::ESightType, Fvector&)>()
			.addConstructor<void(*)(SightManager::ESightType, CScriptGameObject*)>()
			.addConstructor<void(*)(SightManager::ESightType, CScriptGameObject*, LPCSTR)>()

			// searchlight
			.addConstructor<void(*)(const Fvector&, float, float)>()
			.addConstructor<void(*)(CScriptGameObject*, float, float)>()

			.addFunction("object", &CScriptWatchAction::SetWatchObject) // time
			.addFunction("direct", &CScriptWatchAction::SetWatchDirection) // time
			.addFunction("type", &CScriptWatchAction::SetWatchType)
			.addFunction("bone", &CScriptWatchAction::SetWatchBone)
			.addFunction("completed", (bool (CScriptWatchAction::*)())(&CScriptWatchAction::completed))
		.endClass();

	lua_getglobal(L, "look");
	lua_createtable(L, 0, 7);
	lua_pushinteger(L, int(SightManager::eSightTypePathDirection));	lua_setfield(L, -2, "path_dir");
	lua_pushinteger(L, int(SightManager::eSightTypeSearch));	lua_setfield(L, -2, "search");
	lua_pushinteger(L, int(SightManager::eSightTypeCover));	lua_setfield(L, -2, "danger");
	lua_pushinteger(L, int(SightManager::eSightTypePosition));	lua_setfield(L, -2, "point");
	lua_pushinteger(L, int(SightManager::eSightTypeFirePosition));	lua_setfield(L, -2, "fire_point");
	lua_pushinteger(L, int(SightManager::eSightTypeCurrentDirection));	lua_setfield(L, -2, "cur_dir");
	lua_pushinteger(L, int(SightManager::eSightTypeDirection));	lua_setfield(L, -2, "direction");
	lua_setfield(L, -2, "look");
	lua_pop(L, 1);
}
