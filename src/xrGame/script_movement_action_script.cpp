////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action_script.cpp
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_movement_action.h"
#include "script_game_object.h"
#include "patrol_path_manager_space.h"
#include "detail_path_manager_space.h"
#include "ai_monster_space.h"
#include "patrol_path_params.h"
#include "patrol_path.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CScriptMovementAction::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CScriptMovementAction>("move")
			.addConstructor<void(*)()>()
			.addConstructor<void(*)(const CScriptMovementAction::EInputKeys)>()
			.addConstructor<void(*)(const CScriptMovementAction::EInputKeys, float)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CScriptGameObject*)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CScriptGameObject*, float)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CPatrolPathParams*)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, CPatrolPathParams*, float)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, Fvector*)>()
			.addConstructor<void(*)(MonsterSpace::EBodyState, MonsterSpace::EMovementType, DetailPathManager::EDetailPathType, Fvector*, float)>()
			.addConstructor<void(*)(Fvector*, float)>()

			// Monsters
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, Fvector*)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams*)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, Fvector*, float)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, u32, Fvector*)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, u32, Fvector*, float)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams*, float)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*, float)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, Fvector*, float, MonsterSpace::EScriptMonsterSpeedParam)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CPatrolPathParams*, float, MonsterSpace::EScriptMonsterSpeedParam)>()
			.addConstructor<void(*)(MonsterSpace::EScriptMonsterMoveAction, CScriptGameObject*, float, MonsterSpace::EScriptMonsterSpeedParam)>()

			.addFunction("body", &CScriptMovementAction::SetBodyState)
			.addFunction("move", &CScriptMovementAction::SetMovementType)
			.addFunction("path", &CScriptMovementAction::SetPathType)
			.addFunction("object", &CScriptMovementAction::SetObjectToGo)
			.addFunction("patrol", &CScriptMovementAction::SetPatrolPath)
			.addFunction("position", &CScriptMovementAction::SetPosition)
			.addFunction("input", &CScriptMovementAction::SetInputKeys)
			.addFunction("completed", (bool (CScriptMovementAction::*)())(&CScriptMovementAction::completed))
		.endClass();

	lua_getglobal(L, "move");
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, int(MonsterSpace::eBodyStateCrouch));	lua_setfield(L, -2, "crouch");
	lua_pushinteger(L, int(MonsterSpace::eBodyStateStand));	lua_setfield(L, -2, "standing");
	lua_setfield(L, -2, "body");
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, int(MonsterSpace::eMovementTypeWalk));	lua_setfield(L, -2, "walk");
	lua_pushinteger(L, int(MonsterSpace::eMovementTypeRun));	lua_setfield(L, -2, "run");
	lua_pushinteger(L, int(MonsterSpace::eMovementTypeStand));	lua_setfield(L, -2, "stand");
	lua_setfield(L, -2, "move");
	lua_createtable(L, 0, 5);
	lua_pushinteger(L, int(DetailPathManager::eDetailPathTypeSmooth));	lua_setfield(L, -2, "line");
	lua_pushinteger(L, int(DetailPathManager::eDetailPathTypeSmoothDodge));	lua_setfield(L, -2, "dodge");
	lua_pushinteger(L, int(DetailPathManager::eDetailPathTypeSmoothCriteria));	lua_setfield(L, -2, "criteria");
	lua_pushinteger(L, int(DetailPathManager::eDetailPathTypeSmooth));	lua_setfield(L, -2, "curve");
	lua_pushinteger(L, int(DetailPathManager::eDetailPathTypeSmoothCriteria));	lua_setfield(L, -2, "curve_criteria");
	lua_setfield(L, -2, "path");
	lua_createtable(L, 0, 10);
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyNone));	lua_setfield(L, -2, "none");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyForward));	lua_setfield(L, -2, "fwd");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyBack));	lua_setfield(L, -2, "back");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyLeft));	lua_setfield(L, -2, "left");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyRight));	lua_setfield(L, -2, "right");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyShiftUp));	lua_setfield(L, -2, "up");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyShiftDown));	lua_setfield(L, -2, "down");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyBreaks));	lua_setfield(L, -2, "handbrake");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyEngineOn));	lua_setfield(L, -2, "on");
	lua_pushinteger(L, int(CScriptMovementAction::eInputKeyEngineOff));	lua_setfield(L, -2, "off");
	lua_setfield(L, -2, "input");
	lua_createtable(L, 0, 8);
	lua_pushinteger(L, int(MonsterSpace::eMA_WalkFwd));	lua_setfield(L, -2, "walk_fwd");
	lua_pushinteger(L, int(MonsterSpace::eMA_WalkBkwd));	lua_setfield(L, -2, "walk_bkwd");
	lua_pushinteger(L, int(MonsterSpace::eMA_Run));	lua_setfield(L, -2, "run_fwd");
	lua_pushinteger(L, int(MonsterSpace::eMA_Drag));	lua_setfield(L, -2, "drag");
	lua_pushinteger(L, int(MonsterSpace::eMA_Jump));	lua_setfield(L, -2, "jump");
	lua_pushinteger(L, int(MonsterSpace::eMA_Steal));	lua_setfield(L, -2, "steal");
	lua_pushinteger(L, int(MonsterSpace::eMA_WalkWithLeader));	lua_setfield(L, -2, "walk_with_leader");
	lua_pushinteger(L, int(MonsterSpace::eMA_RunWithLeader));	lua_setfield(L, -2, "run_with_leader");
	lua_setfield(L, -2, "monster");
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, int(MonsterSpace::eSP_Default));	lua_setfield(L, -2, "default");
	lua_pushinteger(L, int(MonsterSpace::eSP_ForceSpeed));	lua_setfield(L, -2, "force");
	lua_setfield(L, -2, "monster_speed_param");
	lua_pop(L, 1);
}
