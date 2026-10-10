////////////////////////////////////////////////////////////////////////////
//	Module 		: script_render_device_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script render device script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "script_render_device.h"

#include "LuaBridge/LuaBridge.h"

bool is_device_paused(CRenderDevice* d)
{
	return !!Device.Paused();
}

void set_device_paused(CRenderDevice* d, bool b)
{
	Device.Pause(b, TRUE, FALSE, "set_device_paused_script");
}

extern ENGINE_API BOOL bShowPauseString;

void set_device_paused_ex(CRenderDevice* d, bool b)
{
	Device.Pause(b, TRUE, TRUE, "set_device_paused_ex_script");
	bShowPauseString = FALSE;
}

extern ENGINE_API BOOL g_appLoaded;

bool is_app_ready()
{
	return !!g_appLoaded;
}

u32 time_global(const CRenderDevice* self)
{
	THROW(self);
	return (self->dwTimeGlobal);
}

u32 time_continual(const CRenderDevice* self)
{
	THROW(self);
	return (self->dwTimeContinual);
}

#pragma optimize("s",on)
void CScriptRenderDevice::script_register(lua_State* L)
{
	luabridge::getGlobalNamespace(L)
		.beginClass<CRenderDevice>("render_device")
			.addProperty("width", &CRenderDevice::dwWidth)
			.addProperty("height", &CRenderDevice::dwHeight)
			.addProperty("time_delta", &CRenderDevice::dwTimeDelta)
			.addProperty("f_time_delta", &CRenderDevice::fTimeDelta)
			.addProperty("cam_pos", &CRenderDevice::vCameraPosition)
			.addProperty("cam_dir", &CRenderDevice::vCameraDirection)
			.addProperty("cam_top", &CRenderDevice::vCameraTop)
			.addProperty("cam_right", &CRenderDevice::vCameraRight)
			//			.addProperty("view",					&CRenderDevice::mView)
			//			.addProperty("projection",				&CRenderDevice::mProject)
			//			.addProperty("full_transform",			&CRenderDevice::mFullTransform)
			.addProperty("fov", &CRenderDevice::fFOV)
			.addProperty("aspect_ratio", &CRenderDevice::fASPECT)
			.addFunction("time_global", &time_global)
			.addFunction("time_continual", &time_continual)
			.addProperty("precache_frame", &CRenderDevice::dwPrecacheFrame)
			.addProperty("frame", &CRenderDevice::dwFrame)
			.addFunction("is_paused", &is_device_paused)
			.addFunction("pause", &set_device_paused)
			.addFunction("pause_ex", &set_device_paused_ex)
		.endClass()
		.addFunction("app_ready", &is_app_ready);
}
