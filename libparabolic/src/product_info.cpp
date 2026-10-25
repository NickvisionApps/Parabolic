#include "product_info.h"
#include <format>
#include <semver.hpp>
#include <string>

namespace parabolic::product_info
{
    const std::string& ProductInfo::id()
    {
        return m_id;
    }

    void ProductInfo::set_id(const std::string& id)
    {
        m_id = id;
    }

    const std::string& ProductInfo::name()
    {
        return m_name;
    }

    void ProductInfo::set_name(const std::string& name)
    {
        m_name = name;
    }

    const std::string& ProductInfo::short_name()
    {
        return m_short_name;
    }

    void ProductInfo::set_short_name(const std::string& short_name)
    {
        m_short_name = short_name;
    }

    const std::string& ProductInfo::repo_owner()
    {
        return m_repo_owner;
    }

    void ProductInfo::set_repo_owner(const std::string& repo_owner)
    {
        m_repo_owner = repo_owner;
    }

    const std::string& ProductInfo::repo_name()
    {
        return m_repo_name;
    }

    void ProductInfo::set_repo_name(const std::string& repo_name)
    {
        m_repo_name = repo_name;
    }

    const std::string& ProductInfo::description()
    {
        return m_description;
    }

    void ProductInfo::set_description(const std::string& description)
    {
        m_description = description;
    }

    const semver::version<>& ProductInfo::version()
    {
        return m_version;
    }

    void ProductInfo::set_version(const semver::version<>& version)
    {
        m_version = version;
    }

    const std::string& ProductInfo::changelog()
    {
        return m_changelog;
    }

    void ProductInfo::set_changelog(const std::string& changelog)
    {
        m_changelog = changelog;
    }

    const std::string& ProductInfo::repo_url()
    {
        if (m_repo_url.empty())
        {
            m_repo_url = std::format("https://github.com/{}/{}", m_repo_owner, m_repo_name);
        }
        return m_repo_url;
    }

    const std::string& ProductInfo::issues_url()
    {
        if (m_issues_url.empty())
        {
            m_issues_url = std::format("https://github.com/{}/{}/issues", m_repo_owner, m_repo_name);
        }
        return m_issues_url;
    }

    const std::string& ProductInfo::discussions_url()
    {
        if (m_discussions_url.empty())
        {
            m_discussions_url = std::format("https://github.com/{}/{}/discussions", m_repo_owner, m_repo_name);
        }
        return m_discussions_url;
    }
}
