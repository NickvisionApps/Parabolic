#pragma once

#include <memory>
#include "configuration.h"
#include "translator.h"
#include "product_info.h"
#include "updater.h"

namespace parabolic
{
    class ApplicationState
    {
    public:
        ApplicationState();
        ApplicationState(const ApplicationState&) = delete;
        ApplicationState(ApplicationState&&) = delete;
        ~ApplicationState();
        [[nodiscard]] static ApplicationState& instance();
        [[nodiscard]] Configuration& configuration() const;
        [[nodiscard]] Translator& translator() const;
        [[nodiscard]] ProductInfo& product_info() const;
        [[nodiscard]] Updater& updater() const;
        ApplicationState& operator=(const ApplicationState&) = delete;
        ApplicationState& operator=(ApplicationState&&) = delete;

    private:
        std::unique_ptr<Configuration> m_configuration;
        std::unique_ptr<Translator> m_translator;
        std::unique_ptr<ProductInfo> m_product_info;
        std::unique_ptr<Updater> m_updater;
    };
}
