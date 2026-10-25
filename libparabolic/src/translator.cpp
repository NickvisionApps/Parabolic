#include "translator.h"
#include <algorithm>
#include <libintl.h>
#include <utility>
#include "environment.h"
#include "string_manip.h"

#define FOLLOW_SYSTEM_LANGUAGE "C"

namespace
{
    std::string normalize_language(std::string language)
    {
        language = language.substr(0, language.find('.'));
        language = language.substr(0, language.find('@'));
        std::ranges::replace(language, '-', '_');
        if (language.empty() || language == FOLLOW_SYSTEM_LANGUAGE || language == "POSIX")
        {
            return "en_US";
        }
        return language;
    }
}

namespace parabolic::translator
{
    Translator::Translator(std::string_view app_short_name, std::filesystem::path locale_dir)
        : m_domain_name{ string_manip::lower(app_short_name) },
          m_locale_dir{ !locale_dir.empty() ? std::move(locale_dir) : environment::executable_directory() },
          m_available_languages{ "en_US" },
          m_language{ "en_US" }
    {
        for (const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(m_locale_dir))
        {
            if (entry.is_directory() && std::filesystem::exists(entry.path() / "LC_MESSAGES" / (m_domain_name + ".mo")))
            {
                std::string name{ entry.path().filename().string() };
                if (std::ranges::find(m_available_languages, name) == m_available_languages.end())
                {
                    m_available_languages.push_back(std::move(name));
                }
            }
        }
        std::ranges::sort(m_available_languages);
        std::setlocale(LC_ALL, "");
#ifdef _WIN32
        wbindtextdomain(m_domain_name.c_str(), m_locale_dir.c_str());
#else
        bindtextdomain(m_domain_name.c_str(), m_locale_dir.string().c_str());
#endif
        bind_textdomain_codeset(m_domain_name.c_str(), "UTF-8");
        textdomain(m_domain_name.c_str());
    }

    const std::string& Translator::follow_system_language()
    {
        static std::string follow{ FOLLOW_SYSTEM_LANGUAGE };
        return follow;
    }

    const std::vector<std::string>& Translator::available_languages() const
    {
        return m_available_languages;
    }

    std::string Translator::language() const
    {
        std::scoped_lock lock{ m_mutex };
        return m_language;
    }

    std::string Translator::set_language(std::string_view language)
    {
        std::string resolved;
        if (language.empty() || language == FOLLOW_SYSTEM_LANGUAGE)
        {
            resolved = normalize_language(environment::locale());
            for (const char* name: { "LC_ALL", "LC_MESSAGES", "LANG" })
            {
                std::string env{ environment::variable(name) };
                if (!env.empty())
                {
                    resolved = normalize_language(env);
                    break;
                }
            }
        }
        else
        {
            resolved = normalize_language(std::string(language));
        }
        if (resolved != "en_US" && std::ranges::find(m_available_languages, resolved) == m_available_languages.end())
        {
            std::string base{ resolved.substr(0, resolved.find('_')) };
            if (base != resolved && std::ranges::find(m_available_languages, base) != m_available_languages.end())
            {
                resolved = base;
            }
            else
            {
                resolved = "en_US";
            }
        }
        std::scoped_lock lock{ m_mutex };
        environment::set_variable("LANGUAGE", resolved);
        m_language = resolved;
        return m_language;
    }

    const char* Translator::_(const char* msgid) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return msgid;
        }
        return ::dgettext(m_domain_name.c_str(), msgid);
    }

    const char* Translator::_n(const char* msgid, const char* msgid_plural, unsigned long n) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return n == 1 ? msgid : msgid_plural;
        }
        return ::dngettext(m_domain_name.c_str(), msgid, msgid_plural, n);
    }

    const char* Translator::_p(const char* context, const char* msgid) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return msgid;
        }
        std::string ctx{ std::string{ context } + "\004" + msgid };
        const char* result{ ::dcgettext(m_domain_name.c_str(), ctx.c_str(), LC_MESSAGES) };
        return result == ctx.c_str() ? msgid : result;
    }

    const char* Translator::_pn(const char* context, const char* msgid, const char* msgid_plural, unsigned long n) const
    {
        std::scoped_lock lock{ m_mutex };
        if (m_language == "en_US")
        {
            return n == 1 ? msgid : msgid_plural;
        }
        std::string ctx{ std::string{ context } + "\004" + msgid };
        const char* result{ ::dcngettext(m_domain_name.c_str(), ctx.c_str(), msgid_plural, n, LC_MESSAGES) };
        if (result == ctx.c_str() || result == msgid_plural)
        {
            return n == 1 ? msgid : msgid_plural;
        }
        return result;
    }
}
