#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include "cookies.h"

namespace parabolic::extractor
{
    class Options
    {
    public:
        [[nodiscard]] cookies::Browser cookies_browser() const;
        void set_cookies_browser(cookies::Browser value);
        [[nodiscard]] const std::string& cookies_path() const;
        void set_cookies_path(const std::string& value);
        [[nodiscard]] const std::string& proxy_url() const;
        void set_proxy_url(const std::string& value);
        [[nodiscard]] std::optional<int> speed_limit() const;
        void set_speed_limit(std::optional<int> value);
        [[nodiscard]] bool translate_metadata_and_chapters() const;
        void set_translate_metadata_and_chapters(bool value);
        [[nodiscard]] bool use_part_files() const;
        void set_use_part_files(bool value);
        [[nodiscard]] bool youtube_sponsor_block() const;
        void set_youtube_sponsor_block(bool value);
        bool operator==(const Options& rhs) const = default;
        friend void from_json(const nlohmann::json& json, Options& options);
        friend void to_json(nlohmann::json& json, const Options& options);

    private:
        cookies::Browser m_cookies_browser{ cookies::Browser::None };
        std::string m_cookies_path;
        std::string m_proxy_url;
        std::optional<int> m_speed_limit;
        bool m_translate_metadata_and_chapters{ true };
        bool m_use_part_files{ true };
        bool m_youtube_sponsor_block{ false };
    };
}

#endif
