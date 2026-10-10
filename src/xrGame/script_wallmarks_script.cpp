#include "pch_script.h"
#include "script_wallmarks_manager.h"

#include "LuaBridge/LuaBridge.h"

ScriptWallmarksManager* GetManager()
{
	return &g_pGamePersistent->GetWallmarksManager();
}

#pragma optimize("s",on)
void CScriptWallmarksManager::script_register(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<ScriptWallmarksManager>("ScriptWallmarksManager")
			.addConstructor<void(*)()>()
			.addFunction("place", (void (ScriptWallmarksManager::*)(Fvector, Fvector, float, float, LPCSTR, CScriptGameObject*, float))(&ScriptWallmarksManager::PlaceWallmark), (void (ScriptWallmarksManager::*)(Fvector, Fvector, float, float, LPCSTR, CScriptGameObject*, float, bool))(&ScriptWallmarksManager::PlaceWallmark)

			// demonized: add user defined rotation to wallmark
			, (void (ScriptWallmarksManager::*)(Fvector, Fvector, float, float, LPCSTR, CScriptGameObject*, float, float))(&ScriptWallmarksManager::PlaceWallmark))

			.addFunction("place_skeleton", &ScriptWallmarksManager::PlaceSkeletonWallmark)
		.endClass()
		.addFunction("wallmarks_manager", &GetManager);
}