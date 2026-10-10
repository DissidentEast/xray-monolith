#include "pch_script.h"
#include "UIButton.h"
#include "UI3tButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UISpinNum.h"
#include "UISpinText.h"
#include "UITrackBar.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void CUIButton::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.deriveClass<CUIButton, CUIStatic>("CUIButton")
		.addConstructor<void(*)()>()
		.endClass()

		.deriveClass<CUI3tButton, CUIButton>("CUI3tButton")
		.addConstructor<void(*)()>()
		.endClass()


		.deriveClass<CUICheckButton, CUI3tButton>("CUICheckButton")
		.addConstructor<void(*)()>()
		.addFunction("GetCheck", &CUICheckButton::GetCheck)
		.addFunction("SetCheck", &CUICheckButton::SetCheck)
		.addFunction("SetDependControl", &CUICheckButton::SetDependControl)
		.endClass()

		.deriveClass<CUICustomSpin, CUIWindow>("CUICustomSpin")
		.addFunction("GetText", &CUICustomSpin::GetText)
		.endClass()

		.deriveClass<CUISpinNum, CUICustomSpin>("CUISpinNum")
		.addConstructor<void(*)()>()
		.endClass()

		.deriveClass<CUISpinFlt, CUICustomSpin>("CUISpinFlt")
		.addConstructor<void(*)()>()
		.endClass()

		.deriveClass<CUISpinText, CUICustomSpin>("CUISpinText")
		.addConstructor<void(*)()>()
		.endClass()

		.deriveClass<CUITrackBar, CUIWindow>("CUITrackBar")
		.addConstructor<void(*)()>()
		.addFunction("GetCheck", &CUITrackBar::GetCheck)
		.addFunction("SetCheck", &CUITrackBar::SetCheck)
		.addFunction("GetIValue", &CUITrackBar::GetIValue)
		.addFunction("GetFValue", &CUITrackBar::GetFValue)
		.addFunction("SetIValue", &CUITrackBar::SetIValue)
		.addFunction("SetFValue", &CUITrackBar::SetFValue)
		.addFunction("SetStep", &CUITrackBar::SetStep)
		.addFunction("GetInvert", &CUITrackBar::GetInvert)
		.addFunction("SetInvert", &CUITrackBar::SetInvert)
		.addFunction("SetOptIBounds", &CUITrackBar::SetOptIBounds)
		.addFunction("SetOptFBounds", &CUITrackBar::SetOptFBounds)
		.addFunction("SetCurrentValue", &CUITrackBar::SetCurrentOptValue)
		.endClass();
}
