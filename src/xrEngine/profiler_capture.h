#pragma once

// Scripted baseline capture for the Optick backend (ProfiledDX11 only).
// Armed via "-profcapture <frames> <file>" on the command line or the
// "prof_capture <frames> <file>" console command. Frame counting starts on
// the first frame with a loaded, unpaused level, so menu and loading
// hitches never pollute the baseline. Manual control: prof_start,
// prof_stop, prof_save <file>.
extern int g_prof_capture_armed; // frames still to capture, 0 = idle
extern xr_string g_prof_capture_file;

// Deferred "-start ..." command line (client only): executed once the
// device has pumped enough menu frames. Set in x_ray.cpp, consumed below.
extern xr_string g_deferred_startup_cmd;

// Call once per frame, right after PROF_FRAME(). No-op unless armed.
void profiler_capture_on_frame();
