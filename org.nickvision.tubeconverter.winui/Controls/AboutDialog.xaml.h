#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "pch.h"
#include "Controls/AboutDialog.g.h"
#include "Controls/ViewStack.g.h"
#include <libnick/app/appinfo.h>

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A dialog for showing application information.
     */
    struct AboutDialog : public AboutDialogT<AboutDialog>
    {
    public:
        /**
         * @brief Constructs a AboutDialog.
         */
        AboutDialog();
        /**
         * @brief Sets the information to display.
         * @param appInfo The AppInfo object for the application
         * @param debugInfo The debug information for the application
         */
        void Info(const ::Nickvision::App::AppInfo& appInfo, const std::string& debugInfo);
        /**
         * @brief Handles when the navigation view selection changes.
         * @param sender SelectorBar
         * @param args SelectorItemInvokedEventArgs
         */
        void OnNavViewSelectionChanged(const Microsoft::UI::Xaml::Controls::SelectorBar& sender, const Microsoft::UI::Xaml::Controls::SelectorBarSelectionChangedEventArgs& args);
        /**
         * @brief Copies the debug information to the clipboard.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void CopyDebugInfo(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        Microsoft::UI::Dispatching::DispatcherQueueTimer m_dispatcherQueueTimer;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct AboutDialog : public AboutDialogT<AboutDialog, implementation::AboutDialog> { };
}

#endif //ABOUTDIALOG_H
