#include "pch_script.h"
#include "script_monster_hit_info.h"
#include "script_game_object.h"
#include "ai_monster_space.h"
#include "AI/Monsters/monster_sound_defs.h"

#include "LuaBridge/LuaBridge.h"

struct CMonsterSpace
{
};

#pragma optimize("s",on)
void CScriptMonsterHitInfo::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptMonsterHitInfo>("MonsterHitInfo")
			.addPropertyReadWrite("who", &CScriptMonsterHitInfo::who)
			.addPropertyReadWrite("direction", &CScriptMonsterHitInfo::direction)
			.addPropertyReadWrite("time", &CScriptMonsterHitInfo::time)
		.endClass()

		.beginClass<CMonsterSpace>("MonsterSpace")
		.endClass();

	lua_getglobal(L, "MonsterSpace");
	lua_createtable(L, 0, 1);
	lua_pushinteger(L, MonsterSound::eMonsterSoundScript);	lua_setfield(L, -2, "sound_script");
	lua_setfield(L, -2, "sounds");
	lua_createtable(L, 0, 4);
	lua_pushinteger(L, MonsterSpace::eHeadAnimNormal);	lua_setfield(L, -2, "head_anim_normal");
	lua_pushinteger(L, MonsterSpace::eHeadAnimAngry);	lua_setfield(L, -2, "head_anim_angry");
	lua_pushinteger(L, MonsterSpace::eHeadAnimGlad);	lua_setfield(L, -2, "head_anim_glad");
	lua_pushinteger(L, MonsterSpace::eHeadAnimKind);	lua_setfield(L, -2, "head_anim_kind");
	lua_setfield(L, -2, "head_anim");
	lua_pop(L, 1);
}
