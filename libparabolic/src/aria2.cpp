#include "aria2.h"
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include "environment.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::aria2
{
    int Options::max_connections_per_server() const
    {
        return m_max_connections_per_server;
    }

    void Options::set_max_connections_per_server(int value)
    {
        m_max_connections_per_server = std::clamp(value, 1, 16);
    }

    int Options::min_split_size() const
    {
        return m_min_split_size;
    }

    void Options::set_min_split_size(int value)
    {
        m_min_split_size = std::clamp(value, 1, 1024);
    }

    bool Options::use_aria() const
    {
        return m_use_aria;
    }

    void Options::set_use_aria(bool value)
    {
        m_use_aria = value;
    }

    std::string executable_path()
    {
        const std::filesystem::path& global_path{
            environment::find_dependency("aria2c", environment::DependencySearchOption::Global),
        };
        if (std::filesystem::exists(global_path))
        {
#ifdef _WIN32
            return string_manip::str(global_path.wstring());
#else
            return global_path.string();
#endif
        }
        return "aria2c";
    }

    void from_json(const nlohmann::json& json, Options& options)
    {
        if (json.contains("ariaMaxConnectionsPerServer") && json["ariaMaxConnectionsPerServer"].is_number_integer())
        {
            options.set_max_connections_per_server(json.at("ariaMaxConnectionsPerServer").get<int>());
        }
        else if (json.contains("AriaMaxConnectionsPerServer") &&
                 json["AriaMaxConnectionsPerServer"].is_number_integer())
        {
            options.set_max_connections_per_server(json.at("AriaMaxConnectionsPerServer").get<int>());
        }
        if (json.contains("ariaMinSplitSize") && json["ariaMinSplitSize"].is_number_integer())
        {
            options.set_min_split_size(json.at("ariaMinSplitSize").get<int>());
        }
        else if (json.contains("AriaMinSplitSize") && json["AriaMinSplitSize"].is_number_integer())
        {
            options.set_min_split_size(json.at("AriaMinSplitSize").get<int>());
        }
        if (json.contains("useAria") && json["useAria"].is_boolean())
        {
            options.set_use_aria(json.at("useAria").get<bool>());
        }
        else if (json.contains("UseAria") && json["UseAria"].is_boolean())
        {
            options.set_use_aria(json.at("UseAria").get<bool>());
        }
    }

    void to_json(nlohmann::json& json, const Options& options)
    {
        json = nlohmann::json{
            { "ariaMaxConnectionsPerServer", options.max_connections_per_server() },
            { "ariaMinSplitSize", options.min_split_size() },
            { "useAria", options.use_aria() },
        };
    }
}
