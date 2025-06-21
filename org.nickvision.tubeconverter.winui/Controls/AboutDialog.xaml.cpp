#include "Controls/AboutDialog.xaml.h"
#if __has_include("Controls/AboutDialog.g.cpp")
#include "Controls/AboutDialog.g.cpp"
#endif
#include <chrono>
#include <vector>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::App;
using namespace ::Nickvision::Helpers;
using namespace winrt::Microsoft::UI::Dispatching;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::ApplicationModel::DataTransfer;

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    static std::vector<std::string> keys(const std::unordered_map<std::string, std::string>& map)
    {
        std::vector<std::string> k;
        for(const std::pair<const std::string, std::string>& pair : map)
        {
            k.push_back(pair.first);
        }
        return k;
    }

    AboutDialog::AboutDialog()
        : m_dispatcherQueueTimer{ nullptr }
    {
        InitializeComponent();
        //Localize Strings
        CloseButtonText(winrt::to_hstring(_("OK")));
        NavAbout().Text(winrt::to_hstring(_("About")));
        NavChangelog().Text(winrt::to_hstring(_("Changelog")));
        NavCredits().Text(winrt::to_hstring(_("Credits")));
        NavDebugging().Text(winrt::to_hstring(_("Debugging")));
        LblDevelopersTitle().Text(winrt::to_hstring(_("Developers")));
        LblDesignersTitle().Text(winrt::to_hstring(_("Designers")));
        LblArtistsTitle().Text(winrt::to_hstring(_("Artists")));
        LblCopyDebugInfo().Text(winrt::to_hstring(_("Copy Debug Information")));
    }

    void AboutDialog::Info(const AppInfo& appInfo, const std::string& debugInfo)
    {
        Title(winrt::box_value(winrt::to_hstring(appInfo.getShortName())));
        LblDescription().Text(winrt::to_hstring(appInfo.getDescription()));
        LblVersion().Text(winrt::to_hstring(appInfo.getVersion().str()));
        LblChangelog().Text(winrt::to_hstring(appInfo.getChangelog()));
        LblDevelopers().Text(winrt::to_hstring(StringHelpers::join(keys(appInfo.getDevelopers()), "\n")));
        LblDesigners().Text(winrt::to_hstring(StringHelpers::join(keys(appInfo.getDesigners()), "\n")));
        LblArtists().Text(winrt::to_hstring(StringHelpers::join(keys(appInfo.getArtists()), "\n")));
        LblDebugInfo().Text(winrt::to_hstring(debugInfo));
    }

    void AboutDialog::OnNavViewSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStack().CurrentPageIndex(static_cast<int>(index));
        }
    }

    void AboutDialog::CopyDebugInfo(const IInspectable& sender, const RoutedEventArgs& args)
    {
        DataPackage dataPackage;
        dataPackage.SetText(LblDebugInfo().Text());
        Clipboard::SetContent(dataPackage);
        IcnCopyDebugInfo().Glyph(L"\uE73E");
        m_dispatcherQueueTimer = DispatcherQueue().CreateTimer();
        m_dispatcherQueueTimer.IsRepeating(false);
        m_dispatcherQueueTimer.Interval(std::chrono::seconds(1));
        m_dispatcherQueueTimer.Tick([this](const DispatcherQueueTimer&, const IInspectable&)
        {
            IcnCopyDebugInfo().Glyph(L"\uE8C8");
        });
        m_dispatcherQueueTimer.Start();
    }
}
