#include "video.h"
#include <algorithm>
#include <compare>
#include <limits>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "string_manip.h"
#include "translation.h"

namespace parabolic::video
{
    Resolution::Resolution(int width, int height)
        : m_width{ std::max(width, height) },
          m_height{ std::min(width, height) }
    {
    }

    Resolution& Resolution::best()
    {
        static Resolution best{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max() };
        return best;
    }

    Resolution& Resolution::worst()
    {
        static Resolution worst{ -1, -1 };
        return worst;
    }

    std::optional<Resolution> Resolution::parse(std::string_view value)
    {
        if (value == "Best")
        {
            return best();
        }
        if (value == "Worst")
        {
            return worst();
        }
        std::vector<std::string> parts{ string_manip::split(value, 'x') };
        if (parts.size() != 2)
        {
            return std::nullopt;
        }
        try
        {
            return Resolution{ std::stoi(parts[0]), std::stoi(parts[1]) };
        }
        catch (...)
        {
            return std::nullopt;
        }
    }

    int Resolution::width() const
    {
        return m_width;
    }

    int Resolution::height() const
    {
        return m_height;
    }

    std::string Resolution::str(const translation::Translator& translator) const
    {
        if (m_width == std::numeric_limits<int>::max() && m_height == std::numeric_limits<int>::max())
        {
            return translator._("Best");
        }
        if (m_width == -1 && m_height == -1)
        {
            return translator._("Worst");
        }
        return translator._p("Resolution", "{}x{}", m_width, m_height);
    }

    std::strong_ordering Resolution::operator<=>(const Resolution& rhs) const
    {
        if (m_width == rhs.m_width)
        {
            return m_height <=> rhs.m_height;
        }
        return m_width <=> rhs.m_width;
    }

    bool Resolution::operator==(const Resolution& rhs) const
    {
        return m_width == rhs.m_width && m_height == rhs.m_height;
    }

    void from_json(const nlohmann::json& json, Resolution& resolution)
    {
        if (json.contains("width") && json["width"].is_number_integer())
        {
            resolution.m_width = json.at("width").get<int>();
        }
        else if (json.contains("Width") && json["Width"].is_number_integer())
        {
            resolution.m_width = json.at("Width").get<int>();
        }
        if (json.contains("height") && json["height"].is_number_integer())
        {
            resolution.m_height = json.at("height").get<int>();
        }
        else if (json.contains("Height") && json["Height"].is_number_integer())
        {
            resolution.m_height = json.at("Height").get<int>();
        }
    }

    void to_json(nlohmann::json& json, const Resolution& resolution)
    {
        json = nlohmann::json{ { "width", resolution.m_width }, { "height", resolution.m_height } };
    }
}
