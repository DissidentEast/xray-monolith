////////////////////////////////////////////////////////////////////////////
//	Module 		: script_animation_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script animation action class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_animation_action.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptAnimationAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptAnimationAction>("anim")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(LPCSTR)>()
			.addConstructor<void(*)(LPCSTR, bool)>()
			.addConstructor<void(*)(MonsterSpace::EMentalState)>()
			// Monster specific
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterAnimAction, int)>()
			.addFunction("anim", &CScriptAnimationAction::SetAnimation)
			.addFunction("type", &CScriptAnimationAction::SetMentalState)
			.addFunction("completed", (bool (CScriptAnimationAction::*)())(&CScriptAnimationAction::completed))
		.endClass();

	lua_getglobal(L, "anim");
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, int(MonsterSpace::eMentalStateFree));	lua_setfield(L, -2, "free");
	lua_pushinteger(L, int(MonsterSpace::eMentalStateDanger));	lua_setfield(L, -2, "danger");
	lua_pushinteger(L, int(MonsterSpace::eMentalStatePanic));	lua_setfield(L, -2, "panic");
	lua_setfield(L, -2, "type");
	lua_createtable(L, 0, 10);
	lua_pushinteger(L, int(MonsterSpace::eAA_StandIdle));	lua_setfield(L, -2, "stand_idle");
	lua_pushinteger(L, int(MonsterSpace::eAA_CapturePrepare));	lua_setfield(L, -2, "capture_prepare");
	lua_pushinteger(L, int(MonsterSpace::eAA_SitIdle));	lua_setfield(L, -2, "sit_idle");
	lua_pushinteger(L, int(MonsterSpace::eAA_LieIdle));	lua_setfield(L, -2, "lie_idle");
	lua_pushinteger(L, int(MonsterSpace::eAA_Eat));	lua_setfield(L, -2, "eat");
	lua_pushinteger(L, int(MonsterSpace::eAA_Sleep));	lua_setfield(L, -2, "sleep");
	lua_pushinteger(L, int(MonsterSpace::eAA_Rest));	lua_setfield(L, -2, "rest");
	lua_pushinteger(L, int(MonsterSpace::eAA_Attack));	lua_setfield(L, -2, "attack");
	lua_pushinteger(L, int(MonsterSpace::eAA_LookAround));	lua_setfield(L, -2, "look_around");
	lua_pushinteger(L, int(MonsterSpace::eAA_Turn));	lua_setfield(L, -2, "turn");
	lua_setfield(L, -2, "monster");
	lua_pop(L, 1);
}
