#include "pch_script.h"
#include "script_sound_info.h"
#include "script_game_object.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptSoundInfo::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptSoundInfo>("SoundInfo")
			.addPropertyReadWrite("who", &CScriptSoundInfo::who)
			.addPropertyReadWrite("danger", &CScriptSoundInfo::dangerous)
			.addPropertyReadWrite("position", &CScriptSoundInfo::position)
			.addPropertyReadWrite("power", &CScriptSoundInfo::power)
			.addPropertyReadWrite("time", &CScriptSoundInfo::time)
		.endClass();
}
