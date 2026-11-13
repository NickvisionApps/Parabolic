#include "ui.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include "application.h"

namespace
{
    std::unique_ptr<parabolic::application::State> state;

    char* duplicate(const char* value)
    {
        if (value == nullptr)
        {
            return nullptr;
        }
        const std::size_t size{ std::strlen(value) + 1 };
        char* result{ static_cast<char*>(std::malloc(size)) };
        if (result != nullptr)
        {
            std::memcpy(result, value, size);
        }
        return result;
    }
}

void controller_free_string(char* value)
{
    std::free(value);
}

void controller_init(const char* resource_dir)
{
    std::filesystem::path path;
    if (resource_dir != nullptr)
    {
        path = std::filesystem::path{ std::u8string{ reinterpret_cast<const char8_t*>(resource_dir) } };
    }
    state.reset();
    state = std::make_unique<parabolic::application::State>(path);
}

char* controller_tr(const char* msgid)
{
    if (msgid == nullptr)
    {
        return nullptr;
    }
    if (state == nullptr)
    {
        return duplicate(msgid);
    }
    try
    {
        return duplicate(state->translator()._(msgid));
    }
    catch (...)
    {
        return duplicate(msgid);
    }
}

char* controller_trn(const char* msgid, const char* msgid_plural, std::uint64_t n)
{
    if (msgid == nullptr || msgid_plural == nullptr)
    {
        return nullptr;
    }
    if (state == nullptr)
    {
        return duplicate(n == 1 ? msgid : msgid_plural);
    }
    try
    {
        return duplicate(state->translator()._n(msgid, msgid_plural, static_cast<unsigned long>(n)));
    }
    catch (...)
    {
        return duplicate(n == 1 ? msgid : msgid_plural);
    }
}

char* controller_trp(const char* context, const char* msgid)
{
    if (context == nullptr || msgid == nullptr)
    {
        return nullptr;
    }
    if (state == nullptr)
    {
        return duplicate(msgid);
    }
    try
    {
        return duplicate(state->translator()._p(context, msgid));
    }
    catch (...)
    {
        return duplicate(msgid);
    }
}

char* controller_trpn(const char* context, const char* msgid, const char* msgid_plural, std::uint64_t n)
{
    if (context == nullptr || msgid == nullptr || msgid_plural == nullptr)
    {
        return nullptr;
    }
    if (state == nullptr)
    {
        return duplicate(n == 1 ? msgid : msgid_plural);
    }
    try
    {
        return duplicate(state->translator()._pn(context, msgid, msgid_plural, static_cast<unsigned long>(n)));
    }
    catch (...)
    {
        return duplicate(n == 1 ? msgid : msgid_plural);
    }
}
