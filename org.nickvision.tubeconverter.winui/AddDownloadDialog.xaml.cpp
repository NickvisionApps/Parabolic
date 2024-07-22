#include "AddDownloadDialog.xaml.h"
#if __has_include("AddDownloadDialog.g.cpp")
#include "AddDownloadDialog.g.cpp"
#endif
#include <format>
#include <libnick/helpers/codehelpers.h>
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
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;

namespace winrt::Nickvision::TubeConverter::WinUI::implementation 
{
    static void SetComboBoxModel(const ComboBox& comboBox, const std::vector<std::string>& strs)
    {
        comboBox.Items().Clear();
        for(const std::string& str : strs)
        {
            comboBox.Items().Append(winrt::box_value(winrt::to_hstring(str)));
        }
        comboBox.SelectedIndex(0);
    }

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
        TxtFilenameSingle().Header(winrt::box_value(winrt::to_hstring(_("File Name"))));
        TxtFilenameSingle().PlaceholderText(winrt::to_hstring(_("Enter file name here")));
        ToolTipService::SetToolTip(BtnRevertFilenameSingle(), winrt::box_value(winrt::to_hstring(_("Revert to Title"))));
        TxtTimeFrameStartSingle().Header(winrt::box_value(winrt::to_hstring(_("Start Time"))));
        TxtTimeFrameEndSingle().Header(winrt::box_value(winrt::to_hstring(_("End Time"))));
        TglLimitSpeedSingle().Header(winrt::box_value(winrt::to_hstring(_("Limit Download Speed"))));
        CmbFileTypePlaylist().Header(winrt::box_value(winrt::to_hstring(_("File Type"))));
        TxtSaveFolderPlaylist().Header(winrt::box_value(winrt::to_hstring(_("Save Folder"))));
        ToolTipService::SetToolTip(BtnSelectSaveFolderPlaylist(), winrt::box_value(winrt::to_hstring(_("Select Save Folder"))));
        TglDownloadSubtitlesPlaylist().Header(winrt::box_value(winrt::to_hstring(_("Download Subtitles"))));
        TglPreferAV1Playlist().Header(winrt::box_value(winrt::to_hstring(_("Prefer AV1 Codec"))));
        TglSplitChaptersPlaylist().Header(winrt::box_value(winrt::to_hstring(_("Split Video by Chapters"))));
        TglLimitSpeedPlaylist().Header(winrt::box_value(winrt::to_hstring(_("Limit Download Speed"))));
        TglNumberTitlesPlaylist().Header(winrt::box_value(winrt::to_hstring(_("Number Titles"))));
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
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        credentialNames.insert(credentialNames.begin(), _("None"));
        SetComboBoxModel(CmbAuthenticate(), credentialNames);
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
        if(!m_controller->isUrlPlaylist())
        {
            ViewStack().CurrentPage(L"DownloadSingle");
            SetComboBoxModel(CmbFileTypeSingle(), m_controller->getFileTypeStrings());
            CmbFileTypeSingle().SelectedIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            SetComboBoxModel(CmbQualitySingle(), m_controller->getQualityStrings(static_cast<size_t>(CmbFileTypeSingle().SelectedIndex())));
            SetComboBoxModel(CmbAudioLanguageSingle(), m_controller->getAudioLanguageStrings());
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
            ViewStack().CurrentPage(L"DownloadPlaylist");
            LblItemsPlaylist().Text(winrt::to_hstring(std::vformat(_("Playlist Items ({})"), std::make_format_args(CodeHelpers::unmove(m_controller->getMediaCount())))));
            SetComboBoxModel(CmbFileTypePlaylist(), m_controller->getFileTypeStrings());
            CmbFileTypePlaylist().SelectedIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            TxtSaveFolderPlaylist().Text(winrt::to_hstring(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            TglDownloadSubtitlesPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            TglPreferAV1Playlist().IsOn(m_controller->getPreviousDownloadOptions().getPreferAV1());
            TglSplitChaptersPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getSplitChapters());
            TglLimitSpeedPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getLimitSpeed());
            for(int i = 0; i < m_controller->getMediaCount(); i++)
            {
                winrt::hstring title{ winrt::to_hstring(m_controller->getMediaTitle(i)) };
                TextBox txt;
                txt.Text(title);
                txt.PlaceholderText(title);
                CheckBox chk;
                chk.HorizontalAlignment(HorizontalAlignment::Stretch);
                chk.HorizontalContentAlignment(HorizontalAlignment::Stretch);
                chk.IsChecked(true);
                chk.Content(txt);
                ListItemsPlaylist().Children().Append(chk);
            }
            TglNumberTitlesPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getNumberTitles());
        }
    }

    void AddDownloadDialog::OnCmbFileTypeSingleChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        SetComboBoxModel(CmbQualitySingle(), m_controller->getQualityStrings(static_cast<size_t>(CmbFileTypeSingle().SelectedIndex())));
    }

    Windows::Foundation::IAsyncAction AddDownloadDialog::SelectSaveFolderSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FolderPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L"*");
        StorageFolder folder{ co_await picker.PickSingleFolderAsync() };
        if(folder)
        {
            TxtSaveFolderSingle().Text(folder.Path());
        }   
    }

    void AddDownloadDialog::RevertFilenameSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        TxtFilenameSingle().Text(winrt::to_hstring(m_controller->getMediaTitle(0)));
    }

    Windows::Foundation::IAsyncAction AddDownloadDialog::SelectSaveFolderPlaylist(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FolderPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L"*");
        StorageFolder folder{ co_await picker.PickSingleFolderAsync() };
        if(folder)
        {
            TxtSaveFolderSingle().Text(folder.Path());
        }
    }

    void AddDownloadDialog::OnTglNumberTitlesPlaylistToggled(const IInspectable& sender, const RoutedEventArgs& args)
    {
        int i{ 0 };
        for(const IInspectable& child : ListItemsPlaylist().Children())
        {
            TextBox txt{ child.as<CheckBox>().Content().as<TextBox>() };
            txt.Text(winrt::to_hstring(TglNumberTitlesPlaylist().IsOn() ? std::format("{} - {}", i + 1, m_controller->getMediaTitle(i)) : m_controller->getMediaTitle(i)));
            i++;
        }
    }
}
