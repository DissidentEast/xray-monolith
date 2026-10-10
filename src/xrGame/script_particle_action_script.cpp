////////////////////////////////////////////////////////////////////////////
//	Module 		: script_particle_action.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script particle action class
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_particle_action.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptParticleAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptParticleAction>("particle")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(LPCSTR, LPCSTR)>()
			.addConstructor<void(*)(LPCSTR, LPCSTR, const CParticleParams&)>()
			.addConstructor<void(*)(LPCSTR, LPCSTR, const CParticleParams&, bool)>()
			.addConstructor<void(*)(LPCSTR, const CParticleParams&)>()
			.addConstructor<void(*)(LPCSTR, const CParticleParams&, bool)>()
			.addFunction("set_particle", &CScriptParticleAction::SetParticle)
			.addFunction("set_bone", &CScriptParticleAction::SetBone)
			.addFunction("set_position", &CScriptParticleAction::SetPosition)
			.addFunction("set_angles", &CScriptParticleAction::SetAngles)
			.addFunction("set_velocity", &CScriptParticleAction::SetVelocity)
			.addFunction("completed", (bool (CScriptParticleAction::*)())(&CScriptParticleAction::completed))
		.endClass();
}
