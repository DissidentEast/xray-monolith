////////////////////////////////////////////////////////////////////////////
//	Module 		: script_object_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object action class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_object_action.h"
#include "script_game_object.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptObjectAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptObjectAction>("object")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(CScriptGameObject*, MonsterSpace::EObjectAction)>()
			.addConstructor<void(*)(CScriptGameObject*, MonsterSpace::EObjectAction, u32)>()
			.addConstructor<void(*)(MonsterSpace::EObjectAction)>()
			.addConstructor<void(*)(LPCSTR, MonsterSpace::EObjectAction)>()
			.addFunction("action", &CScriptObjectAction::SetObjectAction)
			.addFunction("object", (void (CScriptObjectAction::*)(LPCSTR))(&CScriptObjectAction::SetObject), (void (CScriptObjectAction::*)(CScriptGameObject*))(&CScriptObjectAction::SetObject))
			.addFunction("completed", (bool (CScriptObjectAction::*)())(&CScriptObjectAction::completed))
		.endClass();

	lua_getglobal(L, "object");
	lua_createtable(L, 0, 23);
	lua_pushinteger(L, int(MonsterSpace::eObjectActionIdle));	lua_setfield(L, -2, "idle");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionShow));	lua_setfield(L, -2, "show");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionHide));	lua_setfield(L, -2, "hide");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionTake));	lua_setfield(L, -2, "take");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionDrop));	lua_setfield(L, -2, "drop");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionStrapped));	lua_setfield(L, -2, "strap");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionAim1));	lua_setfield(L, -2, "aim1");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionAim2));	lua_setfield(L, -2, "aim2");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionReload1));	lua_setfield(L, -2, "reload");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionReload1));	lua_setfield(L, -2, "reload1");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionReload2));	lua_setfield(L, -2, "reload2");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionFire1));	lua_setfield(L, -2, "fire1");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionFire2));	lua_setfield(L, -2, "fire2");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionSwitch1));	lua_setfield(L, -2, "switch1");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionSwitch2));	lua_setfield(L, -2, "switch2");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionActivate));	lua_setfield(L, -2, "activate");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionDeactivate));	lua_setfield(L, -2, "deactivate");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionUse));	lua_setfield(L, -2, "use");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionTurnOn));	lua_setfield(L, -2, "turn_on");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionTurnOff));	lua_setfield(L, -2, "turn_off");
	lua_pushinteger(L, int(MonsterSpace::eObjectActionDummy));	lua_setfield(L, -2, "dummy");
	lua_setfield(L, -2, "state");
	lua_pop(L, 1);
}
