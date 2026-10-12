#pragma once
// MsgFmt: C++20 <format>-backed logging.
// Additive and opt-in: existing Msg()/Log() are untouched. Only TUs that
// include this header pay for <format>.
//
// The format string is checked at COMPILE time (std::format_string), so a
// mismatched argument list is a build error, not a runtime crash. No EH
// unwinding is required on this path (spike-proven under /EHsc- with
// _CPPUNWIND absent), making it safe for Release configs.
//
// The temporary std::string outlives the Log() call: AddOne() in log.cpp
// copies the text synchronously into LogFile.
#include <version>
#ifdef __cpp_lib_format
#include <format>
#include <string_view>
#include "log.h"
#include "xrstring.h"

#ifndef M_NOSTDCONTAINERS_EXT
// xr_string uses the xalloc allocator, so std::format has no formatter for
// it. Format via string_view (no copy, no allocation).
template <>
struct std::formatter<xr_string> : std::formatter<std::string_view>
{
    auto format(const xr_string& s, auto& ctx) const
    {
        return std::formatter<std::string_view>::format(std::string_view(s.data(), s.size()), ctx);
    }
};
#endif

template <typename... Args>
inline void MsgFmt(std::format_string<Args...> fmt, Args&&... args)
{
    Log(std::format(fmt, std::forward<Args>(args)...).c_str());
}
#else
#error "log_format.h requires C++20 <format> support (/std:c++20)"
#endif
