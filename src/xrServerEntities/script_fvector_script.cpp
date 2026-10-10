//////////////////////////////////////////////////////////////////////////
//	Module 		: script_fvector_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script float vector script export
//////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_fvector.h"

#include "LuaBridge/LuaBridge.h"

// HUD transforms live on Device (xrEngine), not in xrCore math, so they are
// exposed to Lua via free functions instead of Fvector methods.
// Follows the script_time_global() XRGAME_EXPORTS pattern below.
#ifdef XRGAME_EXPORTS
ICF Fvector& fvector_hud_to_world(Fvector& v) { Device.hud_to_world(v); return v; }
ICF Fvector& fvector_world_to_hud(Fvector& v) { Device.world_to_hud(v); return v; }
ICF Fvector& fvector_hud_to_world_dir(Fvector& v) { Device.hud_to_world_dir(v); return v; }
ICF Fvector& fvector_world_to_hud_dir(Fvector& v) { Device.world_to_hud_dir(v); return v; }
#else
ICF Fvector& fvector_hud_to_world(Fvector& v) { return v; }
ICF Fvector& fvector_world_to_hud(Fvector& v) { return v; }
ICF Fvector& fvector_hud_to_world_dir(Fvector& v) { return v; }
ICF Fvector& fvector_world_to_hud_dir(Fvector& v) { return v; }
#endif // XRGAME_EXPORTS

