using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AppLifecycle;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using Windows.System;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The PreferencesPage for the application
/// </summary>
public sealed partial class PreferencesPage : UserControl
{
    private readonly PreferencesViewController _controller;

    /// <summary>
    /// Constructs a PreferencesPage
    /// </summary>
    /// <param name="controller">PreferencesViewController</param>
    public PreferencesPage(PreferencesViewController controller)
    {
        InitializeComponent();
        _controller = controller;
        //Localize Strings
        LblTitle.Text = _controller.Localizer["Settings"];
        LblAbout.Text = string.Format(_controller.Localizer["About"], _controller.AppInfo.Name);
        LblDescription.Text = $"{_controller.AppInfo.Description}\n";
        LblVersion.Text = string.Format(_controller.Localizer["Version"], _controller.AppInfo.Version);
        LblCopyright.Text += $"\n{_controller.Localizer["Disclaimer"]}\n";
        LblBtnSupportedSites.Text = _controller.Localizer["SupportedSites"];
        LblBtnChangelog.Text = _controller.Localizer["Changelog"];
        LblBtnCredits.Text = _controller.Localizer["Credits"];
        LblBtnGitHubRepo.Text = _controller.Localizer["GitHubRepo"];
        LblBtnReportABug.Text = _controller.Localizer["ReportABug"];
        LblBtnDiscussions.Text = _controller.Localizer["Discussions"];
        LblBtnMatrixChat.Text = _controller.Localizer["MatrixChat"];
        CardUserInterface.Header = _controller.Localizer["UserInterface"];
        CardUserInterface.Description = _controller.Localizer["UserInterfaceDescription"];
        CardTheme.Header = _controller.Localizer["Theme"];
        CardTheme.Description = _controller.Localizer["ThemeDescription", "WinUI"];
        CardAllowBackground.Header = _controller.Localizer["AllowBackground"];
        CardAllowBackground.Description = _controller.Localizer["AllowBackgroundDescription"];
        CmbTheme.Items.Add(_controller.Localizer["ThemeLight"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeDark"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeSystem"]);
        CardDownloader.Header = _controller.Localizer["Downloader"];
        CardDownloader.Description = _controller.Localizer["Downloader", "Description"];
        CardMaxNumberOfActiveDownloads.Header = _controller.Localizer["MaxNumberOfActiveDownloads"];
        for (var i = 0; i < 10; i++)
        {
            CmbMaxNumberOfActiveDownloads.Items.Add(i + 1);
        }
        CardSpeedLimit.Header = _controller.Localizer["SpeedLimit"];
        CardSpeedLimit.Description = _controller.Localizer["SpeedLimit", "Description"];
        CardConverter.Header = _controller.Localizer["Converter"];
        CardConverter.Description = _controller.Localizer["Converter", "Description"];
        CardEmbedMetadata.Header = _controller.Localizer["EmbedMetadata"];
        CardEmbedMetadata.Description = _controller.Localizer["EmbedMetadata", "Description"];
        ToggleEmbedMetadata.OnContent = "";
        ToggleEmbedMetadata.OffContent = "";
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        ToggleAllowBackground.IsOn = _controller.RunInBackground;
        ToggleEmbedMetadata.IsOn = _controller.EmbedMetadata;
        CmbMaxNumberOfActiveDownloads.SelectedIndex = _controller.MaxNumberOfActiveDownloads - 1;
    }

    /// <summary>
    /// Removes URLs from a credits string
    /// </summary>
    /// <param name="s">The credits string</param>
    /// <returns>The new credits string with URLs removed</returns>
    private string RemoveUrlsFromCredits(string s)
    {
        var credits = s.Split('\n');
        var result = "";
        for (int i = 0; i < credits.Length; i++)
        {
            if (credits[i].IndexOf("https://") != -1)
            {
                result += credits[i].Remove(credits[i].IndexOf("https://"));
            }
            else if (credits[i].IndexOf("http://") != -1)
            {
                result += credits[i].Remove(credits[i].IndexOf("http://"));
            }
            if (i != credits.Length - 1)
            {
                result += "\n";
            }
        }
        return result;
    }

