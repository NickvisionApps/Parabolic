#ifndef DOWNLOADROW_H
#define DOWNLOADROW_H

#include "pch.h"
#include "Controls/ViewStack.g.h"
#include "Controls/DownloadRow.g.h"
#include <filesystem>
#include "events/downloadaddedeventargs.h"
#include "events/downloadcompletedeventargs.h"
#include "events/downloadprogresschangedeventargs.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A row control to display and manage a download.
     */
    struct DownloadRow : public DownloadRowT<DownloadRow>
    {
    public:
        /**
         * @brief Constructs a DownloadRow
         */
        DownloadRow();
        /**
         * @brief Triggers the added state.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadAddedEventArgs
         */
        void TriggerAddedState(const ::Nickvision::TubeConverter::Shared::Events::DownloadAddedEventArgs& args);
        /**
         * @brief Triggers the progress state.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadProgressChangedEventArgs
         */
        void TriggerProgressState(const ::Nickvision::TubeConverter::Shared::Events::DownloadProgressChangedEventArgs& args);
        /**
         * @brief Triggers the completed state.
         * @param args Nickvision::TubeConverter::Shared::Events::DownloadCompletedEventArgs
         */
        void TriggerCompletedState(const ::Nickvision::TubeConverter::Shared::Events::DownloadCompletedEventArgs& args);
        /**
         * @brief Triggers the stopped state.
         */
        void TriggerStoppedState();
        /**
         * @brief Triggers the paused state.
         */
        void TriggerPausedState();
        /**
         * @brief Triggers the resumed state.
         */
        void TriggerResumedState();
        /**
         * @brief Triggers the started from queue state.
         */
        void TriggerStartedFromQueueState();
        /**
         * @brief Shows the download log.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void ShowLog(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Pauses/resumes the download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void PauseResume(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Stops the download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void Stop(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Plays the downloaded media.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction Play(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Opens the containing folder of the downloaded media.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        Windows::Foundation::IAsyncAction OpenFolder(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);
        /**
         * @brief Retries the download.
         * @param sender IInspectable
         * @param args Microsoft::UI::Xaml::RoutedEventArgs
         */
        void Retry(const IInspectable& sender, const Microsoft::UI::Xaml::RoutedEventArgs& args);

    private:
        int m_id;
        std::filesystem::path m_path;
        bool m_isPaused;
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct DownloadRow : public DownloadRowT<DownloadRow, implementation::DownloadRow> { };
}

#endif //DOWNLOADROW_H
