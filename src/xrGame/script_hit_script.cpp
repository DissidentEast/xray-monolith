////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_hit.h"
#include "script_game_object.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptHit::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptHit>("hit")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(CScriptHit const*)>()
			.addPropertyReadWrite("power", &CScriptHit::m_fPower)
			.addPropertyReadWrite("direction", &CScriptHit::m_tDirection)
			.addPropertyReadWrite("draftsman", &CScriptHit::m_tpDraftsman)
			.addPropertyReadWrite("impulse", &CScriptHit::m_fImpulse)
			.addPropertyReadWrite("type", &CScriptHit::m_tHitType)
			.addPropertyReadWrite("weapon_id", &CScriptHit::m_tpWeaponID)
			.addProperty("bullet_id", &CScriptHit::bulletId)
			.addFunction("bone", &CScriptHit::set_bone_name)
			.addProperty("bone", &CScriptHit::get_bone_name, &CScriptHit::set_bone_name)
		.endClass();

	// luabind's class_::enum_ created a nested hit.hit_type table of ints.
	// LuaBridge3 has no enum-table registration, so build the identical
	// layout with the plain API (same global path scripts/mods use).
	lua_getglobal(L, "hit");
	lua_createtable(L, 0, 12);
	lua_pushinteger(L, int(ALife::eHitTypeBurn));		lua_setfield(L, -2, "burn");
	lua_pushinteger(L, int(ALife::eHitTypeShock));		lua_setfield(L, -2, "shock");
	lua_pushinteger(L, int(ALife::eHitTypeStrike));		lua_setfield(L, -2, "strike");
	lua_pushinteger(L, int(ALife::eHitTypeWound));		lua_setfield(L, -2, "wound");
	lua_pushinteger(L, int(ALife::eHitTypeRadiation));	lua_setfield(L, -2, "radiation");
	lua_pushinteger(L, int(ALife::eHitTypeTelepatic));	lua_setfield(L, -2, "telepatic");
	lua_pushinteger(L, int(ALife::eHitTypeChemicalBurn));	lua_setfield(L, -2, "chemical_burn");
	lua_pushinteger(L, int(ALife::eHitTypeExplosion));	lua_setfield(L, -2, "explosion");
	lua_pushinteger(L, int(ALife::eHitTypeFireWound));	lua_setfield(L, -2, "fire_wound");
	lua_pushinteger(L, int(ALife::eHitTypeLightBurn));	lua_setfield(L, -2, "light_burn");
	lua_pushinteger(L, int(ALife::eHitTypeMax));		lua_setfield(L, -2, "dummy");
	lua_setfield(L, -2, "hit_type");
	lua_pop(L, 1);
}
