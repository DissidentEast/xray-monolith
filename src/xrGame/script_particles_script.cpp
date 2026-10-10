////////////////////////////////////////////////////////////////////////////
//	Module 		: script_sound_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 06.02.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script sound class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_particles.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptParticles::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptParticles>("particles_object")
			.addConstructor<void(*)(LPCSTR)>()
			.addFunction("play", &CScriptParticles::Play)
			.addFunction("play_at_pos", &CScriptParticles::PlayAtPos)
			.addFunction("stop", &CScriptParticles::Stop)
			.addFunction("stop_deffered", &CScriptParticles::StopDeffered)

			.addFunction("playing", &CScriptParticles::IsPlaying)
			.addFunction("looped", &CScriptParticles::IsLooped)

			.addFunction("move_to", &CScriptParticles::MoveTo)
			.addFunction("set_position", &CScriptParticles::XFORMMoveTo)
			.addFunction("set_direction", &CScriptParticles::SetDirection)
			.addFunction("set_orientation", &CScriptParticles::SetOrientation)
			.addFunction("set_hud_mode", &CScriptParticles::SetHudMode)

			.addFunction("last_position", &CScriptParticles::LastPosition)
			.addFunction("load_path", &CScriptParticles::LoadPath)
			.addFunction("start_path", &CScriptParticles::StartPath)
			.addFunction("stop_path", &CScriptParticles::StopPath)
			.addFunction("pause_path", &CScriptParticles::PausePath)
		.endClass();
}
