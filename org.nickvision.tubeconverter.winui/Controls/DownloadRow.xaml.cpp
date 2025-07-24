#include "Controls/DownloadRow.xaml.h"
#if __has_include("Controls/DownloadRow.g.cpp")
#include "Controls/DownloadRow.g.cpp"
#endif
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::TubeConverter::Shared::Events;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::System;

enum ButtonsPage
{
    Running = 0,
    Done,
    Error
};

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    DownloadRow::DownloadRow()
        : m_id{ -1 },
        m_isPaused{ false }
    {
        InitializeComponent();
        //Localize Strings
        ToolTipService::SetToolTip(BtnShowLog(), winrt::box_value(winrt::to_hstring(_("Show Log"))));
        LblShowLog().Text(winrt::to_hstring(_("Log")));
        LblPauseResume().Text(winrt::to_hstring(_("Pause")));
        LblStop().Text(winrt::to_hstring(_("Stop")));
        LblPlay().Text(winrt::to_hstring(_("Play")));
        ToolTipService::SetToolTip(BtnOpenFolder(), winrt::box_value(winrt::to_hstring(_("Open Containing Folder"))));
        LblOpenFolder().Text(winrt::to_hstring(_("Open")));
        LblRetry().Text(winrt::to_hstring(_("Retry")));
    }

    winrt::event_token DownloadRow::PauseRequested(const Microsoft::UI::Xaml::RoutedEventHandler& handler)
    {
        return m_pauseRequestedEvent.add(handler);
    }

    void DownloadRow::PauseRequested(const winrt::event_token& token)
    {
        m_pauseRequestedEvent.remove(token);
    }

    winrt::event_token DownloadRow::ResumeRequested(const Microsoft::UI::Xaml::RoutedEventHandler& handler)
    {
        return m_resumeRequestedEvent.add(handler);
    }

    void DownloadRow::ResumeRequested(const winrt::event_token& token)
    {
        m_resumeRequestedEvent.remove(token);
    }

    winrt::event_token DownloadRow::StopRequested(const Microsoft::UI::Xaml::RoutedEventHandler& handler)
    {
        return m_stopRequestedEvent.add(handler);
    }

    void DownloadRow::StopRequested(const winrt::event_token& token)
    {
        m_stopRequestedEvent.remove(token);
    }

    winrt::event_token DownloadRow::RetryRequested(const Microsoft::UI::Xaml::RoutedEventHandler& handler)
    {
        return m_retryRequestedEvent.add(handler);
    }

    void DownloadRow::RetryRequested(const winrt::event_token& token)
    {
        m_retryRequestedEvent.remove(token);
    }

    void DownloadRow::TriggerAddedState(const DownloadAddedEventArgs& args)
    {
        m_id = args.getId();
        m_path = args.getPath();
        IcnStatus().Glyph(L"\uE896");
        LblTitle().Text(winrt::to_hstring(m_path.filename().string()));
        ProgBar().Value(0.0);
        if(args.getStatus() == DownloadStatus::Queued)
        {
            LblStatus().Text(winrt::to_hstring(_("Queued")));
            ProgBar().IsIndeterminate(false);
        }
        else if(args.getStatus() == DownloadStatus::Running)
        {
            LblStatus().Text(winrt::to_hstring(_("Running")));
            ProgBar().IsIndeterminate(true);
        }
        else
        {
            LblStatus().Text(winrt::to_hstring(_("Unknown")));
            ProgBar().IsIndeterminate(true);
        }
        ViewStackButtons().CurrentPageIndex(ButtonsPage::Running);
    }

    void DownloadRow::TriggerProgressState(const DownloadProgressChangedEventArgs& args)
    {
        IcnStatus().Glyph(L"\uE896");
        if(std::isnan(args.getProgress()))
        {
            LblStatus().Text(winrt::to_hstring(_("Processing")));
            ProgBar().Value(0.0);
            ProgBar().IsIndeterminate(true);
        }
        else
        {
            LblStatus().Text(winrt::to_hstring(_f("{} | {} | ETA: {}", _("Running"), args.getSpeedStr(), args.getEtaStr())));
            ProgBar().Value(args.getProgress());
            ProgBar().IsIndeterminate(false);
        }
        LblLog().Text(winrt::to_hstring(args.getLog()));
    }

    void DownloadRow::TriggerCompletedState(const DownloadCompletedEventArgs& args)
    {
        m_path = args.getPath();
        LblTitle().Text(winrt::to_hstring(m_path.filename().string()));
        ProgBar().Value(1.0);
        ProgBar().IsIndeterminate(false);
        if(args.getStatus() == DownloadStatus::Error)
        {
            IcnStatus().Glyph(L"\uEA39");
            LblStatus().Text(winrt::to_hstring(_("Error")));
            ViewStackButtons().CurrentPageIndex(ButtonsPage::Error);
        }
        else if(args.getStatus() == DownloadStatus::Success)
        {
            IcnStatus().Glyph(L"\uE930");
            LblStatus().Text(winrt::to_hstring(_("Success")));
            ViewStackButtons().CurrentPageIndex(ButtonsPage::Done);
        }
    }

    void DownloadRow::TriggerStoppedState()
    {
        IcnStatus().Glyph(L"\uE783");
        LblStatus().Text(winrt::to_hstring(_("Stopped")));
        ProgBar().Value(1.0);
        ProgBar().IsIndeterminate(false);
        ViewStackButtons().CurrentPageIndex(ButtonsPage::Error);
    }

    void DownloadRow::TriggerPausedState()
    {
        IcnStatus().Glyph(L"\uE769");
        LblStatus().Text(winrt::to_hstring(_("Paused")));
        ProgBar().IsIndeterminate(false);
        FntPauseResume().Glyph(L"\uE768");
        LblPauseResume().Text(winrt::to_hstring(_("Resume")));
    }

    void DownloadRow::TriggerResumedState()
    {
        FntPauseResume().Glyph(L"\uE769");
        LblPauseResume().Text(winrt::to_hstring(_("Pause")));
    }

    void DownloadRow::TriggerStartedFromQueueState()
    {
        LblStatus().Text(winrt::to_hstring(_("Running")));
        ProgBar().Value(0.0);
        ProgBar().IsIndeterminate(true);
    }

    void DownloadRow::ShowLog(const IInspectable& sender, const RoutedEventArgs& args)
    {
        GridLog().Visibility(BtnShowLog().IsChecked().Value() ? Visibility::Visible : Visibility::Collapsed);
    }

    void DownloadRow::PauseResume(const IInspectable& sender, const RoutedEventArgs& args)
    {
        if(m_isPaused)
        {
            m_resumeRequestedEvent(sender, args);
        }
        else
        {
            m_pauseRequestedEvent(sender, args);
        }
        m_isPaused = !m_isPaused;
    }

    void DownloadRow::Stop(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_stopRequestedEvent(sender, args);
    }

    Windows::Foundation::IAsyncAction DownloadRow::Play(const IInspectable& sender, const RoutedEventArgs& args)
    {
        StorageFile file{ co_await StorageFile::GetFileFromPathAsync(winrt::to_hstring(m_path.string())) };
        co_await Launcher::LaunchFileAsync(file);
    }

    Windows::Foundation::IAsyncAction DownloadRow::OpenFolder(const IInspectable& sender, const RoutedEventArgs& args)
    {
        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(winrt::to_hstring(m_path.parent_path().string())) };
        co_await Launcher::LaunchFolderAsync(folder);
    }

    void DownloadRow::Retry(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_retryRequestedEvent(sender, args);
    }
}
