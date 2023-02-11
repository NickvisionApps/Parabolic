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
        LblBtnChangelog.Text = _controller.Localizer["Changelog"];
        LblBtnCredits.Text = _controller.Localizer["Credits"];
        LblBtnGitHubRepo.Text = _controller.Localizer["GitHubRepo"];
        LblBtnReportABug.Text = _controller.Localizer["ReportABug"];
        LblBtnDiscussions.Text = _controller.Localizer["Discussions"];
        CardUserInterface.Header = _controller.Localizer["UserInterface"];
        CardUserInterface.Description = _controller.Localizer["UserInterfaceDescription"];
        CardTheme.Header = _controller.Localizer["Theme"];
        CardTheme.Description = _controller.Localizer["ThemeDescription", "WinUI"];
        CmbTheme.Items.Add(_controller.Localizer["ThemeLight"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeDark"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeSystem"]);
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
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
    /// Occurs when the CmbTheme selection is changed
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
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
}
