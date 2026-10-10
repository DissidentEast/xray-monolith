////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script sound action class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_sound_action.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptSoundAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptSoundAction>("sound")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(LPCSTR, LPCSTR)>()
			.addConstructor<void(*)(LPCSTR, LPCSTR, const Fvector&)>()
			.addConstructor<void(*)(LPCSTR, LPCSTR, const Fvector&, const Fvector&)>()
			.addConstructor<void(*)(LPCSTR, LPCSTR, const Fvector&, const Fvector&, bool)>()
			.addConstructor<void(*)(LPCSTR, Fvector*)>()
			.addConstructor<void(*)(LPCSTR, Fvector*, const Fvector&)>()
			.addConstructor<void(*)(LPCSTR, Fvector*, const Fvector&, bool)>()
			.addConstructor<void(*)(CScriptSound*, LPCSTR, const Fvector&)>()
			.addConstructor<void(*)(CScriptSound*, LPCSTR, const Fvector&, const Fvector&)>()
			.addConstructor<void(*)(CScriptSound*, LPCSTR, const Fvector&, const Fvector&, bool)>()
			.addConstructor<void(*)(CScriptSound*, Fvector*)>()
			.addConstructor<void(*)(CScriptSound*, Fvector*, const Fvector&)>()
			.addConstructor<void(*)(CScriptSound*, Fvector*, const Fvector&, bool)>()
			// monster specific
			.addConstructor<void(*)(MonsterSound::EType)>()
			.addConstructor<void(*)(MonsterSound::EType, int)>()
			// trader specific
			.addConstructor<void(*)(LPCSTR, LPCSTR, MonsterSpace::EMonsterHeadAnimType)>()

			.addFunction("set_sound", (void (CScriptSoundAction::*)(LPCSTR))(&CScriptSoundAction::SetSound), (void (CScriptSoundAction::*)(const CScriptSound&))(&CScriptSoundAction::SetSound))
			.addFunction("set_sound_type", &CScriptSoundAction::SetSoundType)
			.addFunction("set_bone", &CScriptSoundAction::SetBone)
			.addFunction("set_position", &CScriptSoundAction::SetPosition)
			.addFunction("set_angles", &CScriptSoundAction::SetAngles)
			.addFunction("completed", (bool (CScriptSoundAction::*)())(&CScriptSoundAction::completed))
		.endClass();

	lua_getglobal(L, "sound");
	lua_createtable(L, 0, 9);
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundIdle));	lua_setfield(L, -2, "idle");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundEat));	lua_setfield(L, -2, "eat");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundAggressive));	lua_setfield(L, -2, "attack");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundAttackHit));	lua_setfield(L, -2, "attack_hit");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundTakeDamage));	lua_setfield(L, -2, "take_damage");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundDie));	lua_setfield(L, -2, "die");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundThreaten));	lua_setfield(L, -2, "threaten");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundSteal));	lua_setfield(L, -2, "steal");
	lua_pushinteger(L, int(MonsterSound::eMonsterSoundPanic));	lua_setfield(L, -2, "panic");
	lua_setfield(L, -2, "type");
	lua_pop(L, 1);
}
