#include "application.h"
#include <curl/curl.h>
#include <format>
#include <memory>
#include <semver.hpp>
#include <string>
#include "configuration.h"
#include "discovery.h"
#include "history.h"
#include "keyring.h"
#include "translation.h"
#include "updates.h"

using namespace parabolic::configuration;
using namespace parabolic::translation;
using namespace parabolic::updates;

namespace parabolic::application
{
    const std::string& Product::id() const
    {
        return m_id;
    }

    void Product::set_id(const std::string& id)
    {
        m_id = id;
    }

    const std::string& Product::name() const
    {
        return m_name;
    }

    void Product::set_name(const std::string& name)
    {
        m_name = name;
    }

    const std::string& Product::short_name() const
    {
        return m_short_name;
    }

    void Product::set_short_name(const std::string& short_name)
    {
        m_short_name = short_name;
    }

    const std::string& Product::repo_owner() const
    {
        return m_repo_owner;
    }

    void Product::set_repo_owner(const std::string& repo_owner)
    {
        m_repo_owner = repo_owner;
    }

    const std::string& Product::repo_name() const
    {
        return m_repo_name;
    }

    void Product::set_repo_name(const std::string& repo_name)
    {
        m_repo_name = repo_name;
    }

    const std::string& Product::description() const
    {
        return m_description;
    }

    void Product::set_description(const std::string& description)
    {
        m_description = description;
    }

    const semver::version<>& Product::version() const
    {
        return m_version;
    }

    void Product::set_version(const semver::version<>& version)
    {
        m_version = version;
    }

    const std::string& Product::changelog() const
    {
        return m_changelog;
    }

    void Product::set_changelog(const std::string& changelog)
    {
        m_changelog = changelog;
    }

    const std::string& Product::repo_url() const
    {
        if (m_repo_url.empty())
        {
            m_repo_url = std::format("https://github.com/{}/{}", m_repo_owner, m_repo_name);
        }
        return m_repo_url;
    }

    const std::string& Product::issues_url() const
    {
        if (m_issues_url.empty())
        {
            m_issues_url = std::format("https://github.com/{}/{}/issues", m_repo_owner, m_repo_name);
        }
        return m_issues_url;
    }

    const std::string& Product::discussions_url() const
    {
        if (m_discussions_url.empty())
        {
            m_discussions_url = std::format("https://github.com/{}/{}/discussions", m_repo_owner, m_repo_name);
        }
        return m_discussions_url;
    }

    State::State()
    {
        curl_global_init(CURL_GLOBAL_ALL);
        m_product = std::make_unique<Product>();
        m_product->set_id("org.nickvision.tubeconverter");
        m_product->set_name("Nickvision Parabolic");
        m_product->set_short_name("Parabolic");
        m_product->set_repo_owner("NickvisionApps");
        m_product->set_repo_name("Parabolic");
        m_product->set_description("Download web video and audio");
        m_product->set_version(semver::from_string("2026.10.0-next"));
        m_product->set_changelog("- Initial Release");
        m_configuration = std::make_unique<Configuration>(m_product->name());
        m_translator = std::make_unique<Translator>(m_product->short_name());
        m_discovery = std::make_unique<discovery::Manager>();
        m_history = std::make_unique<history::Store>(m_product->name());
        m_keyring = std::make_unique<keyring::Store>(m_product->id());
#ifdef _WIN32
#ifdef _M_ARM64
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolicSetup-arm64.exe");
#else
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolicSetup-x64.exe");
#endif
#elifdef __APPLE__
#ifdef __aarch64__
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolic-arm64.dmg");
#else
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolic-x64.dmg");
#endif
#elifdef __linux__
#ifdef __aarch64__
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolic-arm64.AppImage");
#else
        m_updater = std::make_unique<Updater>(m_product->repo_owner(), m_product->repo_name(), m_product->version(),
                                              "NickvisionParabolic-x64.AppImage");
#endif
#endif
    }

    State::~State()
    {
        m_configuration.reset();
        m_discovery.reset();
        m_history.reset();
        m_keyring.reset();
        m_product.reset();
        m_translator.reset();
        m_updater.reset();
        curl_global_cleanup();
    }

    State& State::instance()
    {
        static State instance;
        return instance;
    }

    Configuration& State::configuration() const
    {
        return *m_configuration;
    }

    discovery::Manager& State::discovery() const
    {
        m_discovery->set_options(m_configuration->discovery_options());
        return *m_discovery;
    }

    history::Store& State::history() const
    {
        m_history->set_options(m_configuration->history_options());
        return *m_history;
    }

    keyring::Store& State::keyring() const
    {
        return *m_keyring;
    }

    const Product& State::product() const
    {
        return *m_product;
    }

    const Translator& State::translator() const
    {
        std::string config_language{ m_configuration->translation_language() };
        std::string resolved_language{ m_translator->set_language(config_language) };
        if (config_language != Translator::follow_system_language())
        {
            m_configuration->set_translation_language(resolved_language);
        }
        return *m_translator;
    }

    const Updater& State::updater() const
    {
        m_updater->set_allow_preview_updates(m_configuration->allow_preview_updates());
        return *m_updater;
    }
}
