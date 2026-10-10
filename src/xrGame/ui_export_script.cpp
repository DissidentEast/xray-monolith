#include "pch_script.h"

#include "script_ui_registrator.h"
#include "MainMenu.h"

#include "UIGameCustom.h"
#include "UI/UIScriptWnd.h"
#include "UI/UIButton.h"
#include "UI/UIProgressBar.h"
#include "UI/UIEditBox.h"
#include "UI/UIMessageBox.h"
#include "UI/UIPropertiesBox.h"
#include "UI/UITabControl.h"
#include "UI/UIComboBox.h"
#include "ui/UIOptionsManagerScript.h"
#include "ui/UIMapInfo.h"
#include "ScriptXmlInit.h"
#include "ui/UIActorMenu.h"

#include "login_manager.h"
#include "account_manager.h"
#include "profile_store.h"

#include "LuaBridge/LuaBridge.h"

CMainMenu* MainMenu();

#pragma optimize("s",on)
void UIRegistrator::script_register(lua_State* L)
{
	CUIWindow::script_register(L);
	CUIStatic::script_register(L);
	CUIButton::script_register(L);
	CUIProgressBar::script_register(L);
	CUIComboBox::script_register(L);
	CUIEditBox::script_register(L);
	CUITabControl::script_register(L);
	CUIMessageBox::script_register(L);
	CUIListBox::script_register(L);
	CUIDialogWndEx::script_register(L);
	CUIPropertiesBox::script_register(L);
	CUIOptionsManagerScript::script_register(L);
	CUIMapInfo::script_register(L);
	CScriptXmlInit::script_register(L);
	CUIGameCustom::script_register(L);
	CUIActorMenu::script_register(L);

	luabridge::getGlobalNamespace(L)
		.beginClass<CGameFont>("CGameFont")
		.endClass()

		.beginClass<Patch_Dawnload_Progress>("Patch_Dawnload_Progress")
		.addFunction("GetInProgress", &Patch_Dawnload_Progress::GetInProgress)
		.addFunction("GetStatus", &Patch_Dawnload_Progress::GetStatus)
		.addFunction("GetFlieName", &Patch_Dawnload_Progress::GetFlieName)
		.addFunction("GetProgress", &Patch_Dawnload_Progress::GetProgress)
		.endClass()

		.beginClass<CMainMenu>("CMainMenu")
		.addFunction("GetPatchProgress", &CMainMenu::GetPatchProgress)
		.addFunction("CancelDownload", &CMainMenu::CancelDownload)
		.addFunction("ValidateCDKey", &CMainMenu::ValidateCDKey)
		.addFunction("GetGSVer", &CMainMenu::GetGSVer)
		.addFunction("GetCDKey", &CMainMenu::GetCDKeyFromRegistry)
		.addFunction("GetPlayerName", &CMainMenu::GetPlayerName)
		.addFunction("GetDemoInfo", &CMainMenu::GetDemoInfo)
		//.addFunction("GetLoginMngr",			&CMainMenu::GetLoginMngr)
		//.addFunction("GetAccountMngr",			&CMainMenu::GetAccountMngr)
		//.addFunction("GetProfileStore",			&CMainMenu::GetProfileStore)
		.endClass();

	lua_getglobal(L, "CGameFont");
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, int(CGameFont::alLeft));	lua_setfield(L, -2, "alLeft");
	lua_pushinteger(L, int(CGameFont::alRight));	lua_setfield(L, -2, "alRight");
	lua_pushinteger(L, int(CGameFont::alCenter));	lua_setfield(L, -2, "alCenter");
	lua_setfield(L, -2, "EAligment");
	lua_pop(L, 1);

	luabridge::getGlobalNamespace(L)
		.beginNamespace("main_menu")
		.addFunction("get_main_menu", &MainMenu)
		.endNamespace();
}
