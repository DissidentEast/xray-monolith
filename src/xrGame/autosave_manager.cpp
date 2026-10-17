////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager.cpp
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "autosave_manager.h"

CAutosaveManager::CAutosaveManager()
{
	m_not_ready_count = 0;

	shedule.t_min = 5000;
	shedule.t_max = 5000;
	shedule_register();
}

CAutosaveManager::~CAutosaveManager()
{
	shedule_unregister();
}

float CAutosaveManager::shedule_Scale()
{
	return (.5f);
}

// The autosave body was disabled here since 2004 ("if (true) return;") and never came back:
// only the "level is busy" counter below is still used, by the code that postpones saving.
// shortcut: no autosave happens. Restore it here when someone wants autosave again, and drop
// this comment - the config keys autosave_interval and delay_autosave_interval are still in use
void CAutosaveManager::shedule_Update(u32 dt)
{
	PROF_EVENT("CAutosaveManager::shedule_Update");
	inherited::shedule_Update(dt);
}
