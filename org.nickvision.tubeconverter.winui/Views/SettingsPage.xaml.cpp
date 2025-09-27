#include "Views/SettingsPage.xaml.h"
#if __has_include("Views/SettingsPage.g.cpp")
#include "Views/SettingsPage.g.cpp"
#endif
#include <libnick/localization/gettext.h>
#include "Controls/SettingsRow.xaml.h"
#include "Helpers/WinUIHelpers.h"

using namespace ::Nickvision::TubeConverter::Shared::Controllers;
using namespace ::Nickvision::TubeConverter::Shared::Models;
using namespace ::Nickvision::TubeConverter::WinUI::Helpers;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Microsoft::UI::Xaml::Controls;
using namespace winrt::Microsoft::UI::Xaml::Media;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Pickers;

namespace winrt::Nickvision::TubeConverter::WinUI::Views::implementation
{
    SettingsPage::SettingsPage()
        : m_constructing{ true }
    {
        InitializeComponent();
        //Localize Strings
        LblSettings().Text(winrt::to_hstring(_("Settings")));
        NavUserInterface().Text(winrt::to_hstring(_("User Interface")));
        NavDownloads().Text(winrt::to_hstring(_("Downloads")));
        NavDownloader().Text(winrt::to_hstring(_("Downloader")));
        NavConverter().Text(winrt::to_hstring(_("Converter")));
        RowTheme().Title(winrt::to_hstring(_("Theme")));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_("Light"))));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_("Dark"))));
        CmbTheme().Items().Append(winrt::box_value(winrt::to_hstring(_("System"))));
        RowLanguage().Title(winrt::to_hstring(_("Translation Language")));
        RowLanguage().Description(winrt::to_hstring(_("An application restart is required for change to take effect")));
        RowPreventSuspend().Title(winrt::to_hstring(_("Prevent Suspend")));
        RowPreventSuspend().Description(winrt::to_hstring(_("Prevent the computer from sleeping while downloads are running")));
        TglPreventSuspend().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglPreventSuspend().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowHistoryLength().Title(winrt::to_hstring(_("Download History Length")));
        RowHistoryLength().Description(winrt::to_hstring(_("The amount of time to keep past downloads in the app's history")));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("Never"))));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("One Day"))));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("One Week"))));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("One Month"))));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("Three Months"))));
        CmbHistoryLength().Items().Append(winrt::box_value(winrt::to_hstring(_("Forever"))));
        RowMaxActiveDownloads().Title(winrt::to_hstring(_("Max Number of Active Downloads")));
        RowOverwriteExistingFiles().Title(winrt::to_hstring(_("Overwrite Existing Files")));
        TglOverwriteExistingFiles().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglOverwriteExistingFiles().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowIncludeMediaId().Title(winrt::to_hstring(_("Include Media Id in Title")));
        RowIncludeMediaId().Description(winrt::to_hstring(_("Add the media's id to its default title")));
        TglIncludeMediaId().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglIncludeMediaId().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowIncludeAutoSubtitles().Title(winrt::to_hstring(_("Include Auto-Generated Subtitles")));
        RowIncludeAutoSubtitles().Description(winrt::to_hstring(_("Show auto-generated subtitles to download in addition to available subtitles")));
        TglIncludeAutoSubtitles().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglIncludeAutoSubtitles().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowPreferredVideoCodec().Title(winrt::to_hstring(_("Preferred Video Codec")));
        RowPreferredVideoCodec().Description(winrt::to_hstring(_("Prefer this codec when parsing video formats to show available to download")));
        CmbPreferredVideoCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("Any"))));
        CmbPreferredVideoCodec().Items().Append(winrt::box_value(L"VP9"));
        CmbPreferredVideoCodec().Items().Append(winrt::box_value(L"AV1"));
        CmbPreferredVideoCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("H.264 (AVC)"))));
        CmbPreferredVideoCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("H.265 (HEVC)"))));
        RowPreferredAudioCodec().Title(winrt::to_hstring(_("Preferred Audio Codec")));
        RowPreferredAudioCodec().Description(winrt::to_hstring(_("Prefer this codec when parsing audio formats to show available to download")));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("Any"))));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("FLAC (ALAC)"))));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(winrt::to_hstring(_("WAV (AIFF)"))));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(L"OPUS"));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(L"AAC"));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(L"MP4A"));
        CmbPreferredAudioCodec().Items().Append(winrt::box_value(L"MP3"));
        RowPreferredSubtitleFormat().Title(winrt::to_hstring(_("Preferred Subtitle Format")));
        RowPreferredSubtitleFormat().Description(winrt::to_hstring(_("Prefer this subtitle file format when downloading")));
        CmbPreferredSubtitleFormat().Items().Append(winrt::box_value(winrt::to_hstring(_("Any"))));
        CmbPreferredSubtitleFormat().Items().Append(winrt::box_value(L"VTT"));
        CmbPreferredSubtitleFormat().Items().Append(winrt::box_value(L"SRT"));
        CmbPreferredSubtitleFormat().Items().Append(winrt::box_value(L"ASS"));
        CmbPreferredSubtitleFormat().Items().Append(winrt::box_value(L"LRC"));
        RowUsePartFiles().Title(winrt::to_hstring(_("Use Part Files")));
        RowUsePartFiles().Description(winrt::to_hstring(_("Download media in separate .part files instead of directly into the output file")));
        TglUsePartFiles().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglUsePartFiles().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowUseSponsorBlock().Title(winrt::to_hstring(_("Use SponsorBlock for YouTube")));
        RowUseSponsorBlock().Description(winrt::to_hstring(_("Try to remove sponsored segments from videos")));
        TglUseSponsorBlock().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglUseSponsorBlock().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowLimitSpeed().Title(winrt::to_hstring(_("Limit Download Speed")));
        TglLimitSpeed().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglLimitSpeed().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowSpeedLimit().Title(winrt::to_hstring(_("Speed Limit")));
        RowProxyUrl().Title(winrt::to_hstring(_("Proxy URL")));
        TxtProxyUrl().PlaceholderText(winrt::to_hstring(_("Enter proxy url here")));
        RowCookiesBrowser().Title(winrt::to_hstring(_("Cookies from Browser")));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("None"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Brave"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Chrome"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Chromium"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Edge"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Firefox"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Opera"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Vivaldi"))));
        CmbCookiesBrowser().Items().Append(winrt::box_value(winrt::to_hstring(_("Whale"))));
        RowCookiesFile().Title(winrt::to_hstring(_("Cookies File")));
        LblCookiesFile().Text(winrt::to_hstring(_("No file selected")));
        ToolTipService::SetToolTip(BtnSelectCookiesFile(), winrt::box_value(winrt::to_hstring(_("Select Cookies File"))));
        ToolTipService::SetToolTip(BtnClearCookiesFile(), winrt::box_value(winrt::to_hstring(_("Clear Cookies File"))));
        LblAria().Text(winrt::to_hstring(_("aria2c")));
        RowUseAria().Title(winrt::to_hstring(_("Use aria2c")));
        RowUseAria().Description(winrt::to_hstring(_("An alternative downloader that may be faster in some regions compared to yt-dlp's native downloader")));
        TglUseAria().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglUseAria().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowMaxConnectionsPerServer().Title(winrt::to_hstring(_("Max Connections Per Server (-x)")));
        RowMinimumSplitSize().Title(winrt::to_hstring(_("Minimum Split Size (-k)")));
        RowMinimumSplitSize().Description(winrt::to_hstring(_("The minimum size of which to split a file (in MiB)")));
        RowEmbedMetadata().Title(winrt::to_hstring(_("Embed Metadata")));
        TglEmbedMetadata().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglEmbedMetadata().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowRemoveSourceData().Title(winrt::to_hstring(_("Remove Source Data")));
        RowRemoveSourceData().Description(winrt::to_hstring(_("Clear metadata fields containing identifying download information")));
        TglRemoveSourceData().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglRemoveSourceData().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowEmbedThumbnails().Title(winrt::to_hstring(_("Embed Thumbnails")));
        RowEmbedThumbnails().Description(winrt::to_hstring(_("If the file type does not support embedding, the thumbnail will be written to a separate image file")));
        TglEmbedThumbnails().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglEmbedThumbnails().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowCropAudioThumbnails().Title(winrt::to_hstring(_("Crop Audio Thumbnails")));
        RowCropAudioThumbnails().Description(winrt::to_hstring(_("Crop thumbnails of audio files to squares")));
        TglCropAudioThumbnails().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglCropAudioThumbnails().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowEmbedChapters().Title(winrt::to_hstring(_("Embed Chapters")));
        TglEmbedChapters().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglEmbedChapters().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        RowEmbedSubtitles().Title(winrt::to_hstring(_("Embed Subtitles")));
        RowEmbedSubtitles().Description(winrt::to_hstring(_("If disabled or if embedding is not supported, downloaded subtitles will be saved to separate files")));
        TglEmbedSubtitles().OnContent(winrt::box_value(winrt::to_hstring(_("On"))));
        TglEmbedSubtitles().OffContent(winrt::box_value(winrt::to_hstring(_("Off"))));
        LblPostprocessing().Text(winrt::to_hstring(_("Postprocessing")));
        RowFfmpegThreads().Title(winrt::to_hstring(_("FFmpeg Threads")));
        RowFfmpegThreads().Description(winrt::to_hstring(_("Limit the number of threads used by ffmpeg")));
        RowPostprocessingArguments().Title(winrt::to_hstring(_("Arguments")));
        RowPostprocessingArguments().Description(winrt::to_hstring(_("Arguments will be shown for selection in the add download dialog")));
        LblPostprocessingArgumentsAdd().Text(winrt::to_hstring(_("Add")));
    }

    void SettingsPage::Controller(const std::shared_ptr<PreferencesViewController>& controller)
    {
        m_controller = controller;
        //Load
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        CmbTheme().SelectedIndex(static_cast<int>(m_controller->getTheme()));
        for(const std::string& language : m_controller->getAvailableTranslationLanguages())
        {
            Windows::Foundation::IInspectable item{ winrt::box_value(winrt::to_hstring(language)) };
            CmbLanguage().Items().Append(item);
            if(language == m_controller->getTranslationLanguage())
            {
                CmbLanguage().SelectedItem(item);
            }
        }
        TglPreventSuspend().IsOn(m_controller->getPreventSuspend());
        CmbHistoryLength().SelectedIndex(static_cast<int>(m_controller->getHistoryLengthIndex()));
        NumMaxActiveDownloads().Value(static_cast<double>(options.getMaxNumberOfActiveDownloads()));
        TglOverwriteExistingFiles().IsOn(options.getOverwriteExistingFiles());
        TglIncludeMediaId().IsOn(options.getIncludeMediaIdInTitle());
        TglIncludeAutoSubtitles().IsOn(options.getIncludeAutoGeneratedSubtitles());
        CmbPreferredVideoCodec().SelectedIndex(static_cast<int>(options.getPreferredVideoCodec()));
        CmbPreferredAudioCodec().SelectedIndex(static_cast<int>(options.getPreferredAudioCodec()));
        CmbPreferredSubtitleFormat().SelectedIndex(static_cast<int>(options.getPreferredSubtitleFormat()));
        TglUsePartFiles().IsOn(options.getUsePartFiles());
        TglUseSponsorBlock().IsOn(options.getYouTubeSponsorBlock());
        if(options.getSpeedLimit())
        {
            TglLimitSpeed().IsOn(true);
            NumSpeedLimit().Value(static_cast<double>(*options.getSpeedLimit()));
        }
        TxtProxyUrl().Text(winrt::to_hstring(options.getProxyUrl()));
        CmbCookiesBrowser().SelectedIndex(static_cast<int>(options.getCookiesBrowser()));
        if(std::filesystem::exists(options.getCookiesPath()))
        {
            m_cookiesFilePath = options.getCookiesPath();
            LblCookiesFile().Text(winrt::to_hstring(std::filesystem::path(m_cookiesFilePath).filename().string()));
        }
        TglUseAria().IsOn(options.getUseAria());
        NumMaxConnectionsPerServer().Value(static_cast<double>(options.getAriaMaxConnectionsPerServer()));
        NumMinimumSplitSize().Value(static_cast<double>(options.getAriaMinSplitSize()));
        TglEmbedMetadata().IsOn(options.getEmbedMetadata());
        TglRemoveSourceData().IsOn(options.getRemoveSourceData());
        TglEmbedThumbnails().IsOn(options.getEmbedThumbnails());
        TglCropAudioThumbnails().IsOn(options.getCropAudioThumbnails());
        TglEmbedChapters().IsOn(options.getEmbedChapters());
        TglEmbedSubtitles().IsOn(options.getEmbedSubtitles());
        NumFfmpegThreads().Maximum(m_controller->getMaxPostprocessingThreads());
        NumFfmpegThreads().Value(static_cast<double>(options.getPostprocessingThreads()));
        ReloadPostprocessingArguments();
        m_constructing = false;
    }

    void SettingsPage::Hwnd(HWND hwnd)
    {
        m_hwnd = hwnd;
    }

    void SettingsPage::OnNavViewSelectionChanged(const SelectorBar& sender, const SelectorBarSelectionChangedEventArgs& args)
    {
        uint32_t index;
        if(sender.Items().IndexOf(sender.SelectedItem(), index))
        {
            ViewStack().CurrentPageIndex(static_cast<int>(index));
        }
    }

    void SettingsPage::OnCmbChanged(const IInspectable& sender, const SelectionChangedEventArgs& args)
    {
        ApplyChanges();
    }

    void SettingsPage::OnSwitchToggled(const IInspectable& sender, const RoutedEventArgs& args)
    {
        ApplyChanges();
    }

    void SettingsPage::OnNumChanged(const NumberBox& sender, const NumberBoxValueChangedEventArgs& args)
    {
        ApplyChanges();
    }

    void SettingsPage::OnTextChanged(const IInspectable& sender, const TextChangedEventArgs& args)
    {
        ApplyChanges();
    }

    Windows::Foundation::IAsyncAction SettingsPage::SelectCookiesFile(const IInspectable& sender, const RoutedEventArgs& args)
    {
        FileOpenPicker picker;
        picker.as<::IInitializeWithWindow>()->Initialize(m_hwnd);
        picker.FileTypeFilter().Append(L".txt");
        StorageFile file{ co_await picker.PickSingleFileAsync() };
        if(file)
        {
            m_cookiesFilePath = winrt::to_string(file.Path());
            LblCookiesFile().Text(winrt::to_hstring(std::filesystem::path(m_cookiesFilePath).filename().string()));
            ApplyChanges();
        }
    }

    void SettingsPage::ClearCookiesFile(const IInspectable& sender, const RoutedEventArgs& args)
    {
        m_cookiesFilePath.clear();
        LblCookiesFile().Text(winrt::to_hstring(_("No file selected")));
        ApplyChanges();
    }

    Windows::Foundation::IAsyncAction SettingsPage::AddPostprocessingArgument(const IInspectable& sender, const RoutedEventArgs& args)
    {
        TextBox txtName;
        txtName.Header(winrt::box_value(winrt::to_hstring(_("Name"))));
        txtName.PlaceholderText(winrt::to_hstring(_("Enter name here")));
        ComboBox cmbPostProcessor;
        cmbPostProcessor.HorizontalAlignment(HorizontalAlignment::Stretch);
        cmbPostProcessor.Header(winrt::box_value(winrt::to_hstring(_("Post Processor"))));
        for(const std::string& postProcessor : m_controller->getPostProcessorStrings())
        {
            cmbPostProcessor.Items().Append(winrt::box_value(winrt::to_hstring(postProcessor)));
        }
        cmbPostProcessor.SelectedIndex(0);
        ComboBox cmbExecutable;
        cmbExecutable.HorizontalAlignment(HorizontalAlignment::Stretch);
        cmbExecutable.Header(winrt::box_value(winrt::to_hstring(_("Executable"))));
        for(const std::string& executable : m_controller->getExecutableStrings())
        {
            cmbExecutable.Items().Append(winrt::box_value(winrt::to_hstring(executable)));
        }
        cmbExecutable.SelectedIndex(0);
        TextBox txtArgs;
        txtArgs.Header(winrt::box_value(winrt::to_hstring(_("Args"))));
        txtArgs.PlaceholderText(winrt::to_hstring(_("Enter args here")));
        StackPanel panel;
        panel.Orientation(Orientation::Vertical);
        panel.Spacing(12);
        panel.Children().Append(txtName);
        panel.Children().Append(cmbPostProcessor);
        panel.Children().Append(cmbExecutable);
        panel.Children().Append(txtArgs);
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("New Argument"))));
        dialog.Content(panel);
        dialog.PrimaryButtonText(winrt::to_hstring(_("Add")));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        while(true)
        {
            ContentDialogResult res{ co_await dialog.ShowAsync() };
            if(res == ContentDialogResult::Primary)
            {
                PostProcessorArgumentCheckStatus status{ m_controller->addPostprocessingArgument(winrt::to_string(txtName.Text()), static_cast<PostProcessor>(cmbPostProcessor.SelectedIndex()), static_cast<Executable>(cmbExecutable.SelectedIndex()), winrt::to_string(txtArgs.Text())) };
                ContentDialog errorDialog;
                errorDialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
                errorDialog.CloseButtonText(winrt::to_hstring(_("OK")));
                errorDialog.DefaultButton(ContentDialogButton::Close);
                errorDialog.RequestedTheme(MainGrid().RequestedTheme());
                errorDialog.XamlRoot(MainGrid().XamlRoot());
                switch(status)
                {
                case PostProcessorArgumentCheckStatus::EmptyName:
                {
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The argument name cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                }
                case PostProcessorArgumentCheckStatus::ExistingName:
                {
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("An argument with this name already exists."))));
                    co_await errorDialog.ShowAsync();
                    break;
                }
                case PostProcessorArgumentCheckStatus::EmptyArgs:
                {
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The argument args cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                }
                default:
                    ReloadPostprocessingArguments();
                    co_return;
                }
            }
            else
            {
                co_return;
            }
        }
    }

    Windows::Foundation::IAsyncAction SettingsPage::EditPostprocessingArgument(const IInspectable& sender, const RoutedEventArgs& args)
    {
        std::optional<PostProcessorArgument> argument{ m_controller->getPostprocessingArgument(winrt::to_string(winrt::unbox_value<winrt::hstring>(sender.as<FrameworkElement>().Tag()))) };
        if(!argument)
        {
            co_return;
        }
        TextBox txtName;
        txtName.Header(winrt::box_value(winrt::to_hstring(_("Name"))));
        txtName.Text(winrt::to_hstring(argument->getName()));
        txtName.IsEnabled(false);
        ComboBox cmbPostProcessor;
        cmbPostProcessor.HorizontalAlignment(HorizontalAlignment::Stretch);
        cmbPostProcessor.Header(winrt::box_value(winrt::to_hstring(_("Post Processor"))));
        for(const std::string& postProcessor : m_controller->getPostProcessorStrings())
        {
            cmbPostProcessor.Items().Append(winrt::box_value(winrt::to_hstring(postProcessor)));
        }
        cmbPostProcessor.SelectedIndex(static_cast<int>(argument->getPostProcessor()));
        ComboBox cmbExecutable;
        cmbExecutable.HorizontalAlignment(HorizontalAlignment::Stretch);
        cmbExecutable.Header(winrt::box_value(winrt::to_hstring(_("Executable"))));
        for(const std::string& executable : m_controller->getExecutableStrings())
        {
            cmbExecutable.Items().Append(winrt::box_value(winrt::to_hstring(executable)));
        }
        cmbExecutable.SelectedIndex(static_cast<int>(argument->getExecutable()));
        TextBox txtArgs;
        txtArgs.Header(winrt::box_value(winrt::to_hstring(_("Args"))));
        txtArgs.PlaceholderText(winrt::to_hstring(_("Enter args here")));
        txtArgs.Text(winrt::to_hstring(argument->getArgs()));
        StackPanel panel;
        panel.Orientation(Orientation::Vertical);
        panel.Spacing(12);
        panel.Children().Append(txtName);
        panel.Children().Append(cmbPostProcessor);
        panel.Children().Append(cmbExecutable);
        panel.Children().Append(txtArgs);
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("Edit Argument"))));
        dialog.Content(panel);
        dialog.PrimaryButtonText(winrt::to_hstring(_("Save")));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        ContentDialogResult res{ co_await dialog.ShowAsync() };
        while(true)
        {
            if(res == ContentDialogResult::Primary)
            {
                PostProcessorArgumentCheckStatus status{ m_controller->updatePostprocessingArgument(winrt::to_string(txtName.Text()), static_cast<PostProcessor>(cmbPostProcessor.SelectedIndex()), static_cast<Executable>(cmbExecutable.SelectedIndex()), winrt::to_string(txtArgs.Text())) };
                ContentDialog errorDialog;
                errorDialog.Title(winrt::box_value(winrt::to_hstring(_("Error"))));
                errorDialog.CloseButtonText(winrt::to_hstring(_("OK")));
                errorDialog.DefaultButton(ContentDialogButton::Close);
                errorDialog.RequestedTheme(MainGrid().RequestedTheme());
                errorDialog.XamlRoot(MainGrid().XamlRoot());
                switch(status)
                {
                case PostProcessorArgumentCheckStatus::EmptyArgs:
                {
                    errorDialog.Content(winrt::box_value(winrt::to_hstring(_("The argument args cannot be empty."))));
                    co_await errorDialog.ShowAsync();
                    break;
                }
                default:
                    ReloadPostprocessingArguments();
                    co_return;
                }
            }
            else
            {
                co_return;
            }
        }
    }

    Windows::Foundation::IAsyncAction SettingsPage::DeletePostprocessingArgument(const IInspectable& sender, const RoutedEventArgs& args)
    {
        std::string argumentName{ winrt::to_string(winrt::unbox_value<winrt::hstring>(sender.as<FrameworkElement>().Tag())) };
        ContentDialog dialog;
        dialog.Title(winrt::box_value(winrt::to_hstring(_("Delete Argument?"))));
        dialog.Content(winrt::box_value(winrt::to_hstring(_("Are you sure you want to delete this argument?"))));
        dialog.PrimaryButtonText(winrt::to_hstring(_("Delete")));
        dialog.CloseButtonText(winrt::to_hstring(_("Cancel")));
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.RequestedTheme(MainGrid().RequestedTheme());
        dialog.XamlRoot(MainGrid().XamlRoot());
        ContentDialogResult res{ co_await dialog.ShowAsync() };
        if(res == ContentDialogResult::Primary)
        {
            m_controller->deletePostprocessingArgument(argumentName);
            ReloadPostprocessingArguments();
        }
    }

    void SettingsPage::ReloadPostprocessingArguments()
    {
        m_controller->saveConfiguration();
        ListPostprocessingArguments().Children().Clear();
        for(const PostProcessorArgument& argument : m_controller->getDownloaderOptions().getPostprocessingArguments())
        {
            FontIcon icnEdit;
            icnEdit.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnEdit.Glyph(L"\uE70F");
            Button btnEdit;
            btnEdit.Content(icnEdit);
            btnEdit.Tag(winrt::box_value(winrt::to_hstring(argument.getName())));
            btnEdit.Click({ this, &SettingsPage::EditPostprocessingArgument });
            ToolTipService::SetToolTip(btnEdit, winrt::box_value(winrt::to_hstring(_("Edit Argument"))));
            FontIcon icnDelete;
            icnDelete.FontFamily(WinUIHelpers::LookupAppResource<Microsoft::UI::Xaml::Media::FontFamily>(L"SymbolThemeFontFamily"));
            icnDelete.Glyph(L"\uE74D");
            Button btnDelete;
            btnDelete.Content(icnDelete);
            btnDelete.Tag(winrt::box_value(winrt::to_hstring(argument.getName())));
            btnDelete.Click({ this, &SettingsPage::DeletePostprocessingArgument });
            ToolTipService::SetToolTip(btnDelete, winrt::box_value(winrt::to_hstring(_("Delete Argument"))));
            StackPanel panel;
            panel.Orientation(Orientation::Horizontal);
            panel.Spacing(6);
            panel.Children().Append(btnEdit);
            panel.Children().Append(btnDelete);
            Controls::SettingsRow row{ winrt::make<Controls::implementation::SettingsRow>() };
            row.Title(winrt::to_hstring(argument.getName()));
            row.Description(winrt::to_hstring(argument.getArgs()));
            row.Child(panel);
            ListPostprocessingArguments().Children().Append(row);
        }
        if(m_controller->getDownloaderOptions().getPostprocessingArguments().empty())
        {
            Controls::SettingsRow row{ winrt::make<Controls::implementation::SettingsRow>() };
            row.Title(winrt::to_hstring(_("No Arguments")));
            ListPostprocessingArguments().Children().Append(row);
        }
    }

    void SettingsPage::ApplyChanges()
    {
        if(m_constructing)
        {
            return;
        }
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_controller->setTheme(static_cast<Theme>(CmbTheme().SelectedIndex()));
        m_controller->setTranslationLanguage(winrt::to_string(CmbLanguage().SelectedItem().as<winrt::hstring>()));
        m_controller->setPreventSuspend(TglPreventSuspend().IsOn());
        m_controller->setHistoryLengthIndex(CmbHistoryLength().SelectedIndex());
        options.setMaxNumberOfActiveDownloads(static_cast<int>(NumMaxActiveDownloads().Value()));
        options.setOverwriteExistingFiles(TglOverwriteExistingFiles().IsOn());
        options.setIncludeMediaIdInTitle(TglIncludeMediaId().IsOn());
        options.setIncludeAutoGeneratedSubtitles(TglIncludeAutoSubtitles().IsOn());
        options.setPreferredVideoCodec(static_cast<VideoCodec>(CmbPreferredVideoCodec().SelectedIndex()));
        options.setPreferredAudioCodec(static_cast<AudioCodec>(CmbPreferredAudioCodec().SelectedIndex()));
        options.setPreferredSubtitleFormat(static_cast<SubtitleFormat>(CmbPreferredSubtitleFormat().SelectedIndex()));
        options.setUsePartFiles(TglUsePartFiles().IsOn());
        options.setYouTubeSponsorBlock(TglUseSponsorBlock().IsOn());
        options.setSpeedLimit(TglLimitSpeed().IsOn() ? std::make_optional<int>(static_cast<int>(NumSpeedLimit().Value())) : std::nullopt);
        options.setProxyUrl(winrt::to_string(TxtProxyUrl().Text()));
        options.setCookiesBrowser(static_cast<Browser>(CmbCookiesBrowser().SelectedIndex()));
        options.setCookiesPath(m_cookiesFilePath);
        options.setUseAria(TglUseAria().IsOn());
        options.setAriaMaxConnectionsPerServer(static_cast<int>(NumMaxConnectionsPerServer().Value()));
        options.setAriaMinSplitSize(static_cast<int>(NumMinimumSplitSize().Value()));
        options.setEmbedMetadata(TglEmbedMetadata().IsOn());
        options.setRemoveSourceData(TglRemoveSourceData().IsOn());
        options.setEmbedThumbnails(TglEmbedThumbnails().IsOn());
        options.setCropAudioThumbnails(TglCropAudioThumbnails().IsOn());
        options.setEmbedChapters(TglEmbedChapters().IsOn());
        options.setEmbedSubtitles(TglEmbedSubtitles().IsOn());
        options.setPostprocessingThreads(static_cast<int>(NumFfmpegThreads().Value()));
        m_controller->setDownloaderOptions(options);
        m_controller->saveConfiguration();
    }
}
