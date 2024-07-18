#include "AddDownloadDialog.xaml.h"
#if __has_include("AddDownloadDialog.g.cpp")
#include "AddDownloadDialog.g.cpp"
#endif
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>

using namespace ::Nickvision::Events;
using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::Keyring;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Windows::ApplicationModel::DataTransfer;

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    AddDownloadDialog::AddDownloadDialog()
    {
        InitializeComponent();
        //Localize Strings
        Title(winrt::box_value(winrt::to_hstring(_("Add Download"))));
        PrimaryButtonText(winrt::to_hstring(_("Validate")));
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        DefaultButton(ContentDialogButton::Primary);
        TxtUrl().Header(winrt::box_value(winrt::to_hstring(_("Media URL"))));
        TxtUrl().PlaceholderText(winrt::to_hstring(_("Enter media url here")));
        CmbAuthenticate().Header(winrt::box_value(winrt::to_hstring(_("Authentication"))));
        TxtUsername().Header(winrt::box_value(winrt::to_hstring(_("Username"))));
        TxtUsername().PlaceholderText(winrt::to_hstring(_("Enter username here")));
        TxtPassword().Header(winrt::box_value(winrt::to_hstring(_("Password"))));
        TxtPassword().PlaceholderText(winrt::to_hstring(_("Enter password here")));
        CmbFileTypeSingle().Header(winrt::box_value(winrt::to_hstring(_("File Type"))));
        CmbQualitySingle().Header(winrt::box_value(winrt::to_hstring(_("Quality"))));
        CmbAudioLanguageSingle().Header(winrt::box_value(winrt::to_hstring(_("Audio Language"))));
        TglDownloadSubtitlesSingle().Header(winrt::box_value(winrt::to_hstring(_("Download Subtitles"))));
        TglPreferAV1Single().Header(winrt::box_value(winrt::to_hstring(_("Prefer AV1 Codec"))));
        TglSplitChaptersSingle().Header(winrt::box_value(winrt::to_hstring(_("Split Video by Chapters"))));
        TxtSaveFolderSingle().Header(winrt::box_value(winrt::to_hstring(_("Save Folder"))));
        ToolTipService::SetToolTip(BtnSelectSaveFolderSingle(), winrt::box_value(winrt::to_hstring(_("Select Save Folder"))));
        TxtFilenameSingle().Header(winrt::box_value(winrt::to_hstring(_("Filename"))));
        TxtFilenameSingle().PlaceholderText(winrt::to_hstring(_("Enter filename here")));
        ToolTipService::SetToolTip(BtnRevertFilenameSingle(), winrt::box_value(winrt::to_hstring(_("Revert to Title"))));
        TxtTimeFrameStartSingle().Header(winrt::box_value(winrt::to_hstring(_("Start Time"))));
        TxtTimeFrameEndSingle().Header(winrt::box_value(winrt::to_hstring(_("End Time"))));
        TglLimitSpeedSingle().Header(winrt::box_value(winrt::to_hstring(_("Limit Download Speed"))));
    }

    void AddDownloadDialog::SetController(const std::shared_ptr<AddDownloadDialogController>& controller, HWND hwnd)
    {
        m_controller = controller;
        m_hwnd = hwnd;
        //Register Events
        m_controller->urlValidated() += [&](const ParamEventArgs<bool>& args) { DispatcherQueue().TryEnqueue([this, args](){ OnUrlValidated(args); }); };
    }

    Windows::Foundation::IAsyncOperation<ContentDialogResult> AddDownloadDialog::ShowAsync()
    {
        ContentDialogResult result;
        //Load Validate Page
        IsPrimaryButtonEnabled(false);
        ViewStack().CurrentPage(L"Validate");
        std::string clipboardText{ winrt::to_string(co_await Clipboard::GetContent().GetTextAsync()) };
        if(StringHelpers::isValidUrl(clipboardText))
        {
            TxtUrl().Text(winrt::to_hstring(clipboardText));
            IsPrimaryButtonEnabled(true);
        }
        CmbAuthenticate().Items().Append(winrt::box_value(winrt::to_hstring(_("None"))));
        CmbAuthenticate().Items().Append(winrt::box_value(winrt::to_hstring(_("Use manual credential"))));
        for(const std::string& name : m_controller->getKeyringCredentialNames())
        {
            CmbAuthenticate().Items().Append(winrt::box_value(winrt::to_hstring(name)));
        }
        CmbAuthenticate().SelectedIndex(0);
        result = co_await base_type::ShowAsync();
        //Validate Url
        if(result == ContentDialogResult::Primary)
        {
            std::optional<Credential> credential{ std::nullopt };
            if(CmbAuthenticate().SelectedIndex() == 1)
            {
                credential = Credential{ "", "", winrt::to_string(TxtUsername().Text()), winrt::to_string(TxtPassword().Password()) };
            }
            if(CmbAuthenticate().SelectedIndex() < 2)
            {
                m_controller->validateUrl(winrt::to_string(TxtUrl().Text()), credential);
            }
            else
            {
                m_controller->validateUrl(winrt::to_string(TxtUrl().Text()), static_cast<size_t>(CmbAuthenticate().SelectedIndex() - 2));
            }
            PrimaryButtonText(L"");
            CloseButtonText(L"");
            ViewStack().CurrentPage(L"Spinner");
            result = co_await base_type::ShowAsync();
            //Download
            if(result == ContentDialogResult::Primary)
            {

            }
        }
        co_return result;
    }

    void AddDownloadDialog::OnClosing(const ContentDialog& sender, const ContentDialogClosingEventArgs& args)
    {
        args.Cancel(ViewStack().CurrentPage() == L"Spinner");
    }

    void AddDownloadDialog::OnTxtUrlChanged(const IInspectable& sender, const TextChangedEventArgs& args)
    {
        IsPrimaryButtonEnabled(StringHelpers::isValidUrl(winrt::to_string(TxtUrl().Text())));
    }

    void AddDownloadDialog::OnCmbAuthenticateChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        TxtUsername().Visibility(CmbAuthenticate().SelectedIndex() == 1 ? Visibility::Visible : Visibility::Collapsed);
        TxtPassword().Visibility(CmbAuthenticate().SelectedIndex() == 1 ? Visibility::Visible : Visibility::Collapsed);
    }

    void AddDownloadDialog::OnUrlValidated(const ParamEventArgs<bool>& args)
    {
        if(!args.getParam())
        {
            Title(winrt::box_value(winrt::to_hstring(_("Error"))));
            Content(winrt::box_value(winrt::to_hstring(_("The url provided is invalid or unable to be reached. Check both the url and authentication used."))));
            CloseButtonText(winrt::to_hstring(_("Close")));
            DefaultButton(ContentDialogButton::Close);
            return;
        }
        PrimaryButtonText(winrt::to_hstring(_("Download")));
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        DefaultButton(ContentDialogButton::Primary);
        //Load Download Page
        IsPrimaryButtonEnabled(false);
        if(!m_controller->isUrlPlaylist())
        {
            ViewStack().CurrentPage(L"DownloadSingle");
            for(const std::string& fileType : m_controller->getFileTypeStrings())
            {
                CmbFileTypeSingle().Items().Append(winrt::box_value(winrt::to_hstring(fileType)));
            }
            CmbFileTypeSingle().SelectedIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            for(const std::string& quality : m_controller->getQualityStrings(static_cast<size_t>(CmbFileTypeSingle().SelectedIndex())))
            {
                CmbQualitySingle().Items().Append(winrt::box_value(winrt::to_hstring(quality)));
            }
            CmbQualitySingle().SelectedIndex(0);
            for(const std::string& audioLanguage : m_controller->getAudioLanguageStrings())
            {
                CmbAudioLanguageSingle().Items().Append(winrt::box_value(winrt::to_hstring(audioLanguage)));
            }
            CmbAudioLanguageSingle().SelectedIndex(0);
            TglDownloadSubtitlesSingle().IsOn(m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            TglPreferAV1Single().IsOn(m_controller->getPreviousDownloadOptions().getPreferAV1());
            TglSplitChaptersSingle().IsOn(m_controller->getPreviousDownloadOptions().getSplitChapters());
            TxtSaveFolderSingle().Text(winrt::to_hstring(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            TxtFilenameSingle().Text(winrt::to_hstring(m_controller->getMediaTitle(0)));
            TxtTimeFrameStartSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtTimeFrameStartSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtTimeFrameEndSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
            TxtTimeFrameEndSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
            TglLimitSpeedSingle().IsOn(m_controller->getPreviousDownloadOptions().getLimitSpeed());
        }
        else
        {
            //TODO
        }
    }

    void AddDownloadDialog::RevertFilenameSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        TxtFilenameSingle().Text(winrt::to_hstring(m_controller->getMediaTitle(0)));
    }
}
