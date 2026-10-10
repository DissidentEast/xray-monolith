#include "pch_script.h"
#include "UITabControl.h"
#include "UITabButton.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CUITabControl::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<CUITabControl, CUIWindow>("CUITabControl")
		.addConstructor<void(*)()>()
		.addFunction("AddItem", (bool (CUITabControl::*)(CUITabButton*))(&CUITabControl::AddItem), (bool (CUITabControl::*)(LPCSTR, LPCSTR, Fvector2, Fvector2))&CUITabControl::AddItem)
		.addFunction("RemoveAll", &CUITabControl::RemoveAll)
			.addFunction("AddTab", &CUITabControl::AddTab)
			.addFunction("SetTabIcon", &CUITabControl::SetTabIcon)
		.addFunction("SetTabIcon", &CUITabControl::SetTabIcon)
		.addFunction("RecalcScroll", &CUITabControl::RecalcScroll)
		.addFunction("GetActiveId", &CUITabControl::GetActiveId_script)
			.addFunction("GetTabsCount", [](CUITabControl const& self) { return self.GetTabsCount(); })
		.addFunction("SetActiveTab", &CUITabControl::SetActiveTab_script)
		.addFunction("GetButtonById", &CUITabControl::GetButtonById_script)
		.addFunction("GetEnabled", &CUITabControl::GetAcceleratorsMode)
		.addFunction("SetEnabled", &CUITabControl::SetAcceleratorsMode)
		.endClass()

		.deriveClass<CUITabButton, CUIButton>("CUITabButton")
		.addConstructor<void(*)()>()
		.endClass();
}
