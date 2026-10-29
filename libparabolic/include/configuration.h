#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstdint>
#include <filesystem>
#include <mutex>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include "aria2.h"
#include "discovery.h"
#include "download.h"
#include "extractor.h"
#include "history.h"
#include "postprocessing.h"
#include "subtitle.h"
#include "translation.h"
#include "update.h"

namespace parabolic::configuration
{
    enum class Theme : std::uint8_t
    {
        Light,
        Dark,
        System,
    };

    class WindowGeometry
    {
    public:
        WindowGeometry() = default;
        WindowGeometry(int width, int height, bool is_maximized);
        WindowGeometry(int width, int height, bool is_maximized, int x, int y);
        [[nodiscard]] int height() const;
        void set_height(int value);
        [[nodiscard]] bool is_maximized() const;
        void set_is_maximized(bool value);
        [[nodiscard]] int width() const;
        void set_width(int value);
        [[nodiscard]] int x() const;
        void set_x(int value);
        [[nodiscard]] int y() const;
        void set_y(int value);
        bool operator==(const WindowGeometry& rhs) const = default;
        friend void from_json(const nlohmann::json& json, WindowGeometry& geometry);
        friend void to_json(nlohmann::json& json, const WindowGeometry& geometry);

    private:
        int m_height{ 600 };
        bool m_is_maximized{ false };
        int m_width{ 800 };
        int m_x{ 10 };
        int m_y{ 10 };
    };

    class Configuration
    {
    public:
        Configuration(std::string_view app_name);
        [[nodiscard]] aria2::Options aria2_options() const;
        void set_aria2_options(const aria2::Options& value);
        [[nodiscard]] bool automatically_check_for_updates() const;
        void set_automatically_check_for_updates(bool value);
        [[nodiscard]] discovery::Options discovery_options() const;
        void set_discovery_options(const discovery::Options& value);
        [[nodiscard]] download::Options download_options() const;
        void set_download_options(const download::Options& value);
        [[nodiscard]] extractor::Options extractor_options() const;
        void set_extractor_options(const extractor::Options& value);
        [[nodiscard]] history::Options history_options() const;
        void set_history_options(const history::Options& value);
        [[nodiscard]] postprocessing::Options postprocessing_options() const;
        void set_postprocessing_options(const postprocessing::Options& value);
        [[nodiscard]] download::PreviousDownloadOptions previous_download_options() const;
        void set_previous_download_options(const download::PreviousDownloadOptions& value);
        [[nodiscard]] bool show_disclaimer_on_startup() const;
        void set_show_disclaimer_on_startup(bool value);
        [[nodiscard]] subtitle::Options subtitle_options() const;
        void set_subtitle_options(const subtitle::Options& value);
        [[nodiscard]] Theme theme() const;
        void set_theme(Theme value);
        [[nodiscard]] translation::Options translation_options() const;
        void set_translation_options(const translation::Options& value);
        [[nodiscard]] update::Options update_options() const;
        void set_update_options(const update::Options& value);
        [[nodiscard]] WindowGeometry window_geometry() const;
        void set_window_geometry(const WindowGeometry& value);
        [[nodiscard]] std::string str() const;
        friend void from_json(const nlohmann::json& json, Configuration& config);
        friend void to_json(nlohmann::json& json, const Configuration& config);

    private:
        void save() const;
        mutable std::mutex m_mutex;
        aria2::Options m_aria2_options;
        bool m_automatically_check_for_updates{ true };
        discovery::Options m_discovery_options;
        download::Options m_download_options;
        extractor::Options m_extractor_options;
        history::Options m_history_options;
        std::filesystem::path m_path;
        postprocessing::Options m_postprocessing_options;
        download::PreviousDownloadOptions m_previous_download_options;
        bool m_show_disclaimer_on_startup{ true };
        subtitle::Options m_subtitle_options;
        Theme m_theme{ Theme::System };
        translation::Options m_translation_options;
        update::Options m_update_options;
        WindowGeometry m_window_geometry;
    };
}

#endif
