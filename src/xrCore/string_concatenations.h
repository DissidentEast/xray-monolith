//////////////////////////////////////////////////////////
// Desc   : String concat system
// Author : Giperion
//////////////////////////////////////////////////////////
// Oxygen Engine 2.0 - 2016-2019(c)
//////////////////////////////////////////////////////////

#pragma once


LPSTR XRCORE_API strconcat(int dest_sz, char* dest, const char* S1, const char* S2);
LPSTR XRCORE_API strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3);
LPSTR XRCORE_API strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4);
LPSTR XRCORE_API strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4,
                           const char* S5);
LPSTR XRCORE_API strconcat(int dest_sz, char* dest, const char* S1, const char* S2, const char* S3, const char* S4,
                           const char* S5, const char* S6);


bool XRCORE_API _strconcatSingle(char*& destPtr, char* pDestEnd, const char* src);

// warning: do not comment this macro, as stack overflow check is very light
// (consumes ~1% performance of STRCONCAT macro)
#define STRCONCAT_STACKOVERFLOW_CHECK

#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#define STRCONCAT(dest, ...) \
 do { \
 xray::core::detail::string_tupples STRCONCAT_tupples_unique_identifier(__VA_ARGS__); \
 u32 STRCONCAT_buffer_size = STRCONCAT_tupples_unique_identifier.size(); \
 xray::core::detail::check_stack_overflow(STRCONCAT_buffer_size); \
 (dest) = (LPSTR)_alloca(STRCONCAT_buffer_size); \
 STRCONCAT_tupples_unique_identifier.concat (dest); \
  } while (0)

#else //#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#define STRCONCAT(dest, ...) \
 do { \
 xray::core::detail::string_tupples STRCONCAT_tupples_unique_identifier(__VA_ARGS__); \
 (dest) = (LPSTR)_alloca(STRCONCAT_tupples_unique_identifier.size()); \
 STRCONCAT_tupples_unique_identifier.concat (dest); \
  } while (0)

#endif //#ifdef STRCONCAT_STACKOVERFLOW_CHECK

#include "string_concatenations_inline.h"

// Giperion XRay Oxygen - ultimate version of strconcat
template<typename StringReceiverType, typename... ArgList>
char* xr_strconcat(StringReceiverType& receiver, ArgList... args)
{
    static_assert(std::is_array< StringReceiverType>::value); // must be array...
    static_assert(std::is_same<typename std::remove_extent< StringReceiverType>::type, char>::value); // ... of chars

    char* pStrCursor = &receiver[0];
    char* pStrEnd = &receiver[0] + sizeof(StringReceiverType);
    const char* errorSource = nullptr;

    // Use a lambda to process args and catch the first one that fails
    auto process = [&](const char* s)
    {
        if (!errorSource && !_strconcatSingle(pStrCursor, pStrEnd, s))
        {
            errorSource = s;
        }
    };

    // Fold expression (C++17) or initializer list trick to process args
    (process(args), ...);

    // If errorSource is set, one of the strings was truncated
    if (errorSource)
    {
        Debug.fatal(DEBUG_INFO,
            "String Buffer Overflow!\n"
            "Max Capacity: %zu bytes\n"
            "Failed at string: \"%s\"\n"
            "Current buffer state: \"%s\"",
            sizeof(StringReceiverType), errorSource, &receiver[0]);
    }

    *pStrCursor = '\0'; // Guaranteed safe because of the -1 check in _strconcatSingle
    return &receiver[0];
}
