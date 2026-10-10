////////////////////////////////////////////////////////////////////////////
//	Module 		: script_world_property_script.h
//	Created 	: 19.03.2004
//  Modified 	: 19.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script world property script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_world_property.h"
#include "operator_abstract.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)

static bool world_property_less(CScriptWorldProperty const& a, CScriptWorldProperty const& b)
{
	return a < b;
}

static bool world_property_equal(CScriptWorldProperty const& a, CScriptWorldProperty const& b)
{
	return a == b;
}

void CScriptWorldPropertyWrapper::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptWorldProperty>("world_property")
			.addConstructor<void(*)(CScriptWorldProperty::_condition_type, CScriptWorldProperty::_value_type)>()
			.addFunction("condition", &CScriptWorldProperty::condition)
			.addFunction("value", &CScriptWorldProperty::value)
			.addFunction("__lt", &world_property_less)
			.addFunction("__eq", &world_property_equal)
		.endClass();
}
