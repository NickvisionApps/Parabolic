#include <algorithm>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>
#include <sago/platform_folders.h>
#include <semver.hpp>
#include <string>
#include "application_state.h"
#include "hash.h"
#include "updater.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

#ifdef _WIN32
#ifdef _M_ARM64
#define TARGET_ASSET "NickvisionParabolicSetup-arm64.exe"
#else
#define TARGET_ASSET "NickvisionParabolicSetup.exe"
#endif
#elif defined(__APPLE__)
#ifdef __aarch64__
#define TARGET_ASSET "Parabolic-macOS-arm64.zip"
#else
#define TARGET_ASSET "Parabolic-macOS-x64.zip"
#endif
#else
#ifdef __aarch64__
#define TARGET_ASSET "org.nickvision.tubeconverter.aarch64.flatpak"
#else
#define TARGET_ASSET "org.nickvision.tubeconverter.x64.flatpak"
#endif
#endif

using namespace parabolic;

namespace
{
    std::filesystem::path cache_file()
    {
        ProductInfo& product_info{ ApplicationState::instance().product_info() };
#ifdef _WIN32
        return std::filesystem::path(string_manip::wstr(sago::getCacheDir())) / product_info.repo_owner() / product_info.repo_name() / "releases.json";
#else
        return std::filesystem::path(sago::getCacheDir()) / product_info.repo_owner() / product_info.repo_name() / "releases.json";
#endif
    }

    class DownloadedAsset
    {
    public:
        DownloadedAsset()
            : m_path{ cache_file().parent_path() / TARGET_ASSET }
        {

        }

        ~DownloadedAsset()
        {
            std::error_code ec;
            std::filesystem::remove(m_path, ec);
        }

        DownloadedAsset(const DownloadedAsset&) = delete;
        DownloadedAsset& operator=(const DownloadedAsset&) = delete;

    private:
        std::filesystem::path m_path;
    };

    Updater make_updater(const semver::version<>& version, const std::string& asset)
    {
        ProductInfo& product_info{ ApplicationState::instance().product_info() };
        return Updater{ product_info.repo_owner(), product_info.repo_name(), version, asset };
    }
}

TEST(UpdaterTest, update_info)
{
    UpdateInfo info;
    ASSERT_EQ((semver::version<>{ 0, 0, 0 }), info.version());
    ASSERT_FALSE(info.is_prerelease());
    ASSERT_EQ("", info.changelog());
    ASSERT_EQ("", info.asset_name());
    ASSERT_EQ("", info.asset_url());
    ASSERT_EQ("", info.asset_digest());
    info.set_version(semver::version<>{ 2026, 5, 0 });
    info.set_is_prerelease(true);
    info.set_changelog("- Changes");
    info.set_asset_name("asset.exe");
    info.set_asset_url("https://example.com/asset.exe");
    info.set_asset_digest("abc123");
    ASSERT_EQ((semver::version<>{ 2026, 5, 0 }), info.version());
    ASSERT_TRUE(info.is_prerelease());
    ASSERT_EQ("- Changes", info.changelog());
    ASSERT_EQ("asset.exe", info.asset_name());
    ASSERT_EQ("https://example.com/asset.exe", info.asset_url());
    ASSERT_EQ("abc123", info.asset_digest());
}

TEST(UpdaterTest, check_for_updates_latest_release)
{
    std::optional<UpdateInfo> info{ make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET).check_for_updates() };
    ASSERT_TRUE(info.has_value());
    ASSERT_GT(info->version(), (semver::version<>{ 2026, 4, 0 }));
    ASSERT_FALSE(info->is_prerelease());
    ASSERT_FALSE(info->changelog().empty());
    ASSERT_EQ(TARGET_ASSET, info->asset_name());
    ASSERT_EQ("https://github.com/NickvisionApps/Parabolic/releases/download/" + info->version().to_string() + "/" + TARGET_ASSET, info->asset_url());
    ASSERT_EQ(64U, info->asset_digest().size());
}

