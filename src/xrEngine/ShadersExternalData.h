#pragma once

// Ã•Ã°Ã Ã­Ã¨Ã«Ã¨Ã¹Ã¥ Ã¢Ã­Ã¥Ã¸Ã­Ã¨Ãµ Ã¸Ã¥Ã©Ã¤Ã¥Ã°Ã­Ã»Ãµ Ã¯Ã Ã°Ã Ã¬Ã¥Ã²Ã°Ã®Ã¢, ÃªÃ®Ã²Ã®Ã°Ã»Ã¥ Ã·Ã¨Ã²Ã Ã¾Ã²Ã±Ã¿ Ã¢ Blender_Recorder_StandartBinding.cpp
class ShadersExternalData //--#SM+#--
{
public:
	Fmatrix m_script_params; // ÃŒÃ Ã²Ã°Ã¨Ã¶Ã , Ã§Ã­Ã Ã·Ã¥Ã­Ã¨Ã¿ ÃªÃ®Ã²Ã®Ã°Ã®Ã© Ã¤Ã®Ã±Ã²Ã³Ã¯Ã­Ã» Ã¨Ã§ Lua
	Fvector4 hud_params;     // [zoom_rotate_factor, secondVP_zoom_factor, hud_fov, NULL] - ÃÃ Ã°Ã Ã¬Ã¥Ã²Ã°Ã» ÃµÃ³Ã¤Ã  Ã®Ã°Ã³Ã¦Ã¨Ã¿
	Fvector4 hud_fov_params; // [scope_zoom_factor, min_scope_zoom_factor, nullptr, NULL]
	Fvector4 m_blender_mode; // x\y = [0 - default, 1 - night vision, 2 - thermo vision, ... Ã±Ã¬. common.h] - ÃÃ¥Ã¦Ã¨Ã¬Ã» Ã°Ã¥Ã­Ã¤Ã¥Ã°Ã¨Ã­Ã£Ã 
							 // x - Ã®Ã±Ã­Ã®Ã¢Ã­Ã®Ã© Ã¢Ã¼Ã¾Ã¯Ã®Ã°Ã², y - Ã¢Ã²Ã®Ã°Ã®Ã© Ã¢Ã¼Ã¾Ã¯Ã®Ã°Ã², z = ?, w = [0 - Ã¨Ã¤Â¸Ã² Ã°Ã¥Ã­Ã¤Ã¥Ã° Ã®Ã¡Ã»Ã·Ã­Ã®Ã£Ã® Ã®Ã¡ÃºÃ¥ÃªÃ²Ã , 1 - Ã¨Ã¤Â¸Ã² Ã°Ã¥Ã­Ã¤Ã¥Ã° Ã¤Ã¥Ã²Ã Ã«Ã¼Ã­Ã»Ãµ Ã®Ã¡ÃºÃ¥ÃªÃ²Ã®Ã¢ (Ã²Ã°Ã Ã¢Ã , Ã¬Ã³Ã±Ã®Ã°)]

	ShadersExternalData()
	{
		m_script_params = Fmatrix();
		hud_params.set(0.f, 0.f, 0.f, 0.f);
		hud_fov_params.set(0.f, 0.f, 0.f, 0.f);
		m_blender_mode.set(0.f, 0.f, 0.f, 0.f);
	}
}; 