// Ported luabind -> LuaBridge3 (5.4-ready, still on LuaJIT):
// - module/class/def -> getGlobalNamespace/beginClass/addFunction
// - .scope()[def] statics -> addStaticFunction
// - overload sets -> single addFunction with all overloads (arity+type dispatch)
// - return_reference_to<1>() dropped: reference returns stay aliased, chaining works
#pragma optimize("s",on)
void CScriptFvector::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<Fvector>("vector")
		// demonized: new exports of static functions, in Lua use like this: vector.generate_orthonormal_basis(a, b, c)
		.addStaticFunction("generate_orthonormal_basis", &Fvector::generate_orthonormal_basis)
		.addStaticFunction("generate_orthonormal_basis_normalized", &Fvector::generate_orthonormal_basis_normalized)
		.addPropertyReadWrite("x", &Fvector::x)
		.addPropertyReadWrite("y", &Fvector::y)
		.addPropertyReadWrite("z", &Fvector::z)
		.addConstructor<void(*)()>()
		.addFunction("set", (Fvector & (Fvector::*)(float, float, float))(&Fvector::set), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::set))
		.addFunction("add", (Fvector & (Fvector::*)(float))(&Fvector::add), (Fvector & (Fvector::*)(float, float, float))(&Fvector::add), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::add), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::add), (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::add))
		.addFunction("sub", (Fvector & (Fvector::*)(float))(&Fvector::sub), (Fvector & (Fvector::*)(float, float, float))(&Fvector::sub), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::sub), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::sub), (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::sub))
		.addFunction("mul", (Fvector & (Fvector::*)(float))(&Fvector::mul), (Fvector & (Fvector::*)(float, float, float))(&Fvector::mul), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::mul), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::mul), (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::mul))
		.addFunction("div", (Fvector & (Fvector::*)(float))(&Fvector::div), (Fvector & (Fvector::*)(float, float, float))(&Fvector::div), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::div), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::div), (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::div))
		.addFunction("invert", (Fvector & (Fvector::*)())(&Fvector::invert), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::invert))
		.addFunction("min", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::min), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::min))
		.addFunction("max", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::max), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::max))
		.addFunction("abs", &Fvector::abs)
		.addFunction("similar", &Fvector::similar)
		.addFunction("set_length", &Fvector::set_length)
		.addFunction("align", &Fvector::align)
		//			.addFunction("squeeze",						&Fvector::squeeze)
		.addFunction("clamp", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::clamp), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::clamp))
		.addFunction("inertion", &Fvector::inertion)
		.addFunction("average", (Fvector & (Fvector::*)(const Fvector&))(&Fvector::average), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::average))
		.addFunction("lerp", &Fvector::lerp)
		.addFunction("mad", (Fvector & (Fvector::*)(const Fvector&, float))(&Fvector::mad), (Fvector & (Fvector::*)(const Fvector&, const Fvector&, float))(&Fvector::mad), (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::mad), (Fvector & (Fvector::*)(const Fvector&, const Fvector&, const Fvector&))(&Fvector::mad))
		//			.addFunction("square_magnitude",			&Fvector::square_magnitude)
		.addFunction("magnitude", &Fvector::magnitude)
		//			.addFunction("normalize_magnitude",			&Fvector::normalize_magn)
		.addFunction("normalize", (Fvector & (Fvector::*)())(&Fvector::normalize_safe), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::normalize_safe))
		.addFunction("normalize_safe", (Fvector & (Fvector::*)())(&Fvector::normalize_safe), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::normalize_safe))
		//			.addFunction("random_dir",					(Fvector & (Fvector::*)())(&Fvector::random_dir))
		//			.addFunction("random_dir",					(Fvector & (Fvector::*)(const Fvector &, float))(&Fvector::random_dir))
		//			.addFunction("random_point",				(Fvector & (Fvector::*)(const Fvector &))(&Fvector::random_point))
		//			.addFunction("random_point",				(Fvector & (Fvector::*)(float))(&Fvector::random_point))
		.addFunction("dotproduct", &Fvector::dotproduct)
		.addFunction("crossproduct", &Fvector::crossproduct)
		.addFunction("distance_to_xz", &Fvector::distance_to_xz)
		.addFunction("distance_to_xz_sqr", &Fvector::distance_to_xz_sqr)
		.addFunction("distance_to_sqr", &Fvector::distance_to_sqr)
		.addFunction("distance_to", &Fvector::distance_to)
		//			.addFunction("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, float, float, float))(&Fvector::from_bary))
		//			.addFunction("from_bary",					(Fvector & (Fvector::*)(const Fvector &, const Fvector &, const Fvector &, const Fvector &))(&Fvector::from_bary))
		//			.addFunction("from_bary4",					&Fvector::from_bary4)
		//			.addFunction("mknormal_non_normalized",		&Fvector::mknormal_non_normalized)
		//			.addFunction("mknormal",					&Fvector::mknormal)
		.addFunction("setHP", &Fvector::setHP)
		//			.addFunction("getHP",						&Fvector::getHP)
		.addFunction("getH", &Fvector::getH)
		.addFunction("getP", &Fvector::getP)

		.addFunction("reflect", &Fvector::reflect)
		.addFunction("slide", &Fvector::slide)

		// demonized: new exports
		.addFunction("project", (Fvector & (Fvector::*)(const Fvector&, const Fvector&))(&Fvector::project), (Fvector & (Fvector::*)(const Fvector&))(&Fvector::project))
		.addFunction("hud_to_world", &fvector_hud_to_world)
		.addFunction("world_to_hud", &fvector_world_to_hud)
		.addFunction("hud_to_world_dir", &fvector_hud_to_world_dir)
		.addFunction("world_to_hud_dir", &fvector_world_to_hud_dir)
		.endClass()

		.beginClass<Fvector2>("vector2")
		.addPropertyReadWrite("x", &Fvector2::x)
		.addPropertyReadWrite("y", &Fvector2::y)
		.addConstructor<void(*)()>()

		// demonized: new exports
		.addFunction("normalize", (Fvector2 & (Fvector2::*)(void))(&Fvector2::normalize))

		.addFunction("set", (Fvector2 & (Fvector2::*)(float, float))(&Fvector2::set), (Fvector2 & (Fvector2::*)(const Fvector2&))(&Fvector2::set))
		.endClass()

		.beginClass<Fvector4>("vector4")
		.addPropertyReadWrite("x", &Fvector4::x)
		.addPropertyReadWrite("y", &Fvector4::y)
		.addPropertyReadWrite("z", &Fvector4::z)
		.addPropertyReadWrite("w", &Fvector4::w)
		.addConstructor<void(*)()>()
		.addFunction("set", (Fvector4& (Fvector4::*)(float, float, float, float))(&Fvector4::set), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::set))
		.addFunction("add", (Fvector4& (Fvector4::*)(float, float, float, float))(&Fvector4::add), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::add), (Fvector4& (Fvector4::*)(float))(&Fvector4::add), (Fvector4& (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::add), (Fvector4& (Fvector4::*)(const Fvector4&, float))(&Fvector4::add))
		.addFunction("sub", (Fvector4& (Fvector4::*)(float, float, float, float))(&Fvector4::sub), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::sub), (Fvector4& (Fvector4::*)(float))(&Fvector4::sub), (Fvector4& (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::sub), (Fvector4& (Fvector4::*)(const Fvector4&, float))(&Fvector4::sub))
		.addFunction("mul", (Fvector4& (Fvector4::*)(float, float, float, float))(&Fvector4::mul), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::mul), (Fvector4& (Fvector4::*)(float))(&Fvector4::mul), (Fvector4& (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::mul), (Fvector4& (Fvector4::*)(const Fvector4&, float))(&Fvector4::mul))
		.addFunction("div", (Fvector4& (Fvector4::*)(float, float, float, float))(&Fvector4::div), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::div), (Fvector4& (Fvector4::*)(float))(&Fvector4::div), (Fvector4& (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::div), (Fvector4& (Fvector4::*)(const Fvector4&, float))(&Fvector4::div))
		.addFunction("clamp", (Fvector4& (Fvector4::*)(const Fvector4&, const Fvector4&))(&Fvector4::clamp), (Fvector4& (Fvector4::*)(const Fvector4&))(&Fvector4::clamp))
		.addFunction("similar", &Fvector4::similar)
		.addFunction("magnitude", &Fvector4::magnitude)
		.addFunction("normalize", &Fvector4::normalize)
		.addFunction("normalize_as_plane", &Fvector4::normalize_as_plane)
		.addFunction("lerp", &Fvector4::lerp)
		.endClass()

		.beginClass<Fbox>("Fbox")
		.addPropertyReadWrite("min", &Fbox::min)
		.addPropertyReadWrite("max", &Fbox::max)
		.addConstructor<void(*)()>()
		.endClass()

		.beginClass<Frect>("Frect")
		.addConstructor<void(*)()>()
		.addFunction("set", (Frect & (Frect::*)(float, float, float, float))(&Frect::set))
		.addPropertyReadWrite("lt", &Frect::lt)
		.addPropertyReadWrite("rb", &Frect::rb)
		.addPropertyReadWrite("x1", &Frect::x1)
		.addPropertyReadWrite("x2", &Frect::x2)
		.addPropertyReadWrite("y1", &Frect::y1)
		.addPropertyReadWrite("y2", &Frect::y2)
		.endClass();
}
