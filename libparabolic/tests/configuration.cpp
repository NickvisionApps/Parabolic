#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <random>
#include <sago/platform_folders.h>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include "configuration.h"
#include "translator.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

using namespace parabolic;

namespace
{
    class ConfigurationFile
    {
    public:
        ConfigurationFile()
        {
            static constexpr std::string_view charset{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
            thread_local std::mt19937_64 random_device{ std::random_device{}() };
            std::uniform_int_distribution<std::size_t> random{ 0, charset.size() - 1 };
            std::string suffix;
            suffix.reserve(16);
            for (std::size_t i{ 0 }; i < 16; i++)
            {
                suffix.push_back(charset[random(random_device)]);
            }
            m_name = "parabolic_test_" + suffix;
#ifdef _WIN32
            m_directory = std::filesystem::path(string_manip::wstr(sago::getConfigHome())) / m_name;
#else
            m_directory = std::filesystem::path(sago::getConfigHome()) / m_name;
#endif
        }

        ~ConfigurationFile()
        {
            std::error_code ec;
            std::filesystem::remove_all(m_directory, ec);
        }

        ConfigurationFile(const ConfigurationFile&) = delete;
        ConfigurationFile& operator=(const ConfigurationFile&) = delete;

        [[nodiscard]] const std::string& name() const
        {
            return m_name;
        }

        [[nodiscard]] std::filesystem::path path() const
        {
            return m_directory / "config.json";
        }

        void write(const std::string& content) const
        {
            std::filesystem::create_directories(m_directory);
            std::ofstream file{ path(), std::ios::binary };
            file << content;
        }

        [[nodiscard]] nlohmann::json read() const
        {
            std::ifstream file{ path() };
            return nlohmann::json::parse(file, nullptr, false);
        }

    private:
        std::string m_name;
        std::filesystem::path m_directory;
    };
}

TEST(ConfigurationTest, defaults)
{
    ConfigurationFile file;
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::System, config.theme());
    ASSERT_TRUE(config.automatically_check_for_updates());
    ASSERT_FALSE(config.allow_preview_updates());
    ASSERT_EQ(Translator::follow_system_language(), config.translation_language());
}

TEST(ConfigurationTest, constructor_does_not_create_file)
{
    ConfigurationFile file;
    Configuration config{ file.name() };
    ASSERT_FALSE(std::filesystem::exists(file.path()));
}

TEST(ConfigurationTest, set_theme)
{
    ConfigurationFile file;
    {
        Configuration config{ file.name() };
        config.set_theme(Theme::Dark);
        ASSERT_EQ(Theme::Dark, config.theme());
    }
    Configuration reloaded{ file.name() };
    ASSERT_EQ(Theme::Dark, reloaded.theme());
}

TEST(ConfigurationTest, set_automatically_check_for_updates)
{
    ConfigurationFile file;
    {
        Configuration config{ file.name() };
        config.set_automatically_check_for_updates(false);
        ASSERT_FALSE(config.automatically_check_for_updates());
    }
    Configuration reloaded{ file.name() };
    ASSERT_FALSE(reloaded.automatically_check_for_updates());
}

TEST(ConfigurationTest, set_allow_preview_updates)
{
    ConfigurationFile file;
    {
        Configuration config{ file.name() };
        config.set_allow_preview_updates(true);
        ASSERT_TRUE(config.allow_preview_updates());
    }
    Configuration reloaded{ file.name() };
    ASSERT_TRUE(reloaded.allow_preview_updates());
}

TEST(ConfigurationTest, set_translation_language)
{
    ConfigurationFile file;
    {
        Configuration config{ file.name() };
        config.set_translation_language("fr_FR");
        ASSERT_EQ("fr_FR", config.translation_language());
    }
    Configuration reloaded{ file.name() };
    ASSERT_EQ("fr_FR", reloaded.translation_language());
}

TEST(ConfigurationTest, set_translation_language_utf8)
{
    ConfigurationFile file;
    {
        Configuration config{ file.name() };
        config.set_translation_language("caf\xC3\xA9");
        ASSERT_EQ("caf\xC3\xA9", config.translation_language());
    }
    Configuration reloaded{ file.name() };
    ASSERT_EQ("caf\xC3\xA9", reloaded.translation_language());
}

TEST(ConfigurationTest, save_creates_directory_and_file)
{
    ConfigurationFile file;
    Configuration config{ file.name() };
    config.set_theme(Theme::Dark);
    config.set_automatically_check_for_updates(false);
    config.set_allow_preview_updates(true);
    config.set_translation_language("es_ES");
    ASSERT_TRUE(std::filesystem::is_regular_file(file.path()));
    nlohmann::json expected{
        { "theme", 1 },
        { "automaticallyCheckForUpdates", false },
        { "allowPreviewUpdates", true },
        { "translationLanguage", "es_ES" }
    };
    ASSERT_EQ(expected, file.read());
}

TEST(ConfigurationTest, unchanged_value_does_not_save)
{
    ConfigurationFile file;
    Configuration config{ file.name() };
    config.set_theme(Theme::System);
    config.set_automatically_check_for_updates(true);
    config.set_allow_preview_updates(false);
    config.set_translation_language("");
    ASSERT_EQ(Translator::follow_system_language(), config.translation_language());
    ASSERT_FALSE(std::filesystem::exists(file.path()));
}

TEST(ConfigurationTest, save_overwrites_invalid_file)
{
    ConfigurationFile file;
    file.write("{ this is not json");
    Configuration config{ file.name() };
    config.set_theme(Theme::Dark);
    nlohmann::json json = file.read();
    ASSERT_FALSE(json.is_discarded());
    ASSERT_EQ(1, json["theme"].get<int>());
}

