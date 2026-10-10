////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_sound.h"
#include "script_game_object.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptSound::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CSound_params>("sound_params")
			.addPropertyReadWrite("position", &CSound_params::position)
			.addPropertyReadWrite("volume", &CSound_params::volume)
			.addPropertyReadWrite("frequency", &CSound_params::freq)
			.addPropertyReadWrite("min_distance", &CSound_params::min_distance)
			.addPropertyReadWrite("max_distance", &CSound_params::max_distance)
		.endClass()

		.beginClass<CScriptSound>("sound_object")
			.addProperty("frequency", [](CScriptSound const& self) { return self.GetFrequency(); }, &CScriptSound::SetFrequency)
			.addProperty("min_distance", [](CScriptSound const& self) { return self.GetMinDistance(); }, &CScriptSound::SetMinDistance)
			.addProperty("max_distance", [](CScriptSound const& self) { return self.GetMaxDistance(); }, &CScriptSound::SetMaxDistance)
			.addProperty("volume", [](CScriptSound const& self) { return self.GetVolume(); }, &CScriptSound::SetVolume)
			.addProperty("position", [](CScriptSound const& self) { return self.GetPosition(); }, &CScriptSound::SetPosition)
			.addConstructor<void(*)(LPCSTR)>()
			.addConstructor<void(*)(LPCSTR, ESoundTypes)>()
			.addFunction("get_position", &CScriptSound::GetPosition)
			.addFunction("set_position", &CScriptSound::SetPosition)
			.addFunction("play", (void (CScriptSound::*)(CScriptGameObject*))(&CScriptSound::Play), (void (CScriptSound::*)(CScriptGameObject*, float))(&CScriptSound::Play), (void (CScriptSound::*)(CScriptGameObject*, float, int))(&CScriptSound::Play))
			.addFunction("play_at_pos", (void (CScriptSound::*)(CScriptGameObject*, const Fvector&))(&CScriptSound::PlayAtPos), (void (CScriptSound::*)(CScriptGameObject*, const Fvector&, float))(&CScriptSound::PlayAtPos), (void (CScriptSound::*)(CScriptGameObject*, const Fvector&, float, int))(&CScriptSound::PlayAtPos))
			.addFunction("play_no_feedback", &CScriptSound::PlayNoFeedback)
			.addFunction("stop", &CScriptSound::Stop)
			.addFunction("stop_deffered", &CScriptSound::StopDeffered)
			.addFunction("playing", &CScriptSound::IsPlaying)
			.addFunction("length", &CScriptSound::Length)
			.addFunction("attach_tail", &CScriptSound::AttachTail)
		.endClass();

	lua_getglobal(L, "sound_object");
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, sm_Looped);	lua_setfield(L, -2, "looped");
	lua_pushinteger(L, sm_2D);	lua_setfield(L, -2, "s2d");
	lua_pushinteger(L, 0);	lua_setfield(L, -2, "s3d");
	lua_setfield(L, -2, "sound_play_type");
	lua_pop(L, 1);
}
