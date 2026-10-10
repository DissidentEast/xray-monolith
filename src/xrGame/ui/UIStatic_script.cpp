#include "pch_script.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)

void CUIStatic::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CUILines>("CUILines")
		.addFunction("SetFont", &CUILines::SetFont)
		.addFunction("SetText", &CUILines::SetText)
		.addFunction("SetTextST", &CUILines::SetTextST)
		.addFunction("GetText", &CUILines::GetText)
		.addFunction("SetElipsis", &CUILines::SetEllipsis)
		.addFunction("SetTextColor", &CUILines::SetTextColor)
		.endClass()


		.deriveClass<CUIStatic, CUIWindow>("CUIStatic")
		.addConstructor<void(*)()>()
		.addFunction("SetTextureColor", &CUIStatic::SetTextureColor)
		.addFunction("GetTextureColor", &CUIStatic::GetTextureColor)
		.addFunction("AdjustHeightToText", &CUIStatic::AdjustHeightToText)
		.addFunction("AdjustWidthToText", &CUIStatic::AdjustWidthToText)
		.addFunction("GetStretchTexture", &CUIStatic::GetStretchTexture)
		.addFunction("TextControl", &CUIStatic::TextItemControl)
		.addFunction("InitTexture", &CUIStatic::InitTexture)
		.addFunction("InitTextureEx", &CUIStatic::InitTextureEx)
		.addFunction("SetTextureRect", &CUIStatic::SetTextureRect_script)
		.addFunction("SetStretchTexture", &CUIStatic::SetStretchTexture)
		.addFunction("SetCoverTexture", &CUIStatic::SetCoverTexture)
		.addFunction("GetCoverTexture", &CUIStatic::GetCoverTexture)
		.addFunction("GetTextureRect", &CUIStatic::GetTextureRect_script)
		.addFunction("EnableHeading", &CUIStatic::EnableHeading)
		.addFunction("GetHeading", &CUIStatic::GetHeading)
		.addFunction("SetHeading", &CUIStatic::SetHeading)
		.addFunction("SetConstHeading", &CUIStatic::SetConstHeading)
		.addFunction("GetConstHeading", &CUIStatic::GetConstHeading)
		.addFunction("SetColorAnimation", &CUIStatic::SetColorAnimation)
		.addFunction("ResetColorAnimation", &CUIStatic::ResetColorAnimation)
		.addFunction("RemoveColorAnimation", &CUIStatic::RemoveColorAnimation)
		.endClass()

		.deriveClass<CUITextWnd, CUIWindow>("CUITextWnd")
		.addConstructor<void(*)()>()
		.addFunction("AdjustHeightToText", &CUITextWnd::AdjustHeightToText)
		.addFunction("AdjustWidthToText", &CUITextWnd::AdjustWidthToText)
		.addFunction("SetText", &CUITextWnd::SetText)
		.addFunction("SetTextST", &CUITextWnd::SetTextST)
		.addFunction("GetText", &CUITextWnd::GetText)
		.addFunction("SetFont", &CUITextWnd::SetFont)
		.addFunction("GetFont", &CUITextWnd::GetFont)
		.addFunction("SetTextColor", &CUITextWnd::SetTextColor)
		.addFunction("GetTextColor", &CUITextWnd::GetTextColor)
		.addFunction("SetTextComplexMode", &CUITextWnd::SetTextComplexMode)
		.addFunction("SetTextAlignment", &CUITextWnd::SetTextAlignment)
		.addFunction("SetVTextAlignment", &CUITextWnd::SetVTextAlignment)
		.addFunction("SetEllipsis", &CUITextWnd::SetEllipsis)
		.addFunction("SetTextOffset", &CUITextWnd::SetTextOffset)
		.endClass()
		//		.addFunction("",					&CUITextWnd::)

		.deriveClass<CUISleepStatic, CUIStatic>("CUISleepStatic")
		.addConstructor<void(*)()>()
		.endClass();
}
