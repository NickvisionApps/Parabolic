#ifndef SUBTITLE_H
#define SUBTITLE_H

#include <compare>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "translation.h"

namespace parabolic::subtitle
{
    enum class Type : std::uint8_t
    {
        Any,
        Vtt,
        Srt,
        Ass,
        Lrc,
    };

    class Language
    {
    public:
        Language(std::string language, bool is_auto_generated);
        [[nodiscard]] const std::string& language() const;
        [[nodiscard]] bool is_auto_generated() const;
        [[nodiscard]] std::string str(const translation::Translator& translator) const;
        std::strong_ordering operator<=>(const Language& rhs) const;
        bool operator==(const Language& rhs) const;
        friend void from_json(const nlohmann::json& json, Language& language);
        friend void to_json(nlohmann::json& json, const Language& language);

    private:
        std::string m_language;
        bool m_is_auto_generated;
    };

    class Options
    {
    public:
        [[nodiscard]] bool include_auto_generated() const;
        void set_include_auto_generated(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        bool m_include_auto_generated{ true };
    };
}

#endif
