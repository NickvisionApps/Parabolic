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
            ProgBar().IsIndeterminate(true);
        }
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
        if(ProgBar().Value() == 0.0)
        {
            ProgBar().IsIndeterminate(true);
        }
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

    }

    void DownloadRow::PauseResume(const IInspectable& sender, const RoutedEventArgs& args)
    {

    }

    void DownloadRow::Stop(const IInspectable& sender, const RoutedEventArgs& args)
    {

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

    }
}