TEST(ConfigurationTest, load_camel_case_keys)
{
    ConfigurationFile file;
    file.write(R"({"theme":0,"automaticallyCheckForUpdates":false,"allowPreviewUpdates":true,"translationLanguage":"it_IT"})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::Light, config.theme());
    ASSERT_FALSE(config.automatically_check_for_updates());
    ASSERT_TRUE(config.allow_preview_updates());
    ASSERT_EQ("it_IT", config.translation_language());
}

TEST(ConfigurationTest, pascal_case_keys)
{
    ConfigurationFile file;
    file.write(R"({"Theme":1,"AutomaticallyCheckForUpdates":false,"AllowPreviewUpdates":true,"TranslationLanguage":"pt_BR"})");
    Configuration pascal{ file.name() };
    ASSERT_EQ(Theme::Dark, pascal.theme());
    ASSERT_FALSE(pascal.automatically_check_for_updates());
    ASSERT_TRUE(pascal.allow_preview_updates());
    ASSERT_EQ("pt_BR", pascal.translation_language());
    pascal.set_allow_preview_updates(false);
    nlohmann::json expected{
        { "theme", 1 },
        { "automaticallyCheckForUpdates", false },
        { "allowPreviewUpdates", false },
        { "translationLanguage", "pt_BR" }
    };
    ASSERT_EQ(expected, file.read());
    file.write(R"({"theme":0,"Theme":1,"translationLanguage":"a","TranslationLanguage":"b"})");
    Configuration both{ file.name() };
    ASSERT_EQ(Theme::Light, both.theme());
    ASSERT_EQ("a", both.translation_language());
}

TEST(ConfigurationTest, load_pascal_case_used_when_camel_case_has_wrong_type)
{
    ConfigurationFile file;
    file.write(R"({"theme":"dark","Theme":1})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::Dark, config.theme());
}

TEST(ConfigurationTest, load_partial_keeps_defaults)
{
    ConfigurationFile file;
    file.write(R"({"allowPreviewUpdates":true})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::System, config.theme());
    ASSERT_TRUE(config.automatically_check_for_updates());
    ASSERT_TRUE(config.allow_preview_updates());
    ASSERT_EQ(Translator::follow_system_language(), config.translation_language());
}

TEST(ConfigurationTest, load_unknown_keys_ignored)
{
    ConfigurationFile file;
    file.write(R"({"unknown":5,"theme":1})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::Dark, config.theme());
}

TEST(ConfigurationTest, load_wrong_types_ignored)
{
    ConfigurationFile file;
    file.write(R"({"theme":"dark","automaticallyCheckForUpdates":"no","allowPreviewUpdates":1,"translationLanguage":5})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::System, config.theme());
    ASSERT_TRUE(config.automatically_check_for_updates());
    ASSERT_FALSE(config.allow_preview_updates());
    ASSERT_EQ(Translator::follow_system_language(), config.translation_language());
}

TEST(ConfigurationTest, load_float_theme_ignored)
{
    ConfigurationFile file;
    file.write(R"({"theme":1.5})");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::System, config.theme());
}

TEST(ConfigurationTest, load_comments_allowed)
{
    ConfigurationFile file;
    file.write("/* comment */\n{\n// line comment\n\"theme\":1\n}");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::Dark, config.theme());
}

TEST(ConfigurationTest, load_empty_file)
{
    ConfigurationFile file;
    file.write("");
    Configuration config{ file.name() };
    ASSERT_EQ(Theme::System, config.theme());
    ASSERT_TRUE(config.automatically_check_for_updates());
}

TEST(ConfigurationTest, load_invalid_json)
{
    ConfigurationFile file;
    file.write("{ this is not json");
    Configuration malformed{ file.name() };
    ASSERT_EQ(Theme::System, malformed.theme());
    ASSERT_TRUE(malformed.automatically_check_for_updates());
    ASSERT_FALSE(malformed.allow_preview_updates());
    ASSERT_EQ(Translator::follow_system_language(), malformed.translation_language());
    file.write("[1,2,3]");
    Configuration non_object{ file.name() };
    ASSERT_EQ(Theme::System, non_object.theme());
    ASSERT_TRUE(non_object.automatically_check_for_updates());
    ASSERT_FALSE(non_object.allow_preview_updates());
    ASSERT_EQ(Translator::follow_system_language(), non_object.translation_language());
}

TEST(ConfigurationTest, concurrent_setters)
{
    ConfigurationFile file;
    Configuration config{ file.name() };
    std::vector<std::thread> threads;
    for (int i{ 0 }; i < 4; i++)
    {
        threads.emplace_back([&config, i]()
        {
            for (int j{ 0 }; j < 25; j++)
            {
                config.set_theme(j % 2 == 0 ? Theme::Dark : Theme::Light);
                config.set_allow_preview_updates(j % 2 == 0);
                config.set_translation_language("lang_" + std::to_string(i));
            }
        });
    }
    for (std::thread& thread : threads)
    {
        thread.join();
    }
    nlohmann::json json = file.read();
    ASSERT_FALSE(json.is_discarded());
    Configuration reloaded{ file.name() };
    ASSERT_EQ(config.theme(), reloaded.theme());
    ASSERT_EQ(config.allow_preview_updates(), reloaded.allow_preview_updates());
    ASSERT_EQ(config.translation_language(), reloaded.translation_language());
}