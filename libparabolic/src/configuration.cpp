#include "configuration.h"
#include <fstream>
#include <sago/platform_folders.h>
#include "environment.h"
#include "product_info.h"
#include "translator.h"
#ifdef _WIN32
#include "string_manip.h"
#endif

using namespace parabolic::translator;

namespace parabolic::configuration
{
    Configuration::Configuration(const std::string& app_name)
#ifdef _WIN32
        : m_path{ environment::is_portable() ? environment::executable_directory() / "config.json"
                                             : std::filesystem::path(string_manip::wstr(sago::getConfigHome())) /
                                                       app_name / "config.json" },
#else
        : m_path{
              environment::is_portable() ? environment::executable_directory() / "config.json"
                                         : std::filesystem::path(sago::getConfigHome()) / app_name / "config.json",
          },
#endif
          m_translation_language{ Translator::follow_system_language() }
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
        catch (...)
        {
        }
    }

    Theme Configuration::theme() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_theme;
    }

    void Configuration::set_theme(Theme value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_theme)
        {
            return;
        }
        m_theme = value;
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
        if (value == m_automatically_check_for_updates)
        {
            return;
        }
        m_automatically_check_for_updates = value;
        save();
    }

    bool Configuration::allow_preview_updates() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_allow_preview_updates;
    }

    void Configuration::set_allow_preview_updates(bool value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_allow_preview_updates)
        {
            return;
        }
        m_allow_preview_updates = value;
        save();
    }

    std::string Configuration::translation_language() const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_translation_language.empty())
        {
            return Translator::follow_system_language();
        }
        return m_translation_language;
    }

    void Configuration::set_translation_language(const std::string& value)
    {
        std::scoped_lock lock{ m_mutex };
        if (value == m_translation_language ||
            (m_translation_language == Translator::follow_system_language() && value.empty()))
        {
            return;
        }
        if (value.empty())
        {
            m_translation_language = Translator::follow_system_language();
        }
        else
        {
            m_translation_language = value;
        }
        save();
    }

    std::string Configuration::str() const
    {
        std::scoped_lock lock{ m_mutex };
        nlohmann::json json = *this;
        return json.dump(4);
    }

    void Configuration::save() const
    {
        try
        {
            std::filesystem::create_directories(m_path.parent_path());
            std::ofstream file{ m_path, std::ios::out | std::ios::trunc };
            if (file)
            {
                nlohmann::json json = *this;
                file << json.dump();
            }
        }
        catch (...)
        {
        }
    }
}
