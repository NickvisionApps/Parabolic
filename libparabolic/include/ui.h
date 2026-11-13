#ifndef UI_H
#define UI_H

#include <cstdint>

#ifdef _WIN32
#define CONTROLLER_API __declspec(dllexport)
#else
#define CONTROLLER_API __attribute__((visibility("default")))
#endif

extern "C"
{
    CONTROLLER_API void controller_free_string(char* value);
    CONTROLLER_API void controller_init(const char* locale_dir);
    CONTROLLER_API char* controller_tr(const char* msgid);
    CONTROLLER_API char* controller_trn(const char* msgid, const char* msgid_plural, std::uint64_t n);
    CONTROLLER_API char* controller_trp(const char* context, const char* msgid);
    CONTROLLER_API char* controller_trpn(const char* context, const char* msgid, const char* msgid_plural,
                                         std::uint64_t n);
}

#endif
