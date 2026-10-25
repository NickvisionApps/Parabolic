#pragma once

#include <compare>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "translator.h"

namespace parabolic::subtitle
{
    enum class Format : std::uint8_t
    {
        Any,
        Vtt,
        Srt,
        Ass,
        Lrc,
    };

    class Subtitle
    {
    public:
        Subtitle(std::string language, bool is_auto_generated);

        [[nodiscard]] const std::string& language() const;

        [[nodiscard]] bool is_auto_generated() const;

        [[nodiscard]] std::string str(const translator::Translator& translator) const;

        std::strong_ordering operator<=>(const Subtitle& rhs) const;

        bool operator==(const Subtitle& rhs) const;

        friend void from_json(const nlohmann::json& json, Subtitle& language);

        friend void to_json(nlohmann::json& json, const Subtitle& language);

    private:
        std::string m_language;
        bool m_is_auto_generated;
    };
}
