#ifndef UI_H
#define UI_H

#include <stdint.h>

#ifdef _WIN32
#define CONTROLLER_API __declspec(dllexport)
#else
#define CONTROLLER_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    CONTROLLER_API void controller_free_string(char* value);
    CONTROLLER_API char* controller_get_available_languages(void);
    CONTROLLER_API char* controller_get_configuration(void);
    CONTROLLER_API char* controller_get_discussions_url(void);
    CONTROLLER_API char* controller_get_issues_url(void);
    CONTROLLER_API char* controller_get_repo_url(void);
    CONTROLLER_API void controller_init(const char* resource_dir);
    CONTROLLER_API void controller_set_configuration(const char* value);
    CONTROLLER_API char* controller_tr(const char* msgid);
    CONTROLLER_API char* controller_trn(const char* msgid, const char* msgid_plural, uint64_t n);
    CONTROLLER_API char* controller_trp(const char* context, const char* msgid);
    CONTROLLER_API char* controller_trpn(const char* context, const char* msgid, const char* msgid_plural,
                                         uint64_t n);
#ifdef __cplusplus
}
#endif

#endif
