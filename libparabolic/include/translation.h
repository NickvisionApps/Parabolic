#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <filesystem>
#include <format>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <vector>
#include "string_manip.h"

namespace parabolic::translation
{
    class Options
    {
    public:
        Options();
        [[nodiscard]] const std::string& language() const;
        void set_language(const std::string& value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        std::string m_language;
    };

    class Translator
    {
    public:
        Translator(const std::string& app_short_name, std::filesystem::path locale_dir = {});
        [[nodiscard]] static const std::string& follow_system_language();
        [[nodiscard]] const std::vector<std::string>& available_languages() const;
        [[nodiscard]] std::string language() const;
        void set_options(const Options& options);
        [[nodiscard]] const char* _(const char* msgid) const;
        [[nodiscard]] const char* _n(const char* msgid, const char* msgid_plural, unsigned long n) const;
        [[nodiscard]] const char* _p(const char* context, const char* msgid) const;
        [[nodiscard]] const char* _pn(const char* context, const char* msgid, const char* msgid_plural,
                                      unsigned long n) const;

        template<typename... Args>
        std::string _(const char* msgid, const Args&... args) const
        {
            return std::vformat(_(msgid), std::make_format_args(args...));
        }
        template<typename... Args>
        std::string _n(const char* msgid, const char* msgid_plural, unsigned long n, const Args&... args) const
        {
            return std::vformat(_n(msgid, msgid_plural, n), std::make_format_args(args...));
        }
        template<typename... Args>
        std::string _p(const char* context, const char* msgid, const Args&... args) const
        {
            return std::vformat(_p(context, msgid), std::make_format_args(args...));
        }
        template<typename... Args>
        std::string _pn(const char* context, const char* msgid, const char* msgid_plural, unsigned long n,
                        const Args&... args) const
        {
            return std::vformat(_pn(context, msgid, msgid_plural, n), std::make_format_args(args...));
        }
        template<typename... Args>
        std::wstring w_(const char* msgid, const Args&... args) const
        {
            return string_manip::wstr(std::vformat(_(msgid), std::make_format_args(args...)));
        }
        template<typename... Args>
        std::wstring w_n(const char* msgid, const char* msgid_plural, unsigned long n, const Args&... args) const
        {
            return string_manip::wstr(std::vformat(_n(msgid, msgid_plural, n), std::make_format_args(args...)));
        }
        template<typename... Args>
        std::wstring w_p(const char* context, const char* msgid, const Args&... args) const
        {
            return string_manip::wstr(std::vformat(_p(context, msgid), std::make_format_args(args...)));
        }
        template<typename... Args>
        std::wstring w_pn(const char* context, const char* msgid, const char* msgid_plural, unsigned long n,
                          const Args&... args) const
        {
            return string_manip::wstr(
                    std::vformat(_pn(context, msgid, msgid_plural, n), std::make_format_args(args...)));
        }

    private:
        mutable std::mutex m_mutex;
        std::string m_domain_name;
        std::filesystem::path m_locale_dir;
        std::vector<std::string> m_available_languages;
        Options m_options;
        std::string m_language;
    };

    [[nodiscard]] bool is_youtube_supported_language(const std::string& language);
}

#endif
