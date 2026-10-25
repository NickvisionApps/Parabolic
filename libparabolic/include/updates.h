#pragma once

#include <filesystem>
#include <functional>
#include <optional>
#include <semver.hpp>
#include <string>

namespace parabolic::updates
{
    using DownloadProgressCallback = std::function<void(std::uint64_t, std::uint64_t)>;

    class Info
    {
    public:
        Info() = default;
        [[nodiscard]] const semver::version<>& version() const;
        void set_version(const semver::version<>& version);
        [[nodiscard]] bool is_prerelease() const;
        void set_is_prerelease(bool is_prerelease);
        [[nodiscard]] const std::string& changelog() const;
        void set_changelog(const std::string& changelog);
        [[nodiscard]] const std::string& asset_name() const;
        void set_asset_name(const std::string& asset_name);
        [[nodiscard]] const std::string& asset_url() const;
        void set_asset_url(const std::string& asset_url);
        [[nodiscard]] const std::string& asset_digest() const;
        void set_asset_digest(const std::string& asset_digest);

    private:
        semver::version<> m_version{ 0, 0, 0 };
        bool m_is_prerelease{ false };
        std::string m_changelog;
        std::string m_asset_name;
        std::string m_asset_url;
        std::string m_asset_digest;
    };

    class Updater
    {
    public:
        Updater(std::string repo_owner, std::string repo_name, semver::version<> current_version,
                const std::string& target_asset_name);
        [[nodiscard]] bool allow_preview_updates() const;
        void set_allow_preview_updates(bool allow_preview_updates);
        [[nodiscard]] std::optional<Info> check_for_updates() const;
        [[nodiscard]] std::optional<std::filesystem::path>
        download_update(const Info& info, const DownloadProgressCallback& progress) const;

    private:
        std::string m_repo_owner;
        std::string m_repo_name;
        semver::version<> m_current_version;
        std::string m_target_asset_name;
        bool m_allow_preview_updates{ false };
    };
}
