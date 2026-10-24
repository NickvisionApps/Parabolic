#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "environment.h"
#include "translator.h"

using namespace parabolic;

namespace
{
    class EnvironmentVariableGuard
    {
    public:
        EnvironmentVariableGuard(std::string name) :
            m_name{ std::move(name) },
            m_original{ environment::variable(m_name) }
        {
        }

        ~EnvironmentVariableGuard()
        {
            environment::set_variable(m_name, m_original);
        }

        EnvironmentVariableGuard(const EnvironmentVariableGuard&) = delete;
        EnvironmentVariableGuard& operator=(const EnvironmentVariableGuard&) = delete;

    private:
        std::string m_name;
        std::string m_original;
    };

    class TranslatorDirectory
    {
    public:
        TranslatorDirectory()
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
            m_path = std::filesystem::temp_directory_path() / ("parabolic_test_" + suffix);
            std::filesystem::create_directories(m_path);
        }

        ~TranslatorDirectory()
        {
            std::error_code ec;
            std::filesystem::remove_all(m_path, ec);
        }

        TranslatorDirectory(const TranslatorDirectory&) = delete;
        TranslatorDirectory& operator=(const TranslatorDirectory&) = delete;

        [[nodiscard]] const std::filesystem::path& path() const
        {
            return m_path;
        }

        void add_language(const std::string& language, const std::string& domain) const
        {
            std::filesystem::path messages{ m_path / language / "LC_MESSAGES" };
            std::filesystem::create_directories(messages);
            std::ofstream file{ messages / (domain + ".mo"), std::ios::binary };
        }

    private:
        std::filesystem::path m_path;
    };
}

TEST(TranslatorTest, available_languages)
{
    TranslatorDirectory empty;
    Translator defaults{ "Test", empty.path() };
    std::vector<std::string> expected_defaults{ "en_US" };
    ASSERT_EQ(expected_defaults, defaults.available_languages());
    TranslatorDirectory directory;
    directory.add_language("fr_FR", "test");
    directory.add_language("de_DE", "test");
    directory.add_language("en_US", "test");
    directory.add_language("es_ES", "other");
    directory.add_language("pt_BR", "test");
    std::filesystem::remove(directory.path() / "pt_BR" / "LC_MESSAGES" / "test.mo");
    std::filesystem::create_directories(directory.path() / "it_IT");
    Translator translator{ "Test", directory.path() };
    std::vector<std::string> expected{ "de_DE", "en_US", "fr_FR" };
    ASSERT_EQ(expected, translator.available_languages());
}

TEST(TranslatorTest, set_language)
{
    EnvironmentVariableGuard language{ "LANGUAGE" };
    EnvironmentVariableGuard all{ "LC_ALL" };
    TranslatorDirectory directory;
    directory.add_language("fr", "test");
    directory.add_language("fr_FR", "test");
    Translator translator{ "Test", directory.path() };
    ASSERT_EQ("en_US", translator.set_language("en_US"));
    ASSERT_EQ("en_US", translator.language());
    ASSERT_EQ("en_US", environment::variable("LANGUAGE"));
    ASSERT_EQ("en_US", translator.set_language("en_US"));
    ASSERT_EQ("fr_FR", translator.set_language("fr-FR"));
    ASSERT_EQ("fr_FR", translator.set_language("fr_FR.UTF-8"));
    ASSERT_EQ("fr_FR", translator.set_language("fr_FR@euro"));
    ASSERT_EQ("en_US", translator.set_language("en_US.UTF-8"));
    ASSERT_EQ("en_US", translator.set_language("de_DE"));
    ASSERT_EQ("en_US", translator.language());
    ASSERT_EQ("en_US", environment::variable("LANGUAGE"));
    ASSERT_EQ("fr", translator.set_language("fr_CA"));
    ASSERT_EQ("fr", translator.language());
    ASSERT_EQ("fr", environment::variable("LANGUAGE"));
    environment::set_variable("LC_ALL", "fr_FR.UTF-8");
    ASSERT_EQ("fr_FR", translator.set_language(""));
    ASSERT_EQ("en_US", translator.set_language("en_US"));
    ASSERT_EQ("fr_FR", translator.set_language(Translator::follow_system_language()));
    environment::set_variable("LC_ALL", "C");
    ASSERT_EQ("en_US", translator.set_language(""));
}

TEST(TranslatorTest, gettext)
{
    EnvironmentVariableGuard guard{ "LANGUAGE" };
    TranslatorDirectory directory;
    Translator translator{ "Test", directory.path() };
    translator.set_language("en_US");
    ASSERT_STREQ("Hello", translator._("Hello"));
    ASSERT_STREQ("caf\xC3\xA9", translator._("caf\xC3\xA9"));
    ASSERT_EQ("Hello, World!", translator._("Hello, {}!", std::string{ "World" }));
    ASSERT_EQ("1 + 2 = 3", translator._("{} + {} = {}", 1, 2, 3));
    ASSERT_STREQ("file", translator._n("file", "files", 1));
    ASSERT_STREQ("files", translator._n("file", "files", 0));
    ASSERT_STREQ("files", translator._n("file", "files", 2));
    ASSERT_STREQ("files", translator._n("file", "files", 1000000));
    ASSERT_EQ("1 file", translator._n("{} file", "{} files", 1, 1));
    ASSERT_EQ("0 files", translator._n("{} file", "{} files", 0, 0));
    ASSERT_EQ("5 files", translator._n("{} file", "{} files", 5, 5));
    ASSERT_STREQ("Open", translator._p("menu", "Open"));
    ASSERT_STREQ("Open", translator._p("button", "Open"));
    ASSERT_EQ("Open report.txt", translator._p("menu", "Open {}", std::string{ "report.txt" }));
    ASSERT_STREQ("item", translator._pn("menu", "item", "items", 1));
    ASSERT_STREQ("items", translator._pn("menu", "item", "items", 0));
    ASSERT_STREQ("items", translator._pn("menu", "item", "items", 3));
    ASSERT_EQ("1 item", translator._pn("menu", "{} item", "{} items", 1, 1));
    ASSERT_EQ("3 items", translator._pn("menu", "{} item", "{} items", 3, 3));
}

TEST(TranslatorTest, gettext_wide)
{
    EnvironmentVariableGuard guard{ "LANGUAGE" };
    TranslatorDirectory directory;
    Translator translator{ "Test", directory.path() };
    translator.set_language("en_US");
    ASSERT_EQ(L"Hello, World!", translator.w_("Hello, {}!", std::string{ "World" }));
    ASSERT_EQ(L"1 file", translator.w_n("{} file", "{} files", 1, 1));
    ASSERT_EQ(L"2 files", translator.w_n("{} file", "{} files", 2, 2));
    ASSERT_EQ(L"Open report.txt", translator.w_p("menu", "Open {}", std::string{ "report.txt" }));
    ASSERT_EQ(L"1 item", translator.w_pn("menu", "{} item", "{} items", 1, 1));
    ASSERT_EQ(L"3 items", translator.w_pn("menu", "{} item", "{} items", 3, 3));
}

TEST(TranslatorTest, gettext_without_catalog)
{
    EnvironmentVariableGuard guard{ "LANGUAGE" };
    TranslatorDirectory directory;
    directory.add_language("fr_FR", "test");
    Translator translator{ "Test", directory.path() };
    translator.set_language("fr_FR");
    ASSERT_STREQ("Hello", translator._("Hello"));
    ASSERT_STREQ("items", translator._n("item", "items", 2));
    ASSERT_STREQ("Open", translator._p("menu", "Open"));
    ASSERT_STREQ("items", translator._pn("menu", "item", "items", 2));
}