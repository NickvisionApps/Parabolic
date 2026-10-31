#include "configuration.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <limits>
#include <mutex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include "aria2.h"
#include "discovery.h"
#include "download.h"
#include "environment.h"
#include "extractor.h"
#include "history.h"
#include "postprocessing.h"
#include "translation.h"
#include "update.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

namespace parabolic::configuration
{
    WindowGeometry::WindowGeometry(int width, int height, bool is_maximized)
        : m_height{ height },
          m_is_maximized{ is_maximized },
          m_width{ width }
    {
    }

    WindowGeometry::WindowGeometry(int width, int height, bool is_maximized, int x, int y)
        : m_height{ height },
          m_is_maximized{ is_maximized },
          m_width{ width },
          m_x{ x },
          m_y{ y }
    {
    }

    int WindowGeometry::height() const
    {
        return m_height;
    }

    void WindowGeometry::set_height(int value)
    {
        m_height = value;
    }

    bool WindowGeometry::is_maximized() const
    {
        return m_is_maximized;
    }

    void WindowGeometry::set_is_maximized(bool value)
    {
        m_is_maximized = value;
    }

    int WindowGeometry::width() const
    {
        return m_width;
    }

    void WindowGeometry::set_width(int value)
    {
        m_width = value;
    }

    int WindowGeometry::x() const
    {
        return m_x;
    }

    void WindowGeometry::set_x(int value)
    {
        m_x = value;
    }

    int WindowGeometry::y() const
    {
        return m_y;
    }

    void WindowGeometry::set_y(int value)
    {
        m_y = value;
    }

    Configuration::Configuration(std::string_view app_name)
#ifdef _WIN32
        : m_path{ environment::is_portable() ? environment::executable_directory() / "config.json"
                                             : std::filesystem::path(string_manip::wstr(sago::getConfigHome())) /
                                                       app_name / "config.json" }
#else
        : m_path{
              environment::is_portable() ? environment::executable_directory() / "config.json"
                                         : std::filesystem::path(sago::getConfigHome()) / app_name / "config.json",
          }
#endif
    {
        try
        {
            std::ifstream file{ m_path };
            if (file)
            {
                nlohmann::json json = nlohmann::json::parse(file, nullptr, false, true);
                if (!json.is_discarded() && json.is_object())
                {
                    from_json(json, *this);
                }
            }
        }
        catch (const std::exception&)
        {
            return;
        }
    }

    void Configuration::save() const
    {
        try
        {
            nlohmann::json json = *this;
            std::filesystem::create_directories(m_path.parent_path());
            std::filesystem::path temp{ m_path };
            temp += ".tmp";
            std::ofstream file{ temp, std::ios::out | std::ios::trunc };
            file << json.dump();
            file.close();
            if (file.fail())
            {
                std::filesystem::remove(temp);
                return;
            }
            std::filesystem::rename(temp, m_path);
        }
        catch (const std::exception&)
        {
            return;
        }
    }

