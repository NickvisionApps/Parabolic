using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AppLifecycle;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// A dialog for managing application settings
/// </summary>
public sealed partial class SettingsDialog : ContentDialog
{
    private readonly PreferencesViewController _controller;

    /// <summary>
    /// Constructs a SettingsDialog
    /// </summary>
    /// <param name="controller">PreferencesViewController</param>
    public SettingsDialog(PreferencesViewController controller)
    {
        InitializeComponent();
        _controller = controller;
        //Localize Strings
        Title = _controller.Localizer["Settings"];
        PrimaryButtonText = _controller.Localizer["Apply"];
        CloseButtonText = _controller.Localizer["Cancel"];
        LblUserInterface.Text = _controller.Localizer["UserInterface"];
        LblUserInterfaceDescription.Text = _controller.Localizer["UserInterfaceDescription"];
        CardTheme.Header = _controller.Localizer["Theme"];
        CardTheme.Description = _controller.Localizer["ThemeDescription", "WinUI"];
        CmbTheme.Items.Add(_controller.Localizer["ThemeLight"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeDark"]);
        CmbTheme.Items.Add(_controller.Localizer["ThemeSystem"]);
        CardAllowBackground.Header = _controller.Localizer["AllowBackground"];
        CardAllowBackground.Description = _controller.Localizer["AllowBackgroundDescription"];
        LblDownloader.Text = _controller.Localizer["Downloader"];
        LblDownloaderDescription.Text = _controller.Localizer["Downloader", "Description"];
        CardMaxNumberOfActiveDownloads.Header = _controller.Localizer["MaxNumberOfActiveDownloads"];
        CardSpeedLimit.Header = _controller.Localizer["SpeedLimit"];
        CardSpeedLimit.Description = _controller.Localizer["SpeedLimit", "Description"];
        CardUseAria.Header = _controller.Localizer["UseAria"];
        CardUseAria.Description = _controller.Localizer["UseAria", "Description"];
        LblConverter.Text = _controller.Localizer["Converter"];
        LblConverterDescription.Text = _controller.Localizer["Converter", "Description"];
        CardEmbedMetadata.Header = _controller.Localizer["EmbedMetadata"];
        CardEmbedMetadata.Description = _controller.Localizer["EmbedMetadata", "Description"];
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        ToggleAllowBackground.IsOn = _controller.RunInBackground;
        NumMaxNumberOfActiveDownloads.Value = _controller.MaxNumberOfActiveDownloads;
        NumSpeedLimit.Value = _controller.SpeedLimit;
        ToggleUseAria.IsOn = _controller.UseAria;
        ToggleEmbedMetadata.IsOn = _controller.EmbedMetadata;
    }

    /// <summary>
    /// Shows the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync()
    {
        var result = await base.ShowAsync();
        if (result == ContentDialogResult.Primary)
        {
            var needsRestart = false;
            if (_controller.Theme != (Theme)CmbTheme.SelectedIndex)
            {
                _controller.Theme = (Theme)CmbTheme.SelectedIndex;
                needsRestart = true;
            }
            _controller.RunInBackground = ToggleAllowBackground.IsOn;
            var maxNumberOfActiveDownloads = (int)NumMaxNumberOfActiveDownloads.Value;
            if (maxNumberOfActiveDownloads < 1 && (int)NumMaxNumberOfActiveDownloads.Value > 10)
            {
                maxNumberOfActiveDownloads = 5;
            }
            _controller.MaxNumberOfActiveDownloads = maxNumberOfActiveDownloads;
            _controller.SpeedLimit = (uint)NumSpeedLimit.Value;
            _controller.UseAria = ToggleUseAria.IsOn;
            _controller.EmbedMetadata = ToggleEmbedMetadata.IsOn;
            _controller.SaveConfiguration();
            if (needsRestart)
            {
                var restartDialog = new ContentDialog()
                {
                    Title = _controller.Localizer["RestartThemeTitle", "WinUI"],
                    Content = string.Format(_controller.Localizer["RestartThemeDescription", "WinUI"], _controller.AppInfo.ShortName),
                    PrimaryButtonText = _controller.Localizer["Yes"],
                    CloseButtonText = _controller.Localizer["No"],
                    DefaultButton = ContentDialogButton.Close,
                    XamlRoot = XamlRoot
                };
                var resultRestart = await restartDialog.ShowAsync();
                if (resultRestart == ContentDialogResult.Primary)
                {
                    AppInstance.Restart("Apply new theme");
                }
            }
        }
        return result;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the NumMaxNumberOfActiveDownloads' value is changed
    /// </summary>
    /// <param name="sender">NumberBox</param>
    /// <param name="args">NumberBoxValueChangedEventArgs</param>
    private void NumMaxNumberOfActiveDownloads_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
    {
        if (NumMaxNumberOfActiveDownloads.Value % 1 != 0)
        {
            NumMaxNumberOfActiveDownloads.Value = Math.Round(NumMaxNumberOfActiveDownloads.Value);
        }
    }
}