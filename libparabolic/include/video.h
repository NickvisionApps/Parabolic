#ifndef VIDEO_H
#define VIDEO_H

#include <compare>
#include <cstdint>
#include <limits>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "translation.h"

namespace parabolic::video
{
    enum class Codec : std::uint8_t
    {
        Any,
        H265,
        Vp9,
        Av01,
        H264,
    };

    enum class FrameRate : std::uint8_t
    {
        Any,
        Fps24,
        Fps30,
        Fps60,
    };

    class Resolution
    {
    public:
        Resolution() = default;
        Resolution(int width, int height);
        static Resolution& best();
        static Resolution& worst();
        [[nodiscard]] static std::optional<Resolution> parse(std::string_view value);
        [[nodiscard]] int width() const;
        [[nodiscard]] int height() const;
        [[nodiscard]] std::string str(const translation::Translator& translator) const;
        std::strong_ordering operator<=>(const Resolution& rhs) const;
        bool operator==(const Resolution& rhs) const;
        friend void from_json(const nlohmann::json& json, Resolution& resolution);
        friend void to_json(nlohmann::json& json, const Resolution& resolution);

    private:
        int m_width{ std::numeric_limits<int>::max() };
        int m_height{ std::numeric_limits<int>::max() };
    };
}

#endif
