#include "updater.h"
#include <chrono>
#include <cpr/cpr.h>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <sago/platform_folders.h>
#include <semver.hpp>
#include <string>
#include <utility>
#include "hash.h"
#include "string_manip.h"

#define USER_AGENT "NickvisionParabolic-Updater"

namespace parabolic::updater
{
    const semver::version<>& Info::version() const
    {
        return m_version;
    }

    void Info::set_version(const semver::version<>& version)
    {
        m_version = version;
    }

    bool Info::is_prerelease() const
    {
        return m_is_prerelease;
    }

    void Info::set_is_prerelease(bool is_prerelease)
    {
        m_is_prerelease = is_prerelease;
    }

    const std::string& Info::changelog() const
    {
        return m_changelog;
    }

    void Info::set_changelog(const std::string& changelog)
    {
        m_changelog = changelog;
    }

    const std::string& Info::asset_name() const
    {
        return m_asset_name;
    }

    void Info::set_asset_name(const std::string& asset_name)
    {
        m_asset_name = asset_name;
    }

    const std::string& Info::asset_url() const
    {
        return m_asset_url;
    }

    void Info::set_asset_url(const std::string& asset_url)
    {
        m_asset_url = asset_url;
    }

    const std::string& Info::asset_digest() const
    {
        return m_asset_digest;
    }

    void Info::set_asset_digest(const std::string& asset_digest)
    {
        m_asset_digest = asset_digest;
    }

    Updater::Updater(std::string repo_owner, std::string repo_name, semver::version<> current_version,
                     const std::string& target_asset_name)
        : m_repo_owner{ std::move(repo_owner) },
          m_repo_name{ std::move(repo_name) },
          m_current_version{ std::move(current_version) },
          m_target_asset_name{ string_manip::lower(target_asset_name) }
    {
    }

    bool Updater::allow_preview_updates() const
    {
        return m_allow_preview_updates;
    }

    void Updater::set_allow_preview_updates(bool allow_preview_updates)
    {
        m_allow_preview_updates = allow_preview_updates;
    }

    std::optional<Info> Updater::check_for_updates() const
    {
        nlohmann::json releases = nlohmann::json::array();
        std::filesystem::path cache_file{
            std::filesystem::path{ sago::getCacheDir() } / m_repo_owner / m_repo_name / "releases.json",
        };
        if (std::filesystem::exists(cache_file))
        {
            if (std::filesystem::last_write_time(cache_file) + std::chrono::hours(12) <=
                std::filesystem::file_time_type::clock::now())
            {
                std::filesystem::remove(cache_file);
            }
            else
            {
                std::ifstream in{ cache_file };
                if (in)
                {
                    releases = nlohmann::json::parse(in, nullptr, false, true);
                    if (releases.is_discarded() || !releases.is_array())
                    {
                        releases = nlohmann::json::array();
                    }
                }
            }
        }
        if (releases.empty())
        {
            cpr::Response response{
                cpr::Get(
                        cpr::Url{
                                std::format("https://api.github.com/repos/{}/{}/releases?per_page=5", m_repo_owner,
                                            m_repo_name),
                        },
                        cpr::UserAgent{ USER_AGENT },
                        cpr::Header{
                                { "Accept", "application/vnd.github+json" },
                                { "X-GitHub-Api-Version", "2022-11-28" },
                        },
                        cpr::ConnectTimeout{ 15000 }, cpr::Timeout{ 30000 }),
            };
            if (response.error || response.status_code != 200)
            {
                return std::nullopt;
            }
            releases = nlohmann::json::parse(response.text, nullptr, false, true);
            if (releases.is_discarded() || !releases.is_array())
            {
                return std::nullopt;
            }
            std::filesystem::create_directories(cache_file.parent_path());
            std::ofstream out{ cache_file, std::ios::out | std::ios::trunc };
            if (out)
            {
                out << releases.dump();
            }
        }
        for (const nlohmann::json& release: releases)
        {
            if (!release.is_object() || release.value("draft", false))
            {
                continue;
            }
            bool prerelease{ release.value("prerelease", false) };
            if (!m_allow_preview_updates && prerelease)
            {
                continue;
            }
            if (!release.contains("assets") || !release["assets"].is_array())
            {
                continue;
            }
            std::string tag{ release.value("tag_name", "") };
            if (!tag.empty() && (tag[0] == 'v' || tag[0] == 'V'))
            {
                tag = tag.substr(1);
            }
            semver::version version{ 0, 0, 0 };
            try
            {
                version = semver::from_string(tag);
            }
            catch (...)
            {
                continue;
            }
            if (version <= m_current_version)
            {
                continue;
            }
            for (const nlohmann::json& asset: release["assets"])
            {
                if (!asset.is_object() || string_manip::lower(asset.value("name", "")) != m_target_asset_name)
                {
                    continue;
                }
                std::string digest{
                    string_manip::lower(asset.contains("digest") && asset["digest"].is_string()
                                                ? asset["digest"].get<std::string>()
                                                : ""),
                };
                if (digest.empty())
                {
                    continue;
                }
                Info info;
                info.set_version(version);
                info.set_is_prerelease(prerelease);
                info.set_changelog(release.contains("body") && release["body"].is_string()
                                           ? release["body"].get<std::string>()
                                           : "");
                info.set_asset_name(asset.value("name", ""));
                info.set_asset_url(asset.value("browser_download_url", ""));
                info.set_asset_digest(digest.starts_with("sha256:") ? digest.substr(7) : digest);
                return info;
            }
        }
        return std::nullopt;
    }

    std::optional<std::filesystem::path> Updater::download_update(const Info& info,
                                                                  const DownloadProgressCallback& progress) const
    {
        if (info.asset_url().empty() || info.asset_digest().empty() || info.asset_name().empty())
        {
            return std::nullopt;
        }
        std::filesystem::path destination{
            std::filesystem::path{ sago::getCacheDir() } / m_repo_owner / m_repo_name /
                    std::filesystem::path(info.asset_name()).filename(),
        };
        std::filesystem::create_directories(destination.parent_path());
        std::ofstream out{ destination, std::ios::binary | std::ios::trunc };
        if (!out)
        {
            return std::nullopt;
        }
        cpr::Response response{
            cpr::Download(out, cpr::Url{ info.asset_url() }, cpr::UserAgent{ USER_AGENT },
                          cpr::Header{ { "Accept", "application/octet-stream" } }, cpr::ConnectTimeout{ 15000 },
                          cpr::ProgressCallback{ [&progress](cpr::cpr_pf_arg_t total, cpr::cpr_pf_arg_t now,
                                                             cpr::cpr_pf_arg_t, cpr::cpr_pf_arg_t,
                                                             std::intptr_t) -> bool
        {
            if (progress)
            {
                progress(static_cast<std::uint64_t>(now), static_cast<std::uint64_t>(total));
            }
            return true;
        } }),
        };
        out.close();
        if (!response.error && response.status_code == 200)
        {
            if (hash::sha256_from_file(destination) == info.asset_digest())
            {
                return destination;
            }
        }
        std::filesystem::remove(destination);
        return std::nullopt;
    }
}
