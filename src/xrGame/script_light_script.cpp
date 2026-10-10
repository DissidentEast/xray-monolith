#include "pch_script.h"
#include "script_light.h"

#include "LuaBridge/LuaBridge.h"

#pragma optimize("s",on)
void ScriptLight::script_register(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<ScriptLight>("script_light")
			.addConstructor<void(*)()>()
			.addFunction("set_position", (void (ScriptLight::*)(Fvector)) & ScriptLight::SetPosition, (void (ScriptLight::*)(float, float, float)) & ScriptLight::SetPosition)
			.addFunction("set_direction", (void (ScriptLight::*)(Fvector))(&ScriptLight::SetDirection), (void (ScriptLight::*)(float, float, float)) & ScriptLight::SetDirection, (void (ScriptLight::*)(Fvector, Fvector))(&ScriptLight::SetDirection))
			.addFunction("set_cone", &ScriptLight::SetCone)
			.addFunction("update", &ScriptLight::Update)
			.addProperty("color", [](ScriptLight const& self) { return self.GetColor(); }, &ScriptLight::SetColor)
			.addProperty("texture", [](ScriptLight const& self) { return self.GetTexture(); }, &ScriptLight::SetTexture)
			.addProperty("enabled", [](ScriptLight const& self) { return self.IsEnabled(); }, &ScriptLight::Enable)
			.addProperty("type", [](ScriptLight const& self) { return self.GetType(); }, &ScriptLight::SetType)
			.addProperty("range", [](ScriptLight const& self) { return self.GetRange(); }, &ScriptLight::SetRange)
			.addProperty("shadow", [](ScriptLight const& self) { return self.GetShadow(); }, &ScriptLight::SetShadow)
			.addProperty("lanim", [](ScriptLight const& self) { return self.GetLanim(); }, &ScriptLight::SetLanim)
			.addProperty("lanim_brightness", [](ScriptLight const& self) { return self.GetBrightness(); }, &ScriptLight::SetBrightness)
			.addProperty("volumetric", [](ScriptLight const& self) { return self.GetVolumetric(); }, &ScriptLight::SetVolumetric)
			.addProperty("volumetric_quality", [](ScriptLight const& self) { return self.GetVolumetricQuality(); }, &ScriptLight::SetVolumetricQuality)
			.addProperty("volumetric_distance", [](ScriptLight const& self) { return self.GetVolumetricDistance(); }, &ScriptLight::SetVolumetricDistance)
			.addProperty("volumetric_intensity", [](ScriptLight const& self) { return self.GetVolumetricIntensity(); }, &ScriptLight::SetVolumetricIntensity)
			.addProperty("hud_mode", [](ScriptLight const& self) { return self.GetHudMode(); }, &ScriptLight::SetHudMode)
		.endClass()

		.deriveClass<AttachmentScriptLight, ScriptLight>("attachment_script_light")
			.addConstructor<void(*)()>()
			.addFunction("set_position", (void (AttachmentScriptLight::*)(Fvector)) & AttachmentScriptLight::SetPosition, (void (AttachmentScriptLight::*)(float, float, float)) & AttachmentScriptLight::SetPosition)
			.addFunction("set_direction", (void (AttachmentScriptLight::*)(Fvector))(&AttachmentScriptLight::SetDirection), (void (AttachmentScriptLight::*)(float, float, float)) & AttachmentScriptLight::SetDirection, (void (AttachmentScriptLight::*)(Fvector, Fvector))(&AttachmentScriptLight::SetDirection))
		.endClass()

		.beginClass<ScriptGlow>("script_glow")
			.addConstructor<void(*)()>()
			.addFunction("set_position", (void (ScriptGlow::*)(Fvector)) & ScriptGlow::SetPosition, (void (ScriptGlow::*)(float, float, float)) & ScriptGlow::SetPosition)
			.addFunction("set_direction", (void (ScriptGlow::*)(Fvector))(&ScriptGlow::SetDirection), (void (ScriptGlow::*)(float, float, float)) & ScriptGlow::SetDirection)
			.addProperty("enabled", [](ScriptGlow const& self) { return self.IsEnabled(); }, &ScriptGlow::Enable)
			.addProperty("texture", [](ScriptGlow const& self) { return self.GetTexture(); }, &ScriptGlow::SetTexture)
			.addProperty("range", [](ScriptGlow const& self) { return self.GetRange(); }, &ScriptGlow::SetRange)
			.addProperty("color", [](ScriptGlow const& self) { return self.GetColor(); }, &ScriptGlow::SetColor)
			.addProperty("lanim", [](ScriptGlow const& self) { return self.GetLanim(); }, &ScriptGlow::SetLanim)
			.addProperty("lanim_brightness", [](ScriptGlow const& self) { return self.GetBrightness(); }, &ScriptGlow::SetBrightness)
		.endClass();
}