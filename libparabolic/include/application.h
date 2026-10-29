#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <semver.hpp>
#include <string>
#include "configuration.h"
#include "discovery.h"
#include "download.h"
#include "history.h"
#include "keyring.h"
#include "translation.h"
#include "update.h"

namespace parabolic::application
{
    class Product
    {
    public:
        Product() = default;
        [[nodiscard]] const std::string& id() const;
        void set_id(const std::string& id);
        [[nodiscard]] const std::string& name() const;
        void set_name(const std::string& name);
        [[nodiscard]] const std::string& short_name() const;
        void set_short_name(const std::string& short_name);
        [[nodiscard]] const std::string& repo_owner() const;
        void set_repo_owner(const std::string& repo_owner);
        [[nodiscard]] const std::string& repo_name() const;
        void set_repo_name(const std::string& repo_name);
        [[nodiscard]] const std::string& description() const;
        void set_description(const std::string& description);
        [[nodiscard]] const semver::version<>& version() const;
        void set_version(const semver::version<>& version);
        [[nodiscard]] const std::string& changelog() const;
        void set_changelog(const std::string& changelog);
        [[nodiscard]] const std::string& repo_url() const;
        [[nodiscard]] const std::string& issues_url() const;
        [[nodiscard]] const std::string& discussions_url() const;

    private:
        std::string m_id;
        std::string m_name;
        std::string m_short_name;
        std::string m_repo_owner;
        std::string m_repo_name;
        std::string m_description;
        semver::version<> m_version;
        std::string m_changelog;
        mutable std::string m_repo_url;
        mutable std::string m_issues_url;
        mutable std::string m_discussions_url;
    };

    class State
    {
    public:
        State();
        State(const State&) = delete;
        State(State&&) = delete;
        ~State();
        [[nodiscard]] static State& instance();
        [[nodiscard]] configuration::Configuration& configuration() const;
        [[nodiscard]] discovery::Manager& discovery() const;
        [[nodiscard]] download::Manager& downloads() const;
        [[nodiscard]] history::Store& history() const;
        [[nodiscard]] keyring::Store& keyring() const;
        [[nodiscard]] const Product& product() const;
        [[nodiscard]] const translation::Translator& translator() const;
        [[nodiscard]] const update::Updater& updater() const;
        State& operator=(const State&) = delete;
        State& operator=(State&&) = delete;

    private:
        std::unique_ptr<configuration::Configuration> m_configuration;
        std::unique_ptr<discovery::Manager> m_discovery;
        std::unique_ptr<download::Manager> m_downloads;
        std::unique_ptr<history::Store> m_history;
        std::unique_ptr<keyring::Store> m_keyring;
        std::unique_ptr<Product> m_product;
        std::unique_ptr<translation::Translator> m_translator;
        std::unique_ptr<update::Updater> m_updater;
    };
}

#endif
