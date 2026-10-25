#include "application_state.h"
#include <curl/curl.h>
#include <memory>
#include <semver.hpp>
#include "configuration.h"
#include "product_info.h"
#include "translator.h"
#include "updater.h"

using namespace parabolic::configuration;
using namespace parabolic::product_info;
using namespace parabolic::translator;
using namespace parabolic::updater;

namespace parabolic::application_state
{
    ApplicationState::ApplicationState()
    {
        curl_global_init(CURL_GLOBAL_ALL);
        m_product_info = std::make_unique<ProductInfo>();
        m_product_info->set_id("org.nickvision.tubeconverter");
        m_product_info->set_name("Nickvision Parabolic");
        m_product_info->set_short_name("Parabolic");
        m_product_info->set_repo_owner("NickvisionApps");
        m_product_info->set_repo_name("Parabolic");
        m_product_info->set_description("Download web video and audio");
        m_product_info->set_version(semver::from_string("2026.10.0-next"));
        m_product_info->set_changelog("- Initial Release");
        m_configuration = std::make_unique<Configuration>(m_product_info->name());
        m_translator = std::make_unique<Translator>(m_product_info->short_name());
#ifdef _WIN32
#ifdef _M_ARM64
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolicSetup-arm64.exe");
#else
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolicSetup-x64.exe");
#endif
#elifdef __APPLE__
#ifdef __aarch64__
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolic-arm64.dmg");
#else
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolic-x64.dmg");
#endif
#elifdef __linux__
#ifdef __aarch64__
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolic-arm64.AppImage");
#else
        m_updater = std::make_unique<Updater>(m_product_info->repo_owner(), m_product_info->repo_name(),
                                              m_product_info->version(), "NickvisionParabolic-x64.AppImage");
#endif
#endif
    }

    ApplicationState::~ApplicationState()
    {
        m_product_info.reset();
        m_configuration.reset();
        m_translator.reset();
        m_updater.reset();
        curl_global_cleanup();
    }

    ApplicationState& ApplicationState::instance()
    {
        static ApplicationState instance;
        return instance;
    }

    Configuration& ApplicationState::configuration() const
    {
        return *m_configuration;
    }

    Translator& ApplicationState::translator() const
    {
        std::string config_language{ m_configuration->translation_language() };
        std::string resolved_language{ m_translator->set_language(config_language) };
        if (config_language != Translator::follow_system_language())
        {
            m_configuration->set_translation_language(resolved_language);
        }
        return *m_translator;
    }

    ProductInfo& ApplicationState::product_info() const
    {
        return *m_product_info;
    }

    Updater& ApplicationState::updater() const
    {
        m_updater->set_allow_preview_updates(m_configuration->allow_preview_updates());
        return *m_updater;
    }
}
