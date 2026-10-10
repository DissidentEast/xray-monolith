////////////////////////////////////////////////////////////////////////////
//	Module 		: script_monster_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script monster action class
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_monster_action.h"
#include "script_game_object.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptMonsterAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptMonsterAction>("act")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterGlobalAction)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterGlobalAction, CScriptGameObject*)>()
		.endClass();

	lua_getglobal(L, "act");
	lua_createtable(L, 0, 4);
	lua_pushinteger(L, int(MonsterSpace::eGA_Rest));	lua_setfield(L, -2, "rest");
	lua_pushinteger(L, int(MonsterSpace::eGA_Eat));	lua_setfield(L, -2, "eat");
	lua_pushinteger(L, int(MonsterSpace::eGA_Attack));	lua_setfield(L, -2, "attack");
	lua_pushinteger(L, int(MonsterSpace::eGA_Panic));	lua_setfield(L, -2, "panic");
	lua_setfield(L, -2, "type");
	lua_pop(L, 1);
}
