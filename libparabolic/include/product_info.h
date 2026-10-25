#pragma once

#include <semver.hpp>
#include <string>

namespace parabolic::product_info
{
    class ProductInfo
    {
    public:
        ProductInfo() = default;
        [[nodiscard]] const std::string& id();
        void set_id(const std::string& id);
        [[nodiscard]] const std::string& name();
        void set_name(const std::string& name);
        [[nodiscard]] const std::string& short_name();
        void set_short_name(const std::string& short_name);
        [[nodiscard]] const std::string& repo_owner();
        void set_repo_owner(const std::string& repo_owner);
        [[nodiscard]] const std::string& repo_name();
        void set_repo_name(const std::string& repo_name);
        [[nodiscard]] const std::string& description();
        void set_description(const std::string& description);
        [[nodiscard]] const semver::version<>& version();
        void set_version(const semver::version<>& version);
        [[nodiscard]] const std::string& changelog();
        void set_changelog(const std::string& changelog);
        [[nodiscard]] const std::string& repo_url();
        [[nodiscard]] const std::string& issues_url();
        [[nodiscard]] const std::string& discussions_url();

    private:
        std::string m_id;
        std::string m_name;
        std::string m_short_name;
        std::string m_repo_owner;
        std::string m_repo_name;
        std::string m_description;
        semver::version<> m_version;
        std::string m_changelog;
        std::string m_repo_url;
        std::string m_issues_url;
        std::string m_discussions_url;
    };

}
