#include "pch_script.h"
#include "UIWindow.h"
#include "UIDialogHolder.h"
#include "UITextureMaster.h"
#include "../GamePersistent.h"
#include "../ScriptXMLInit.h"
#include "../UICursor.h"
#include "ServerList.h"
#include "UI3tButton.h"
#include "UIActorMenu.h"
#include "UIAnimatedStatic.h"
#include "UIButton.h"
#include "UICheckButton.h"
#include "UIComboBox.h"
#include "UICustomEdit.h"
#include "UICustomSpin.h"
#include "UIDialogWnd.h"
#include "UIEditBox.h"
#include "UIFrameLineWnd.h"
#include "UIFrameWindow.h"
#include "UIHint.h"
#include "UIHudStatesWnd.h"
#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIListBoxItemMsgChain.h"
#include "UIMainIngameWnd.h"
#include "UIMapInfo.h"
#include "UIMapList.h"
#include "UIMessageBox.h"
#include "UIMessageBoxEx.h"
#include "UIMessagesWindow.h"
#include "UIMMShniaga.h"
#include "UIMotionIcon.h"
#include "UIPdaWnd.h"
#include "UIProgressBar.h"
#include "UIPropertiesBox.h"
#include "UIScriptWnd.h"
#include "UIScrollView.h"
#include "UISpinNum.h"
#include "UISpinText.h"
#include "UIStatic.h"
#include "UITabButton.h"
#include "UITabControl.h"
#include "UITrackBar.h"

CFontManager& mngr()
{
	return UI().Font();
}

// hud font
CGameFont* GetFontSmall()
{
	return mngr().pFontStat;
}

CGameFont* GetFontMedium()
{
	return mngr().pFontMedium;
}

CGameFont* GetFontDI()
{
	return mngr().pFontDI;
}

//шрифты для интерфейса
CGameFont* GetFontGraffiti19Russian()
{
	return mngr().pFontGraffiti19Russian;
}

CGameFont* GetFontGraffiti22Russian()
{
	return mngr().pFontGraffiti22Russian;
}

CGameFont* GetFontLetterica16Russian()
{
	return mngr().pFontLetterica16Russian;
}

CGameFont* GetFontLetterica18Russian()
{
	return mngr().pFontLetterica18Russian;
}

CGameFont* GetFontGraffiti32Russian()
{
	return mngr().pFontGraffiti32Russian;
}

CGameFont* GetFontGraffiti50Russian()
{
	return mngr().pFontGraffiti50Russian;
}

CGameFont* GetFontLetterica25()
{
	return mngr().pFontLetterica25;
}


int GetARGB(u16 a, u16 r, u16 g, u16 b)
{
	return color_argb(a, r, g, b);
}

int ClrGetA(u32 argb)
{
	return color_get_A(argb);
}

int ClrGetR(u32 argb)
{
	return color_get_R(argb);
}

int ClrGetG(u32 argb)
{
	return color_get_G(argb);
}

int ClrGetB(u32 argb)
{
	return color_get_B(argb);
}

int ClrSetA(u32 argb, u16 a)
{
	return subst_alpha(argb, a);
};

int ClrSetR(u32 argb, u16 r)
{
	return subst_red(argb, r);
};

int ClrSetG(u32 argb, u16 g)
{
	return subst_green(argb, g);
};

int ClrSetB(u32 argb, u16 b)
{
	return subst_blue(argb, b);
};

const Fvector2* get_wnd_pos(CUIWindow* w)
{
	return &w->GetWndPos();
}

Fvector2 GetCursorPosition_script()
{
	return GetUICursor().GetCursorPosition();
}

void SetCursorPosition_script(Fvector2& pos)
{
	GetUICursor().SetUICursorPosition(pos);
}

template <typename T>
T* ui_window_cast(CUIWindow* window)
{
	return smart_cast<T*>(window);
}

