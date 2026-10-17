////////////////////////////////////////////////////////////////////////////
//	Module 		: autosave_manager_inline.h
//	Created 	: 04.11.2004
//  Modified 	: 04.11.2004
//	Author		: Dmitriy Iassenev
//	Description : Autosave manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC u32 CAutosaveManager::not_ready_count() const
{
	return (m_not_ready_count);
}

IC void CAutosaveManager::inc_not_ready()
{
	++m_not_ready_count;
}

IC void CAutosaveManager::dec_not_ready()
{
	VERIFY(m_not_ready_count);
	--m_not_ready_count;
}

IC bool CAutosaveManager::ready_for_autosave()
{
	return (!m_not_ready_count);
}
