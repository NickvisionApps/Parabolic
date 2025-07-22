#include "Views/AddDownloadDialog.xaml.h"
#if __has_include("Views/AddDownloadDialog.g.cpp")
#include "Views/AddDownloadDialog.g.cpp"
#endif
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "helpers/WinUIHelpers.h"

using namespace ::Nickvision::Events;
using namespace ::Nickvision::Helpers;
using namespace ::Nickvision::Keyring;
using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
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
        LblFileTypeWarningSingle().Text(winrt::to_hstring(_("Generic file types do not fully support embedding thumbnails and subtitles. Please select a specific file type that is known to support embedding to prevent separate files from being written.")));
        RowVideoFormatSingle().Title(winrt::to_hstring(_("Video Format")));
        RowAudioFormatSingle().Title(winrt::to_hstring(_("Audio Format")));
        RowSaveFolderSingle().Title(winrt::to_hstring(_("Save Folder")));
        ToolTipService::SetToolTip(BtnSelectSaveFolderSingle(), winrt::box_value(winrt::to_hstring(_("Select Save Folder"))));
        RowFileNameSingle().Title(winrt::to_hstring(_("File Name")));
        ToolTipService::SetToolTip(BtnRevertFileNameSingle(), winrt::box_value(winrt::to_hstring(_("Revert to Title"))));
        StatusNoSubtitlesSingle().Title(winrt::to_hstring(_("No Subtitles Available")));
        LblSelectAllSubtitlesSingle().Text(winrt::to_hstring(_("Select All")));
        LblDeselectAllSubtitlesSingle().Text(winrt::to_hstring(_("Deselect All")));
        RowSplitVideoByChaptersSingle().Title(winrt::to_hstring(_("Split Video by Chapters")));
        TglSplitVideoByChaptersSingle().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglSplitVideoByChaptersSingle().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowExportDescriptionSingle().Title(winrt::to_hstring(_("Export Description")));
        TglExportDescriptionSingle().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglExportDescriptionSingle().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowExcludeFromHistorySingle().Title(winrt::to_hstring(_("Exclude from History")));
        TglExcludeFromHistorySingle().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglExcludeFromHistorySingle().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowPostProcessorArgumentSingle().Title(winrt::to_hstring(_("Post Processor Argument")));
        RowStartTimeSingle().Title(winrt::to_hstring(_("Start Time")));
        RowEndTimeSingle().Title(winrt::to_hstring(_("End Time")));
        NavViewPlaylistGeneral().Text(winrt::to_hstring(_("General")));
        NavViewPlaylistItems().Text(winrt::to_hstring(_("Items")));
        NavViewPlaylistAdvanced().Text(winrt::to_hstring(_("Advanced")));
        RowFileTypePlaylist().Title(winrt::to_hstring(_("File Type")));
        LblFileTypeWarningPlaylist().Text(winrt::to_hstring(_("Generic file types do not fully support embedding thumbnails and writing playlist files. Please select a specific file type that is known to support embedding to prevent separate files from being written.")));
        RowWriteM3UFilePlaylist().Title(winrt::to_hstring(_("Write M3U Playlist File")));
        TglWriteM3UFilePlaylist().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglWriteM3UFilePlaylist().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowSaveFolderPlaylist().Title(winrt::to_hstring(_("Save Folder")));
        LblSaveFolderWarningPlaylist().Text(winrt::to_hstring(_("Will be ignored for media in batch files that provide save folder paths.")));
        ToolTipService::SetToolTip(BtnSelectSaveFolderPlaylist(), winrt::box_value(winrt::to_hstring(_("Select Save Folder"))));
        RowNumberTitlesPlaylist().Title(winrt::to_hstring(_("Number Titles")));
        TglNumberTitlesPlaylist().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglNumberTitlesPlaylist().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        LblSelectAllItemsPlaylist().Text(winrt::to_hstring(_("Select All")));
        LblDeselectAllItemsPlaylist().Text(winrt::to_hstring(_("Deselect All")));
        RowSplitVideoByChaptersPlaylist().Title(winrt::to_hstring(_("Split Video by Chapters")));
        TglSplitVideoByChaptersPlaylist().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglSplitVideoByChaptersPlaylist().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowExportDescriptionPlaylist().Title(winrt::to_hstring(_("Export Description")));
        TglExportDescriptionPlaylist().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglExportDescriptionPlaylist().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowExcludeFromHistoryPlaylist().Title(winrt::to_hstring(_("Exclude from History")));
        TglExcludeFromHistoryPlaylist().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglExcludeFromHistoryPlaylist().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowPostProcessorArgumentPlaylist().Title(winrt::to_hstring(_("Post Processor Argument")));
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
            res = co_await ShowAsync();
        }
        else if(res == ContentDialogResult::Secondary) //Download
        {
            Download();
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

    void AddDownloadDialog::OnCmbFileTypeChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        int fileTypeIndex{ sender.as<ComboBox>().SelectedIndex() };
        if(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount())
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex) };
        BtnFileTypeWarningSingle().Visibility(type.isGeneric() ? Visibility::Visible : Visibility::Collapsed);
        BtnFileTypeWarningPlaylist().Visibility(type.isGeneric() ? Visibility::Visible : Visibility::Collapsed);
        if(sender.as<ComboBox>() == CmbFileTypeSingle())
        {
            size_t previous{ 0 };
            CmbVideoFormatSingle().Items().Clear();
            for(const std::string& videoFormat : m_controller->getVideoFormatStrings(type, previous))
            {
                CmbVideoFormatSingle().Items().Append(winrt::box_value(winrt::to_hstring(videoFormat)));
            }
            CmbVideoFormatSingle().SelectedIndex(static_cast<int>(previous));
            CmbAudioFormatSingle().Items().Clear();
            for(const std::string& audioFormat : m_controller->getAudioFormatStrings(type, previous))
            {
                CmbAudioFormatSingle().Items().Append(winrt::box_value(winrt::to_hstring(audioFormat)));
            }
            CmbAudioFormatSingle().SelectedIndex(static_cast<int>(previous));
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

    void AddDownloadDialog::RevertFileNameSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        TxtFileNameSingle().Text(winrt::to_hstring(m_controller->getMediaTitle(0)));
    }

    void AddDownloadDialog::SelectAllSubtitlesSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ListSubtitlesSingle().SelectAll();
    }

    void AddDownloadDialog::DeselectAllSubtitlesSingle(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ListSubtitlesSingle().SelectedItems().Clear();
    }

    void AddDownloadDialog::OnNavViewPlaylistSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStackPlaylist().CurrentPageIndex(static_cast<int>(index));
        }
    }

    Windows::Foundation::IAsyncAction AddDownloadDialog::SelectSaveFolderPlaylist(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FolderPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L"*");
        StorageFolder folder{ co_await picker.PickSingleFolderAsync() };
        if(folder)
        {
            TxtSaveFolderPlaylist().Text(folder.Path());
        }
    }

    void AddDownloadDialog::OnTglNumberTitlesPlaylistToggled(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_controller->setPreviousNumberTitles(TglNumberTitlesPlaylist().IsOn());
        for(unsigned int i = 0; i < ListItemsPlaylist().Items().Size(); i++)
        {
            StackPanel panel{ ListItemsPlaylist().Items().GetAt(i).as<StackPanel>() };
            TextBox txt{ panel.Children().GetAt(0).as<TextBox>() };
            txt.Text(winrt::to_hstring(m_controller->getMediaTitle(i, TglNumberTitlesPlaylist().IsOn())));
        }
    }

    void AddDownloadDialog::SelectAllItemsPlaylist(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ListItemsPlaylist().SelectAll();
    }

    void AddDownloadDialog::DeselectAllItemsPlaylist(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ListItemsPlaylist().SelectedItems().Clear();
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
            co_return;
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
            CmbVideoFormatSingle().Items().Clear();
            for(const std::string& videoFormat : m_controller->getVideoFormatStrings(m_controller->getPreviousDownloadOptions().getFileType(), previous))
            {
                CmbVideoFormatSingle().Items().Append(winrt::box_value(winrt::to_hstring(videoFormat)));
            }
            CmbVideoFormatSingle().SelectedIndex(static_cast<int>(previous));
            CmbAudioFormatSingle().Items().Clear();
            for(const std::string& audioFormat : m_controller->getAudioFormatStrings(m_controller->getPreviousDownloadOptions().getFileType(), previous))
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
            if(CmbPostProcessorArgumentSingle().SelectedIndex() == -1)
            {
                CmbPostProcessorArgumentSingle().SelectedIndex(0);
            }
            TxtStartTimeSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtStartTimeSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).startStr()));
            TxtEndTimeSingle().PlaceholderText(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
            TxtEndTimeSingle().Text(winrt::to_hstring(m_controller->getMediaTimeFrame(0).endStr()));
        }
        else //Playlist Download
        {
            ViewStack().CurrentPageIndex(AddDownloadDialogPage::Playlist);
            //Load Options
            for(const std::string& fileType : m_controller->getFileTypeStrings())
            {
                CmbFileTypePlaylist().Items().Append(winrt::box_value(winrt::to_hstring(fileType)));
            }
            CmbFileTypePlaylist().SelectedIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            TglWriteM3UFilePlaylist().IsOn(m_controller->getPreviousDownloadOptions().getWritePlaylistFile());
            TxtSaveFolderPlaylist().Text(winrt::to_hstring(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            //Load Items
            TglNumberTitlesPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getNumberTitles());
            for(size_t i = 0; i < m_controller->getMediaCount(); i++)
            {
                winrt::hstring title{ winrt::to_hstring(m_controller->getMediaTitle(i, TglNumberTitlesPlaylist().IsOn())) };
                StackPanel panel;
                panel.Tag(winrt::box_value(title));
                panel.Orientation(Orientation::Horizontal);
                panel.Spacing(6);
                TextBox txt;
                txt.MaxWidth(320);
                txt.Text(title);
                txt.TextChanged([this, panel, txt](const IInspectable&, const TextChangedEventArgs&)
                {
                    panel.Tag(winrt::box_value(txt.Text()));
                });
                FontIcon icn;
                icn.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
                icn.Glyph(L"\uE7A7");
                Button btn;
                btn.Content(icn);
                ToolTipService::SetToolTip(btn, winrt::box_value(winrt::to_hstring(_("Revert to Title"))));
                btn.Click([this, i, txt](const IInspectable&, const RoutedEventArgs&)
                {
                    txt.Text(winrt::to_hstring(m_controller->getMediaTitle(i, TglNumberTitlesPlaylist().IsOn())));
                });
                panel.Children().Append(txt);
                panel.Children().Append(btn);
                ListItemsPlaylist().Items().Append(panel);
                ListItemsPlaylist().SelectedItems().Append(ListItemsPlaylist().Items().GetAt(static_cast<int>(i)));
            }
            //Load Advanced Options
            TglSplitVideoByChaptersPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getSplitChapters());
            TglExportDescriptionPlaylist().IsOn(m_controller->getPreviousDownloadOptions().getExportDescription());
            for(const std::string& name : m_controller->getPostprocessingArgumentNames())
            {
                CmbPostProcessorArgumentPlaylist().Items().Append(winrt::box_value(winrt::to_hstring(name)));
                if(name == m_controller->getPreviousDownloadOptions().getPostProcessorArgument())
                {
                    CmbPostProcessorArgumentPlaylist().SelectedIndex(CmbPostProcessorArgumentPlaylist().Items().Size() - 1);
                }
            }
            if(CmbPostProcessorArgumentPlaylist().SelectedIndex() == -1)
            {
                CmbPostProcessorArgumentPlaylist().SelectedIndex(0);
            }
        }
        if(TglDownloadImmediately().IsOn())
        {
            Hide();
            Download();
        }
    }

    void AddDownloadDialog::Download()
    {
        if(!m_controller->isUrlPlaylist()) //Single Download
        {
            std::vector<std::string> subtitles;
            for(const IInspectable& item : ListSubtitlesSingle().SelectedItems())
            {
                subtitles.push_back(winrt::to_string(winrt::unbox_value<winrt::hstring>(item)));
            }
            m_controller->addSingleDownload(winrt::to_string(TxtSaveFolderSingle().Text()), winrt::to_string(TxtFileNameSingle().Text()), CmbFileTypeSingle().SelectedIndex(), CmbVideoFormatSingle().SelectedIndex(), CmbAudioFormatSingle().SelectedIndex(), subtitles, TglSplitVideoByChaptersSingle().IsOn(), TglExportDescriptionSingle().IsOn(), TglExcludeFromHistorySingle().IsOn(), CmbPostProcessorArgumentSingle().SelectedIndex(), winrt::to_string(TxtStartTimeSingle().Text()), winrt::to_string(TxtEndTimeSingle().Text()));
        }
        else //Playlist Download
        {
            std::map<size_t, std::string> filenames;
            for(const IInspectable& item : ListItemsPlaylist().SelectedItems())
            {
                unsigned int index;
                if(ListItemsPlaylist().Items().IndexOf(item, index))
                {
                    winrt::hstring filename{ winrt::unbox_value<winrt::hstring>(item.as<FrameworkElement>().Tag()) };
                    filenames.emplace(static_cast<size_t>(index), winrt::to_string(filename));
                }
            }
            m_controller->addPlaylistDownload(winrt::to_string(TxtSaveFolderPlaylist().Text()), filenames, CmbFileTypePlaylist().SelectedIndex(), TglSplitVideoByChaptersPlaylist().IsOn(), TglExportDescriptionPlaylist().IsOn(), TglWriteM3UFilePlaylist().IsOn(), TglExcludeFromHistoryPlaylist().IsOn(), CmbPostProcessorArgumentPlaylist().SelectedIndex());
        }
    }
}