TEST(UpdaterTest, check_for_updates_asset_name_case_insensitive)
{
    std::string uppercase{ TARGET_ASSET };
    std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    std::optional<UpdateInfo> info{ make_updater(semver::version<>{ 2026, 4, 0 }, uppercase).check_for_updates() };
    ASSERT_TRUE(info.has_value());
    ASSERT_EQ(TARGET_ASSET, info->asset_name());
}

TEST(UpdaterTest, check_for_updates_up_to_date)
{
    ASSERT_FALSE(make_updater(semver::version<>{ 9999, 0, 0 }, TARGET_ASSET).check_for_updates().has_value());
}

TEST(UpdaterTest, check_for_updates_unknown_asset)
{
    ASSERT_FALSE(make_updater(semver::version<>{ 2026, 4, 0 }, "Missing.exe").check_for_updates().has_value());
}

TEST(UpdaterTest, check_for_updates_preview)
{
    Updater updater{ make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET) };
    ASSERT_FALSE(updater.allow_preview_updates());
    std::optional<UpdateInfo> stable{ updater.check_for_updates() };
    ASSERT_TRUE(stable.has_value());
    updater.set_allow_preview_updates(true);
    ASSERT_TRUE(updater.allow_preview_updates());
    std::optional<UpdateInfo> preview{ updater.check_for_updates() };
    ASSERT_TRUE(preview.has_value());
    ASSERT_GE(preview->version(), stable->version());
}

TEST(UpdaterTest, check_for_updates_cached)
{
    ASSERT_TRUE(make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET).check_for_updates().has_value());
    ASSERT_TRUE(std::filesystem::exists(cache_file()));
    std::filesystem::file_time_type before{ std::filesystem::last_write_time(cache_file()) };
    ASSERT_TRUE(make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET).check_for_updates().has_value());
    ASSERT_EQ(before, std::filesystem::last_write_time(cache_file()));
}

TEST(UpdaterTest, check_for_updates_stale_cache)
{
    ASSERT_TRUE(make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET).check_for_updates().has_value());
    std::filesystem::file_time_type stale{ std::filesystem::file_time_type::clock::now() - std::chrono::hours(13) };
    std::filesystem::last_write_time(cache_file(), stale);
    ASSERT_TRUE(make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET).check_for_updates().has_value());
    ASSERT_GT(std::filesystem::last_write_time(cache_file()), stale);
}

TEST(UpdaterTest, download_update)
{
    DownloadedAsset downloaded;
    Updater updater{ make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET) };
    std::optional<UpdateInfo> info{ updater.check_for_updates() };
    ASSERT_TRUE(info.has_value());
    std::uint64_t last_now{ 0 };
    std::uint64_t last_total{ 0 };
    std::optional<std::filesystem::path> path{ updater.download_update(*info, [&last_now, &last_total](std::uint64_t now, std::uint64_t total)
    {
        last_now = now;
        last_total = total;
    }) };
    ASSERT_TRUE(path.has_value());
    ASSERT_TRUE(std::filesystem::exists(*path));
    ASSERT_EQ(info->asset_name(), path->filename().string());
    ASSERT_EQ(info->asset_digest(), hash::sha256_from_file(*path));
    ASSERT_GT(last_total, 0U);
    ASSERT_EQ(last_total, last_now);
}

TEST(UpdaterTest, download_update_incomplete_info)
{
    Updater updater{ make_updater(semver::version<>{ 2026, 4, 0 }, TARGET_ASSET) };
    std::optional<UpdateInfo> valid{ updater.check_for_updates() };
    ASSERT_TRUE(valid.has_value());
    for (int missing{ 0 }; missing < 3; missing++)
    {
        UpdateInfo info{ *valid };
        if (missing == 0)
        {
            info.set_asset_name("");
        }
        else if (missing == 1)
        {
            info.set_asset_url("");
        }
        else
        {
            info.set_asset_digest("");
        }
        ASSERT_FALSE(updater.download_update(info, UpdateDownloadProgressCallback{}).has_value());
    }
}
