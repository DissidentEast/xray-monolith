////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone_script.cpp
//	Created 	: 10.10.2003
//  Modified 	: 11.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Script zone object script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_zone.h"
#include "smart_zone.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptZone::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<CScriptZone, DLL_Pure>("ce_script_zone")
			.addConstructor<void(*)()>()
		.endClass();
}

void CSmartZone::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<CSmartZone, DLL_Pure>("ce_smart_zone")
			.addConstructor<void(*)()>()
		.endClass();
}
