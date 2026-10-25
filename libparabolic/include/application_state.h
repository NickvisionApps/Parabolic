#pragma once

#include <memory>
#include "configuration.h"
#include "product_info.h"
#include "translator.h"
#include "updater.h"

namespace parabolic::application_state
{
    class ApplicationState
    {
    public:
        ApplicationState();
        ApplicationState(const ApplicationState&) = delete;
        ApplicationState(ApplicationState&&) = delete;
        ~ApplicationState();
        [[nodiscard]] static ApplicationState& instance();
        [[nodiscard]] configuration::Configuration& configuration() const;
        [[nodiscard]] translator::Translator& translator() const;
        [[nodiscard]] product_info::ProductInfo& product_info() const;
        [[nodiscard]] updater::Updater& updater() const;
        ApplicationState& operator=(const ApplicationState&) = delete;
        ApplicationState& operator=(ApplicationState&&) = delete;

    private:
        std::unique_ptr<configuration::Configuration> m_configuration;
        std::unique_ptr<translator::Translator> m_translator;
        std::unique_ptr<product_info::ProductInfo> m_product_info;
        std::unique_ptr<updater::Updater> m_updater;
    };
}
