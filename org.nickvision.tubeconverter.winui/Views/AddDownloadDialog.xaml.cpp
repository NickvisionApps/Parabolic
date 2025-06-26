#include "Views/AddDownloadDialog.xaml.h"
#if __has_include("Views/AddDownloadDialog.g.cpp")
#include "Views/AddDownloadDialog.g.cpp"
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
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;

enum AddDownloadDialogPage
{
    Validate = 0,
    Loading,
    Single,
    Playlist
};

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    AddDownloadDialog::AddDownloadDialog()
    {
        InitializeComponent();
    }

    Windows::Foundation::IAsyncAction AddDownloadDialog::Controller(const std::shared_ptr<AddDownloadDialogController>& controller, const winrt::hstring& url)
    {
        m_controller = controller;
        //Register Events
        m_controller->urlValidated() += [&](const ParamEventArgs<bool>& args){ DispatcherQueue().TryEnqueue([this, args](){ OnUrlValidated(*args); }); };
        //Localize Strings
        Title(winrt::box_value(winrt::to_hstring(_("Add Download"))));
        CloseButtonText(winrt::to_hstring(_("Cancel")));
        NavViewValidateMedia().Text(winrt::to_hstring(_("Media")));
        NavViewValidateAuthentication().Text(winrt::to_hstring(_("Authentication")));
        RowMediaUrl().Title(winrt::to_hstring(_("Media URL")));
        TxtMediaUrl().PlaceholderText(winrt::to_hstring(_("Enter media url here")));
        ToolTipService::SetToolTip(BtnUseBatchFile(), winrt::box_value(winrt::to_hstring(_("Use Batch File"))));
        RowDownloadImmediately().Title(winrt::to_hstring(_("Download Immediately")));
        TglDownloadImmediately().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglDownloadImmediately().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowCredential().Title(winrt::to_hstring(_("Credential")));
        RowUsername().Title(winrt::to_hstring(_("Username")));
        TxtUsername().PlaceholderText(winrt::to_hstring(_("Enter username here")));
        RowPassword().Title(winrt::to_hstring(_("Password")));
        TxtPassword().PlaceholderText(winrt::to_hstring(_("Enter password here")));
        LblLoading().Text(winrt::to_hstring(_("This may take some time...")));
        NavViewSingleGeneral().Text(winrt::to_hstring(_("General")));
        NavViewSingleSubtitles().Text(winrt::to_hstring(_("Subtitles")));
        NavViewSingleAdvanced().Text(winrt::to_hstring(_("Advanced")));
        RowFileTypeSingle().Title(winrt::to_hstring(_("File Type")));
        RowVideoFormatSingle().Title(winrt::to_hstring(_("Video Format")));
        RowAudioFormatSingle().Title(winrt::to_hstring(_("Audio Format")));
        RowSaveFolderSingle().Title(winrt::to_hstring(_("Save Folder")));
        ToolTipService::SetToolTip(BtnSelectSaveFolderSingle(), winrt::box_value(winrt::to_hstring(_("Select Save Folder"))));
        RowFileNameSingle().Title(winrt::to_hstring(_("File Name")));
        ToolTipService::SetToolTip(BtnRevertFileNameSingle(), winrt::box_value(winrt::to_hstring(_("Revert File Name"))));
        StatusNoSubtitlesSingle().Title(winrt::to_hstring(_("No Subtitles Available")));
        LblSelectAllSubtitlesSingle().Text(winrt::to_hstring(_("Select All")));
        LblDeselectAllSubtitlesSingle().Text(winrt::to_hstring(_("Deselect All")));
        RowSplitVideoByChaptersSingle().Title(winrt::to_hstring(_("Split Video by Chapters")));
        RowExportDescriptionSingle().Title(winrt::to_hstring(_("Export Description")));
        RowExcludeFromHistorySingle().Title(winrt::to_hstring(_("Exclude from History")));
        RowPostProcessorArgumentSingle().Title(winrt::to_hstring(_("Post Processor Argument")));
        RowStartTimeSingle().Title(winrt::to_hstring(_("Start Time")));
        RowEndTimeSingle().Title(winrt::to_hstring(_("End Time")));
        NavViewPlaylistGeneral().Text(winrt::to_hstring(_("General")));
        NavViewPlaylistItems().Text(winrt::to_hstring(_("Items")));
        //Load
        ViewStack().CurrentPageIndex(AddDownloadDialogPage::Validate);
        PrimaryButtonText(winrt::to_hstring(_("Validate")));
        IsPrimaryButtonEnabled(false);
        DefaultButton(ContentDialogButton::Primary);
        if(StringHelpers::isValidUrl(winrt::to_string(url)))
        {
            TxtMediaUrl().Text(url);
            IsPrimaryButtonEnabled(true);
        }
        else
        {
            DataPackageView package{ Clipboard::GetContent() };
            if(package.Contains(StandardDataFormats::Text()))
            {
                winrt::hstring txt{ co_await package.GetTextAsync() };
                if(StringHelpers::isValidUrl(winrt::to_string(txt)))
                {
                    TxtMediaUrl().Text(txt);
                    IsPrimaryButtonEnabled(true);
                }
            }
        }
        CmbCredential().Items().Append(winrt::box_value(winrt::to_hstring(_("None"))));
        CmbCredential().Items().Append(winrt::box_value(winrt::to_hstring(_("Use manual credentials"))));
        for(const std::string& credential : m_controller->getKeyringCredentialNames())
        {
            CmbCredential().Items().Append(winrt::box_value(winrt::to_hstring(credential)));
        }
        CmbCredential().SelectedIndex(0);
    }

    void AddDownloadDialog::Hwnd(HWND hwnd)
    {
        m_hwnd = hwnd;
    }

    Windows::Foundation::IAsyncOperation<ContentDialogResult> AddDownloadDialog::ShowAsync()
    {
        ContentDialogResult res{ co_await base_type::ShowAsync() };
        if(res == ContentDialogResult::Primary) //Validate
        {
            CloseButtonText(L"");
            PrimaryButtonText(L"");
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Loading);
            std::optional<Credential> credential{ std::nullopt };
            if(CmbCredential().SelectedIndex() == 1)
            {
                credential = Credential{ "", "", winrt::to_string(TxtUsername().Text()), winrt::to_string(TxtPassword().Text()) };
            }
            if(CmbCredential().SelectedIndex() < 2)
            {
                m_controller->validateUrl(winrt::to_string(TxtMediaUrl().Text()), credential);
            }
            else
            {
                m_controller->validateUrl(winrt::to_string(TxtMediaUrl().Text()), CmbCredential().SelectedIndex() - 2);
            }
            res = co_await base_type::ShowAsync();
        }
        else if(res == ContentDialogResult::Secondary) //Download
        {

        }
        co_return res;
    }

    void AddDownloadDialog::OnNavViewValidateSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStackValidate().CurrentPageIndex(static_cast<int>(index));
        }
    }

    void AddDownloadDialog::OnTxtMediaUrlTextChanged(const IInspectable& sender, const TextChangedEventArgs& args)
    {
        IsPrimaryButtonEnabled(StringHelpers::isValidUrl(winrt::to_string(TxtMediaUrl().Text())));
    }

    void AddDownloadDialog::OnCmbCredentialSelectionChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        Microsoft::UI::Xaml::Visibility visible{ CmbCredential().SelectedIndex() == 1 ? Visibility::Visible : Visibility::Collapsed };
        RowUsername().Visibility(visible);
        TxtUsername().Text(L"");
        RowPassword().Visibility(visible);
        TxtPassword().Text(L"");
    }

    Windows::Foundation::IAsyncAction AddDownloadDialog::UseBatchFile(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FileOpenPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L".txt");
        StorageFile file{ co_await picker.PickSingleFileAsync() };
        if(file)
        {
            CloseButtonText(L"");
            PrimaryButtonText(L"");
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Loading);
            std::optional<Credential> credential{ std::nullopt };
            if(CmbCredential().SelectedIndex() == 1)
            {
                credential = Credential{ "", "", winrt::to_string(TxtUsername().Text()), winrt::to_string(TxtPassword().Text()) };
            }
            if(CmbCredential().SelectedIndex() < 2)
            {
                m_controller->validateBatchFile(winrt::to_string(file.Path()), credential);
            }
            else
            {
                m_controller->validateBatchFile(winrt::to_string(file.Path()), CmbCredential().SelectedIndex() - 2);
            }
        }
    }

    void AddDownloadDialog::OnNavViewSingleSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStackSingle().CurrentPageIndex(static_cast<int>(index));
        }
    }

    winrt::fire_and_forget AddDownloadDialog::OnUrlValidated(bool valid)
    {
        if(!valid)
        {
            Hide();
            ContentDialog dialog;
            dialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
            dialog.Content(winrt::box_value(winrt::to_hstring(_("The url provided is invalid or unable to be reached. Check the url, the authentication used, the cookies settings, and the preferred codecs selected. Note that the service may have blocked your IP or the video may be geo-restricted."))));
            dialog.CloseButtonText(winrt::to_hstring(_("OK")));
            dialog.DefaultButton(ContentDialogButton::Close);
            dialog.RequestedTheme(RequestedTheme());
            dialog.XamlRoot(XamlRoot());
            co_await dialog.ShowAsync();
        }
        CloseButtonText(L"Cancel");
        SecondaryButtonText(L"Download");
        DefaultButton(ContentDialogButton::Secondary);
        if(!m_controller->isUrlPlaylist()) //Single Download
        {
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Single);
            //Load Options
            size_t previous{ static_cast<size_t>(m_controller->getPreviousDownloadOptions().getFileType()) };
            for(const std::string& fileType : m_controller->getFileTypeStrings())
            {
                CmbFileTypeSingle().Items().Append(winrt::box_value(winrt::to_hstring(fileType)));
            }
            CmbFileTypeSingle().SelectedIndex(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount() ? static_cast<int>(previous) - MediaFileType::getVideoFileTypeCount() : static_cast<int>(previous));
            for(const std::string& videoFormat : m_controller->getVideoFormatStrings(&previous))
            {
                CmbVideoFormatSingle().Items().Append(winrt::box_value(winrt::to_hstring(videoFormat)));
            }
            CmbVideoFormatSingle().SelectedIndex(static_cast<int>(previous));
            for(const std::string& audioFormat : m_controller->getAudioFormatStrings(&previous))
            {
                CmbAudioFormatSingle().Items().Append(winrt::box_value(winrt::to_hstring(audioFormat)));
            }
            CmbAudioFormatSingle().SelectedIndex(static_cast<int>(previous));
            TxtSaveFolderSingle().Text(winrt::to_hstring(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            TxtFileNameSingle().Text(winrt::to_hstring(m_controller->getMediaTitle(0)));
            //Load Subtitles
            std::vector<SubtitleLanguage> previousSubtitles{ m_controller->getPreviousDownloadOptions().getSubtitleLanguages() };
            ViewStackSubtitlesSingle().CurrentPageIndex(0);
            for(const std::string& subtitle : m_controller->getSubtitleLanguageStrings())
            {
                ListSubtitlesSingle().Items().Append(winrt::box_value(winrt::to_hstring(subtitle)));
                for(const SubtitleLanguage& language : previousSubtitles)
                {
                    if(subtitle == language.str())
                    {
                        ListSubtitlesSingle().SelectedItems().Append(ListSubtitlesSingle().Items().GetAt(ListSubtitlesSingle().Items().Size() - 1));
                        break;
                    }
                }
                ViewStackSubtitlesSingle().CurrentPageIndex(1);
            }
            //Load Advanced Options
            TglSplitVideoByChaptersSingle().IsOn(m_controller->getPreviousDownloadOptions().getSplitChapters());
            TglExportDescriptionSingle().IsOn(m_controller->getPreviousDownloadOptions().getExportDescription());
            for(const std::string& name : m_controller->getPostprocessingArgumentNames())
            {
                CmbPostProcessorArgumentSingle().Items().Append(winrt::box_value(winrt::to_hstring(name)));
                if(name == m_controller->getPreviousDownloadOptions().getPostProcessorArgument())
                {
                    CmbPostProcessorArgumentSingle().SelectedIndex(CmbPostProcessorArgumentSingle().Items().Size() - 1);
                }
            }
            TxtStartTimeSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtStartTimeSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtEndTimeSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
            TxtEndTimeSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
        }
        else //Playlist Download
        {
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Playlist);
        }
    }
}
