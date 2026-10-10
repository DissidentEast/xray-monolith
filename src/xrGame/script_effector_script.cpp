////////////////////////////////////////////////////////////////////////////
//	Module 		: script_effector.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script effector class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_effector.h"
#include "script_effector_wrapper.h"

#include "LuaBridge/LuaBridge.h"

void SPPInfo_assign(SPPInfo* self, SPPInfo* obj)
{
	*self = *obj;
}

void add_effector(CScriptEffector* self)
{
	self->Add();
}

void remove_effector(CScriptEffector* self)
{
	self->Remove();
}

#pragma optimize("s",on)
void CScriptEffector::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<SPPInfo::SDuality>("duality")
			.addPropertyReadWrite("h", &SPPInfo::SDuality::h)
			.addPropertyReadWrite("v", &SPPInfo::SDuality::v)
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(float, float)>()
			.addFunction("set", &SPPInfo::SDuality::set)
		.endClass()

		.beginClass<SPPInfo::SColor>("color")
			.addPropertyReadWrite("r", &SPPInfo::SColor::r)
			.addPropertyReadWrite("g", &SPPInfo::SColor::g)
			.addPropertyReadWrite("b", &SPPInfo::SColor::b)
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(float, float, float)>()
			.addFunction("set", &SPPInfo::SColor::set)
		.endClass()

		.beginClass<SPPInfo::SNoise>("noise")
			.addPropertyReadWrite("intensity", &SPPInfo::SNoise::intensity)
			.addPropertyReadWrite("grain", &SPPInfo::SNoise::grain)
			.addPropertyReadWrite("fps", &SPPInfo::SNoise::fps)
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(float, float, float)>()
			.addFunction("set", &SPPInfo::SNoise::set)
		.endClass()

		.beginClass<SPPInfo>("effector_params")
			.addPropertyReadWrite("blur", &SPPInfo::blur)
			.addPropertyReadWrite("gray", &SPPInfo::gray)
			.addPropertyReadWrite("dual", &SPPInfo::duality)
			.addPropertyReadWrite("noise", &SPPInfo::noise)
			.addPropertyReadWrite("color_base", &SPPInfo::color_base)
			.addPropertyReadWrite("color_gray", &SPPInfo::color_gray)
			.addPropertyReadWrite("color_add", &SPPInfo::color_add)
			.addConstructor<void(*)()>()
			.addFunction("assign", &SPPInfo_assign)
		.endClass()

		// NOTE: wrapper base (CScriptEffectorWrapper, luabind::wrap_base) dropped:
		// no shipped script subclasses effector, so Lua-side virtual overrides
		// are dormant; adopt<1>() ownership on start/finish likewise.
		// Playtest item: cam_effector sequences (actor hits, blowout).
		.beginClass<CScriptEffector>("effector")
			.addConstructor<void(*)(int, float)>()
			.addFunction("start", &add_effector)
			.addFunction("finish", &remove_effector)
			.addFunction("process", &CScriptEffector::process)
		.endClass();
}
