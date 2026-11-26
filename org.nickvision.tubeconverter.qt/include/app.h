#ifndef APP_H
#define APP_H

#include <QApplication>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include "helpers/translation_notifier.h"

namespace parabolic::qt
{
    class App : public QApplication
    {
        Q_OBJECT

    public:
        App(int& argc, char** argv);
        ~App() override = default;
        App(const App&) = delete;
        App(App&&) = delete;
        [[nodiscard]] oclero::qlementine::ThemeManager* theme_manager() const;
        [[nodiscard]] helpers::TranslationNotifier* translation_notifier();
        int run();
        App& operator=(const App&) = delete;
        App& operator=(App&&) = delete;

    private:
        oclero::qlementine::ThemeManager* m_theme_manager{ nullptr };
        helpers::TranslationNotifier m_translation_notifier;
    };
}

#endif