    aria2::Options Configuration::aria2_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_aria2_options;
    }

    void Configuration::set_aria2_options(const aria2::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_aria2_options == value)
        {
            return;
        }
        m_aria2_options = value;
        save();
    }

    bool Configuration::automatically_check_for_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_automatically_check_for_updates;
    }

    void Configuration::set_automatically_check_for_updates(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_automatically_check_for_updates == value)
        {
            return;
        }
        m_automatically_check_for_updates = value;
        save();
    }

    discovery::Options Configuration::discovery_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_discovery_options;
    }

    void Configuration::set_discovery_options(const discovery::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_discovery_options == value)
        {
            return;
        }
        m_discovery_options = value;
        save();
    }

    download::Options Configuration::download_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_download_options;
    }

    void Configuration::set_download_options(const download::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_download_options == value)
        {
            return;
        }
        m_download_options = value;
        save();
    }

    extractor::Options Configuration::extractor_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_extractor_options;
    }

    void Configuration::set_extractor_options(const extractor::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_extractor_options == value)
        {
            return;
        }
        m_extractor_options = value;
        save();
    }

    history::Options Configuration::history_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_history_options;
    }

    void Configuration::set_history_options(const history::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_history_options == value)
        {
            return;
        }
        m_history_options = value;
        save();
    }

    postprocessing::Options Configuration::postprocessing_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_postprocessing_options;
    }

    void Configuration::set_postprocessing_options(const postprocessing::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_postprocessing_options == value)
        {
            return;
        }
        m_postprocessing_options = value;
        save();
    }

    download::PreviousDownloadOptions Configuration::previous_download_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_previous_download_options;
    }

    void Configuration::set_previous_download_options(const download::PreviousDownloadOptions& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_previous_download_options == value)
        {
            return;
        }
        m_previous_download_options = value;
        save();
    }

    bool Configuration::show_disclaimer_on_startup() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_show_disclaimer_on_startup;
    }

    void Configuration::set_show_disclaimer_on_startup(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_show_disclaimer_on_startup == value)
        {
            return;
        }
        m_show_disclaimer_on_startup = value;
        save();
    }

    Theme Configuration::theme() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_theme;
    }

    void Configuration::set_theme(Theme value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_theme == value)
        {
            return;
        }
        m_theme = value;
        save();
    }

    translation::Options Configuration::translation_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_translation_options;
    }

    void Configuration::set_translation_options(const translation::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_translation_options == value)
        {
            return;
        }
        m_translation_options = value;
        save();
    }

    update::Options Configuration::update_options() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_update_options;
    }

    void Configuration::set_update_options(const update::Options& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_update_options == value)
        {
            return;
        }
        m_update_options = value;
        save();
    }

    WindowGeometry Configuration::window_geometry() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_window_geometry;
    }

    void Configuration::set_window_geometry(const WindowGeometry& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (m_window_geometry == value)
        {
            return;
        }
        m_window_geometry = value;
        save();
    }

    std::string Configuration::str() const
    {
        std::scoped_lock lock{ m_mutex };
        nlohmann::json json = *this;
        return json.dump(4);
    }

    void from_json(const nlohmann::json& json, Configuration& config)
    {
        if (json.contains("automaticallyCheckForUpdates") && json["automaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json.at("automaticallyCheckForUpdates").get<bool>();
        }
        else if (json.contains("AutomaticallyCheckForUpdates") && json["AutomaticallyCheckForUpdates"].is_boolean())
        {
            config.m_automatically_check_for_updates = json.at("AutomaticallyCheckForUpdates").get<bool>();
        }
        if (json.contains("showDisclaimerOnStartup") && json["showDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json.at("showDisclaimerOnStartup").get<bool>();
        }
        else if (json.contains("ShowDisclaimerOnStartup") && json["ShowDisclaimerOnStartup"].is_boolean())
        {
            config.m_show_disclaimer_on_startup = json.at("ShowDisclaimerOnStartup").get<bool>();
        }
        if (json.contains("theme") && json["theme"].is_number_integer())
        {
            int value{ json.at("theme").get<int>() };
            if (value >= static_cast<int>(Theme::Light) && value <= static_cast<int>(Theme::System))
            {
                config.m_theme = static_cast<Theme>(value);
            }
        }
        else if (json.contains("Theme") && json["Theme"].is_number_integer())
        {
            int value{ json.at("Theme").get<int>() };
            if (value >= static_cast<int>(Theme::Light) && value <= static_cast<int>(Theme::System))
            {
                config.m_theme = static_cast<Theme>(value);
            }
        }
        if (json.contains("windowGeometry") && json["windowGeometry"].is_object())
        {
            config.m_window_geometry = json["windowGeometry"].get<WindowGeometry>();
        }
        from_json(json, config.m_aria2_options);
        from_json(json, config.m_discovery_options);
        from_json(json, config.m_download_options);
        from_json(json, config.m_extractor_options);
        from_json(json, config.m_history_options);
        from_json(json, config.m_postprocessing_options);
        from_json(json, config.m_previous_download_options);
        from_json(json, config.m_translation_options);
        from_json(json, config.m_update_options);
    }

    void from_json(const nlohmann::json& json, WindowGeometry& geometry)
    {
        if (json.contains("height") && json["height"].is_number_integer())
        {
            geometry.m_height = std::clamp(json.at("height").get<int>(), 1, std::numeric_limits<int>::max());
        }
        else if (json.contains("Height") && json["Height"].is_number_integer())
        {
            geometry.m_height = std::clamp(json.at("Height").get<int>(), 1, std::numeric_limits<int>::max());
        }
        if (json.contains("isMaximized") && json["isMaximized"].is_boolean())
        {
            geometry.m_is_maximized = json.at("isMaximized").get<bool>();
        }
        else if (json.contains("IsMaximized") && json["IsMaximized"].is_boolean())
        {
            geometry.m_is_maximized = json.at("IsMaximized").get<bool>();
        }
        if (json.contains("width") && json["width"].is_number_integer())
        {
            geometry.m_width = std::clamp(json.at("width").get<int>(), 1, std::numeric_limits<int>::max());
        }
        else if (json.contains("Width") && json["Width"].is_number_integer())
        {
            geometry.m_width = std::clamp(json.at("Width").get<int>(), 1, std::numeric_limits<int>::max());
        }
        if (json.contains("x") && json["x"].is_number_integer())
        {
            geometry.m_x = json.at("x").get<int>();
        }
        else if (json.contains("X") && json["X"].is_number_integer())
        {
            geometry.m_x = json.at("X").get<int>();
        }
        if (json.contains("y") && json["y"].is_number_integer())
        {
            geometry.m_y = json.at("y").get<int>();
        }
        else if (json.contains("Y") && json["Y"].is_number_integer())
        {
            geometry.m_y = json.at("Y").get<int>();
        }
    }

    void to_json(nlohmann::json& json, const Configuration& config)
    {
        json = nlohmann::json{
            { "automaticallyCheckForUpdates", config.m_automatically_check_for_updates },
            { "showDisclaimerOnStartup", config.m_show_disclaimer_on_startup },
            { "theme", static_cast<int>(config.m_theme) },
            { "windowGeometry", config.m_window_geometry },
        };
        json.update(nlohmann::json(config.m_aria2_options));
        json.update(nlohmann::json(config.m_discovery_options));
        json.update(nlohmann::json(config.m_download_options));
        json.update(nlohmann::json(config.m_extractor_options));
        json.update(nlohmann::json(config.m_history_options));
        json.update(nlohmann::json(config.m_postprocessing_options));
        json.update(nlohmann::json(config.m_previous_download_options));
        json.update(nlohmann::json(config.m_translation_options));
        json.update(nlohmann::json(config.m_update_options));
    }

    void to_json(nlohmann::json& json, const WindowGeometry& geometry)
    {
        json = nlohmann::json{
            { "height", geometry.m_height }, { "isMaximized", geometry.m_is_maximized },
            { "width", geometry.m_width },   { "x", geometry.m_x },
            { "y", geometry.m_y },
        };
    }
}