    /// <summary>
    /// Occurs when the supported sites button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void SupportedSites(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri("https://github.com/yt-dlp/yt-dlp/blob/master/supportedsites.md"));

    /// <summary>
    /// Occurs when the changelog button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Changelog(object sender, RoutedEventArgs e)
    {
        var changelogDialog = new ContentDialog()
        {
            Title = _controller.Localizer["ChangelogTitle", "WinUI"],
            Content = _controller.AppInfo.Changelog,
            CloseButtonText = _controller.Localizer["OK"],
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = Content.XamlRoot
        };
        await changelogDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the credits button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Credits(object sender, RoutedEventArgs e)
    {
        var creditsDialog = new ContentDialog()
        {
            Title = _controller.Localizer["Credits"],
            Content = string.Format(_controller.Localizer["CreditsDialogDescription", "WinUI"], RemoveUrlsFromCredits(_controller.Localizer["Developers", "Credits"]), RemoveUrlsFromCredits(_controller.Localizer["Designers", "Credits"]), RemoveUrlsFromCredits(_controller.Localizer["Artists", "Credits"]), RemoveUrlsFromCredits(_controller.Localizer["Translators", "Credits"])),
            CloseButtonText = _controller.Localizer["OK"],
            DefaultButton = ContentDialogButton.Close,
            XamlRoot = Content.XamlRoot
        };
        await creditsDialog.ShowAsync();
    }

    /// <summary>
    /// Occurs when the github repo button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void GitHubRepo(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.GitHubRepo);

    /// <summary>
    /// Occurs when the report a bug button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void ReportABug(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.IssueTracker);

    /// <summary>
    /// Occurs when the discussions button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Discussions(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(_controller.AppInfo.SupportUrl);

    /// <summary>
    /// Occurs when the matrix chat button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void MatrixChat(object sender, RoutedEventArgs e) => await Launcher.LaunchUriAsync(new Uri("https://matrix.to/#/#nickvision:matrix.org"));

    /// <summary>
    /// Occurs when the CmbTheme selection is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private async void CmbTheme_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (_controller.Theme != (Theme)CmbTheme.SelectedIndex)
        {
            _controller.Theme = (Theme)CmbTheme.SelectedIndex;
            _controller.SaveConfiguration();
            var restartDialog = new ContentDialog()
            {
                Title = _controller.Localizer["RestartThemeTitle", "WinUI"],
                Content = string.Format(_controller.Localizer["RestartThemeDescription", "WinUI"], _controller.AppInfo.ShortName),
                PrimaryButtonText = _controller.Localizer["Yes"],
                CloseButtonText = _controller.Localizer["No"],
                DefaultButton = ContentDialogButton.Close,
                XamlRoot = Content.XamlRoot
            };
            var result = await restartDialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                AppInstance.Restart("Apply new theme");
            }
        }
    }

    /// <summary>
    /// Occurs when the ToggleAllowBackground switch is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ToggleAllowBackground_Toggled(object sender, RoutedEventArgs e)
    {
        _controller.RunInBackground = ToggleAllowBackground.IsOn;
        _controller.SaveConfiguration();
    }

    /// <summary>
    /// Occurs when the CmbMaxNumberOfActiveDownloads' selection is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private void CmbMaxNumberOfActiveDownloads_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        _controller.MaxNumberOfActiveDownloads = CmbMaxNumberOfActiveDownloads.SelectedIndex + 1;
        _controller.SaveConfiguration();
    }

    /// <summary>
    /// Occurs when the NumSpeedLimit's value is changed
    /// </summary>
    /// <param name="sender">NumberBox</param>
    /// <param name="args">NumberBoxValueChangedEventArgs</param>
    private void NumSpeedLimit_ValueChange(NumberBox sender, NumberBoxValueChangedEventArgs args)
    {
        _controller.SpeedLimit = (uint)NumSpeedLimit.Value;
        _controller.SaveConfiguration();
    }

    /// <summary>
    /// Occurs when the ToggleEmbedMetadata switch is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void ToggleEmbedMetadata_Toggled(object sender, RoutedEventArgs e)
    {
        _controller.EmbedMetadata = ToggleEmbedMetadata.IsOn;
        _controller.SaveConfiguration();
    }
}