#define UI_WINDOW_CAST(class_name) &ui_window_cast<class_name>

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CUIWindow::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.addFunction("GetARGB", &GetARGB)
		.addFunction("ClrGetA", &ClrGetA)
		.addFunction("ClrGetR", &ClrGetR)
		.addFunction("ClrGetG", &ClrGetG)
		.addFunction("ClrGetB", &ClrGetB)
		.addFunction("ClrSetA", &ClrSetA)
		.addFunction("ClrSetR", &ClrSetR)
		.addFunction("ClrSetG", &ClrSetG)
		.addFunction("ClrSetB", &ClrSetB)

		.addFunction("GetFontSmall", &GetFontSmall)
		.addFunction("GetFontMedium", &GetFontMedium)
		.addFunction("GetFontDI", &GetFontDI)
		.addFunction("GetFontGraffiti19Russian", &GetFontGraffiti19Russian)
		.addFunction("GetFontGraffiti22Russian", &GetFontGraffiti22Russian)
		.addFunction("GetFontLetterica16Russian", &GetFontLetterica16Russian)
		.addFunction("GetFontLetterica18Russian", &GetFontLetterica18Russian)
		.addFunction("GetFontGraffiti32Russian", &GetFontGraffiti32Russian)
		.addFunction("GetFontGraffiti50Russian", &GetFontGraffiti50Russian)
		.addFunction("GetFontLetterica25", &GetFontLetterica25)
		.addFunction("GetCursorPosition", &GetCursorPosition_script)
		.addFunction("SetCursorPosition", &SetCursorPosition_script)
		.addFunction("FitInRect", &fit_in_rect)

		.beginClass<CUIWindow>("CUIWindow")
		.addConstructor<void(*)()>()
		.addFunction("AttachChild", &CUIWindow::AttachChild)
		.addFunction("AttachChildKeepOwner", &CUIWindow::AttachChild)
		.addFunction("DetachChild", &CUIWindow::DetachChild)
		.addFunction("FindChild", &CUIWindow::FindChild)
		.addFunction("SetAutoDelete", &CUIWindow::SetAutoDelete)
		.addFunction("IsAutoDelete", &CUIWindow::IsAutoDelete)

		.addFunction("IsCursorOverWindow", &CUIWindow::CursorOverWindow)
		.addFunction("FocusReceiveTime", &CUIWindow::FocusReceiveTime)
		.addFunction("GetAbsoluteRect", &CUIWindow::GetAbsoluteRect)

		.addFunction("SetWndRect", (void (CUIWindow::*)(Frect))&CUIWindow::SetWndRect_script)
		.addFunction("SetWndPos", (void (CUIWindow::*)(Fvector2))&CUIWindow::SetWndPos_script)
		.addFunction("SetWndSize", (void (CUIWindow::*)(Fvector2))&CUIWindow::SetWndSize_script)
		.addFunction("GetWndPos", &get_wnd_pos)
		.addFunction("GetWidth", &CUIWindow::GetWidth)
		.addFunction("GetHeight", &CUIWindow::GetHeight)

		.addFunction("Enable", &CUIWindow::Enable)
		.addFunction("IsEnabled", &CUIWindow::IsEnabled)
		.addFunction("Show", &CUIWindow::Show)
		.addFunction("IsShown", &CUIWindow::IsShown)

		.addFunction("WindowName", &CUIWindow::WindowName_script)
		.addFunction("SetWindowName", &CUIWindow::SetWindowName)
		.addFunction("SetPPMode", &CUIWindow::SetPPMode)
		.addFunction("ResetPPMode", &CUIWindow::ResetPPMode)

		.addFunction("cast_3tButton", UI_WINDOW_CAST(CUI3tButton))
		.addFunction("cast_ActorMenu", UI_WINDOW_CAST(CUIActorMenu))
		.addFunction("cast_Button", UI_WINDOW_CAST(CUIButton))
		.addFunction("cast_CheckButton", UI_WINDOW_CAST(CUICheckButton))
		.addFunction("cast_ComboBox", UI_WINDOW_CAST(CUIComboBox))
		.addFunction("cast_CustomEdit", UI_WINDOW_CAST(CUICustomEdit))
		.addFunction("cast_CustomSpin", UI_WINDOW_CAST(CUICustomSpin))
		.addFunction("cast_DialogWnd", UI_WINDOW_CAST(CUIDialogWnd))
		.addFunction("cast_EditBox", UI_WINDOW_CAST(CUIEditBox))
		.addFunction("cast_FrameLineWnd", UI_WINDOW_CAST(CUIFrameLineWnd))
		.addFunction("cast_FrameWindow", UI_WINDOW_CAST(CUIFrameWindow))
		.addFunction("cast_Hint", UI_WINDOW_CAST(UIHint))
		.addFunction("cast_HudStatesWnd", UI_WINDOW_CAST(CUIHudStatesWnd))
		.addFunction("cast_ListBox", UI_WINDOW_CAST(CUIListBox))
		.addFunction("cast_ListBoxItem", UI_WINDOW_CAST(CUIListBoxItem))
		.addFunction("cast_ListBoxItemMsgChain", UI_WINDOW_CAST(CUIListBoxItemMsgChain))
		.addFunction("cast_MMShniaga", UI_WINDOW_CAST(CUIMMShniaga))
		.addFunction("cast_MainIngameWnd", UI_WINDOW_CAST(CUIMainIngameWnd))
		.addFunction("cast_MapInfo", UI_WINDOW_CAST(CUIMapInfo))
		.addFunction("cast_MapList", UI_WINDOW_CAST(CUIMapList))
		.addFunction("cast_MessageBox", UI_WINDOW_CAST(CUIMessageBox))
		.addFunction("cast_MessageBoxEx", UI_WINDOW_CAST(CUIMessageBoxEx))
		.addFunction("cast_MessagesWindow", UI_WINDOW_CAST(CUIMessagesWindow))
		.addFunction("cast_MotionIcon", UI_WINDOW_CAST(CUIMotionIcon))
		.addFunction("cast_PdaWnd", UI_WINDOW_CAST(CUIPdaWnd))
		.addFunction("cast_ProgressBar", UI_WINDOW_CAST(CUIProgressBar))
		.addFunction("cast_PropertiesBox", UI_WINDOW_CAST(CUIPropertiesBox))
		.addFunction("cast_ScriptWnd", UI_WINDOW_CAST(CUIDialogWndEx))
		.addFunction("cast_ScrollView", UI_WINDOW_CAST(CUIScrollView))
		.addFunction("cast_ServerList", UI_WINDOW_CAST(CServerList))
		.addFunction("cast_SleepStatic", UI_WINDOW_CAST(CUISleepStatic))
		.addFunction("cast_SpinFlt", UI_WINDOW_CAST(CUISpinFlt))
		.addFunction("cast_SpinNum", UI_WINDOW_CAST(CUISpinNum))
		.addFunction("cast_SpinText", UI_WINDOW_CAST(CUISpinText))
		.addFunction("cast_Static", UI_WINDOW_CAST(CUIStatic))
		.addFunction("cast_TabButton", UI_WINDOW_CAST(CUITabButton))
		.addFunction("cast_TabControl", UI_WINDOW_CAST(CUITabControl))
		.addFunction("cast_TextWnd", UI_WINDOW_CAST(CUITextWnd))
		.addFunction("cast_TrackBar", UI_WINDOW_CAST(CUITrackBar))
		.endClass();

	luabridge::getGlobalNamespace(L)
		.beginClass<CDialogHolder>("CDialogHolder")
		.addFunction("AddDialogToRender", &CDialogHolder::AddDialogToRender)
		.addFunction("RemoveDialogToRender", &CDialogHolder::RemoveDialogToRender)
		.endClass()

		.deriveClass<CUIDialogWnd, CUIWindow>("CUIDialogWnd")
		.addFunction("ShowDialog", &CUIDialogWnd::ShowDialog)
		.addFunction("HideDialog", &CUIDialogWnd::HideDialog)
		.addFunction("GetHolder", &CUIDialogWnd::GetHolder)
		.addFunction("AllowMovement", &CUIDialogWnd::AllowMovement)
		.addFunction("AllowCursor", &CUIDialogWnd::AllowCursor)
		.addFunction("AllowCenterCursor", &CUIDialogWnd::AllowCenterCursor)
		.addFunction("AllowWorkInPause", &CUIDialogWnd::AllowWorkInPause)
		.endClass()

		.deriveClass<CUIFrameWindow, CUIWindow>("CUIFrameWindow")
		.addConstructor<void(*)()>()
		.addFunction("SetWidth", &CUIFrameWindow::SetWidth)
		.addFunction("SetHeight", &CUIFrameWindow::SetHeight)
		.addFunction("SetColor", &CUIFrameWindow::SetTextureColor)
		.endClass()

		.deriveClass<CUIFrameLineWnd, CUIWindow>("CUIFrameLineWnd")
		.addConstructor<void(*)()>()
		.addFunction("SetWidth", &CUIFrameLineWnd::SetWidth)
		.addFunction("SetHeight", &CUIFrameLineWnd::SetHeight)
		.addFunction("SetColor", &CUIFrameLineWnd::SetTextureColor)
		.endClass()

		.deriveClass<UIHint, CUIWindow>("UIHint")
		.addConstructor<void(*)()>()
		.addFunction("SetWidth", &UIHint::SetWidth)
		.addFunction("SetHeight", &UIHint::SetHeight)
		.addFunction("SetHintText", &UIHint::set_text)
		.addFunction("GetHintText", &UIHint::get_text)
		.endClass()

		.deriveClass<CUIMMShniaga, CUIWindow>("CUIMMShniaga")
		.addFunction("SetVisibleMagnifier", &CUIMMShniaga::SetVisibleMagnifier)
		.addFunction("SetPage", &CUIMMShniaga::SetPage)
		.addFunction("ShowPage", &CUIMMShniaga::ShowPage)
		.endClass()

		.deriveClass<CUIScrollView, CUIWindow>("CUIScrollView")
		.addConstructor<void(*)()>()
		.addFunction("AddWindow", &CUIScrollView::AddWindow)
		.addFunction("RemoveWindow", &CUIScrollView::RemoveWindow)
		.addFunction("Clear", &CUIScrollView::Clear)
		.addFunction("ScrollToBegin", &CUIScrollView::ScrollToBegin)
		.addFunction("ScrollToEnd", &CUIScrollView::ScrollToEnd)
		.addFunction("GetMinScrollPos", &CUIScrollView::GetMinScrollPos)
		.addFunction("GetMaxScrollPos", &CUIScrollView::GetMaxScrollPos)
		.addFunction("GetCurrentScrollPos", &CUIScrollView::GetCurrentScrollPos)
		.addFunction("SetFixedScrollBar", &CUIScrollView::SetFixedScrollBar)
		.addFunction("SetScrollPos", &CUIScrollView::SetScrollPos)
		.endClass()

		.beginClass<enum_exporter<EUIMessages>>("ui_events")
		.endClass();

	lua_getglobal(L, "CUIMMShniaga");
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, CUIMMShniaga::epi_main);	lua_setfield(L, -2, "epi_main");
	lua_pushinteger(L, CUIMMShniaga::epi_new_game);	lua_setfield(L, -2, "epi_new_game");
	lua_pushinteger(L, CUIMMShniaga::epi_new_network_game);	lua_setfield(L, -2, "epi_new_network_game");
	lua_setfield(L, -2, "enum_page_id");
	lua_pop(L, 1);

	lua_getglobal(L, "ui_events");
	lua_createtable(L, 0, 34);
	lua_pushinteger(L, int(WINDOW_LBUTTON_DOWN));	lua_setfield(L, -2, "WINDOW_LBUTTON_DOWN");
	lua_pushinteger(L, int(WINDOW_RBUTTON_DOWN));	lua_setfield(L, -2, "WINDOW_RBUTTON_DOWN");
	lua_pushinteger(L, int(WINDOW_LBUTTON_UP));	lua_setfield(L, -2, "WINDOW_LBUTTON_UP");
	lua_pushinteger(L, int(WINDOW_RBUTTON_UP));	lua_setfield(L, -2, "WINDOW_RBUTTON_UP");
	lua_pushinteger(L, int(WINDOW_MOUSE_MOVE));	lua_setfield(L, -2, "WINDOW_MOUSE_MOVE");
	lua_pushinteger(L, int(WINDOW_MOUSE_WHEEL_UP));	lua_setfield(L, -2, "WINDOW_MOUSE_WHEEL_UP");
	lua_pushinteger(L, int(WINDOW_MOUSE_WHEEL_DOWN));	lua_setfield(L, -2, "WINDOW_MOUSE_WHEEL_DOWN");
	lua_pushinteger(L, int(WINDOW_LBUTTON_DB_CLICK));	lua_setfield(L, -2, "WINDOW_LBUTTON_DB_CLICK");
	lua_pushinteger(L, int(WINDOW_KEY_PRESSED));	lua_setfield(L, -2, "WINDOW_KEY_PRESSED");
	lua_pushinteger(L, int(WINDOW_KEY_RELEASED));	lua_setfield(L, -2, "WINDOW_KEY_RELEASED");
	lua_pushinteger(L, int(WINDOW_KEYBOARD_CAPTURE_LOST));	lua_setfield(L, -2, "WINDOW_KEYBOARD_CAPTURE_LOST");
	lua_pushinteger(L, int(BUTTON_CLICKED));	lua_setfield(L, -2, "BUTTON_CLICKED");
	lua_pushinteger(L, int(BUTTON_DOWN));	lua_setfield(L, -2, "BUTTON_DOWN");
	lua_pushinteger(L, int(TAB_CHANGED));	lua_setfield(L, -2, "TAB_CHANGED");
	lua_pushinteger(L, int(CHECK_BUTTON_SET));	lua_setfield(L, -2, "CHECK_BUTTON_SET");
	lua_pushinteger(L, int(CHECK_BUTTON_RESET));	lua_setfield(L, -2, "CHECK_BUTTON_RESET");
	lua_pushinteger(L, int(RADIOBUTTON_SET));	lua_setfield(L, -2, "RADIOBUTTON_SET");
	lua_pushinteger(L, int(SCROLLBOX_MOVE));	lua_setfield(L, -2, "SCROLLBOX_MOVE");
	lua_pushinteger(L, int(SCROLLBAR_VSCROLL));	lua_setfield(L, -2, "SCROLLBAR_VSCROLL");
	lua_pushinteger(L, int(SCROLLBAR_HSCROLL));	lua_setfield(L, -2, "SCROLLBAR_HSCROLL");
	lua_pushinteger(L, int(LIST_ITEM_CLICKED));	lua_setfield(L, -2, "LIST_ITEM_CLICKED");
	lua_pushinteger(L, int(LIST_ITEM_SELECT));	lua_setfield(L, -2, "LIST_ITEM_SELECT");
	lua_pushinteger(L, int(PROPERTY_CLICKED));	lua_setfield(L, -2, "PROPERTY_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_OK_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_OK_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_YES_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_YES_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_NO_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_NO_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_CANCEL_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_CANCEL_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_COPY_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_COPY_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_QUIT_GAME_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_QUIT_GAME_CLICKED");
	lua_pushinteger(L, int(MESSAGE_BOX_QUIT_WIN_CLICKED));	lua_setfield(L, -2, "MESSAGE_BOX_QUIT_WIN_CLICKED");
	lua_pushinteger(L, int(EDIT_TEXT_COMMIT));	lua_setfield(L, -2, "EDIT_TEXT_COMMIT");
	lua_pushinteger(L, int(MAIN_MENU_RELOADED));	lua_setfield(L, -2, "MAIN_MENU_RELOADED");
	lua_setfield(L, -2, "events");
	lua_pop(L, 1);
}

#undef UI_WINDOW_CAST
