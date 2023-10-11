using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AppLifecycle;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading.Tasks;
using static Nickvision.Aura.Localization.Gettext;

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
        Title = _("Settings");
        PrimaryButtonText = _("Apply");
        CloseButtonText = _("Cancel");
        LblUserInterface.Text = _("User Interface");
        CardTheme.Header = _("Theme");
        CardTheme.Description = _("An application restart is required to change the theme.");
        CmbTheme.Items.Add(_p("Theme", "Light"));
        CmbTheme.Items.Add(_p("Theme", "Dark"));
        CmbTheme.Items.Add(_p("Theme", "System"));
        CardAutomaticallyCheckForUpdates.Header = _("Automatically Check for Updates");
        TglAutomaticallyCheckForUpdates.OnContent = _("On");
        TglAutomaticallyCheckForUpdates.OffContent = _("Off");
        CardCompletedNotification.Header = _("Completed Notification Trigger");
        CardCompletedNotification.Description = _("In combination with the trigger, completed notifications will only be shown if the window is not focused.");
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "For each download"));
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "When all downloads finish"));
        CmbCompletedNotification.Items.Add(_p("CompletedNotification", "Never"));
        CardSuspend.Header = _("Prevent Suspend");
        TglSuspend.OnContent = _("On");
        TglSuspend.OffContent = _("Off");
        CardBackground.Header = _("Allow Running In Background");
        CardBackground.Description = _("Hide the window instead of quitting if there are downloads running.");
        TglBackground.OnContent = _("On");
        TglBackground.OffContent = _("Off");
        LblDownloader.Text = _("Downloader");
        CardMaxNumberOfActiveDownloads.Header = _("Max Number of Active Downloads");
        CardOverwrite.Header = _("Overwrite Existing Files");
        TglOverwrite.OnContent = _("On");
        TglOverwrite.OffContent = _("Off");
        CardSpeedLimit.Header = _("Speed Limit");
        CardSpeedLimit.Description = _("This limit (in KiB/s) is applied to downloads that have speed limit enabled. Changing the value doesn't affect already running downloads.");
        CardUseAria.Header = _("Use aria2");
        CardUseAria.Description = _("Enable to use aria2 downloader. It can be faster, but you will not see download progress.");
        TglUseAria.OnContent = _("On");
        TglUseAria.OffContent = _("Off");
        CardAriaMaxConnectionsPerServer.Header = _("Max Connections Per Server (-x)");
        ToolTipService.SetToolTip(BtnAriaMaxConnectionsPerServerReset, _("Reset to default"));
        CardAriaMinSplitSize.Header = _("Minimum Split Size (-k)");
        CardAriaMinSplitSize.Description = _("The minimum size of which to split a file (in MiB).");
        ToolTipService.SetToolTip(BtnAriaMinSplitSizeReset, _("Reset to default"));
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        TglAutomaticallyCheckForUpdates.IsOn = _controller.AutomaticallyCheckForUpdates;
        CmbCompletedNotification.SelectedIndex = (int)_controller.CompletedNotificationPreference;
        TglSuspend.IsOn = _controller.PreventSuspendWhenDownloading;
        TglBackground.IsOn = _controller.RunInBackground;
        TxtMaxNumberOfActiveDownloads.Value = _controller.MaxNumberOfActiveDownloads;
        TglOverwrite.IsOn = _controller.OverwriteExistingFiles;
        TxtSpeedLimit.Value = _controller.SpeedLimit;
        TglUseAria.IsOn = _controller.UseAria;
        TxtAriaMaxConnectionsPerServer.Value = _controller.AriaMaxConnectionsPerServer;
        TxtAriaMinSplitSize.Value = _controller.AriaMinSplitSize;
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
            _controller.AutomaticallyCheckForUpdates = TglAutomaticallyCheckForUpdates.IsOn;
            _controller.CompletedNotificationPreference = (NotificationPreference)CmbCompletedNotification.SelectedIndex;
            _controller.PreventSuspendWhenDownloading = TglSuspend.IsOn;
            _controller.RunInBackground = TglBackground.IsOn;
            _controller.MaxNumberOfActiveDownloads = (int)TxtMaxNumberOfActiveDownloads.Value;
            _controller.OverwriteExistingFiles = TglOverwrite.IsOn;
            _controller.SpeedLimit = (uint)TxtSpeedLimit.Value;
            _controller.UseAria = TglUseAria.IsOn;
            _controller.AriaMaxConnectionsPerServer = (int)TxtAriaMaxConnectionsPerServer.Value;
            _controller.AriaMinSplitSize = (int)TxtAriaMinSplitSize.Value;
            _controller.SaveConfiguration();
            if (needsRestart)
            {
                var restartDialog = new ContentDialog()
                {
                    Title = _("Restart To Apply Theme?"),
                    Content = _("Would you like to restart {0} to apply the new theme?\nAny unsaved work will be lost.", _controller.AppInfo.ShortName),
                    PrimaryButtonText = _("Yes"),
                    CloseButtonText = _("No"),
                    DefaultButton = ContentDialogButton.Primary,
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
    /// Occurs when the AriaMaxConnectionsPerServerReset button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AriaMaxConnectionsPerServerReset(object sender, RoutedEventArgs e) => TxtAriaMaxConnectionsPerServer.Value = 16;

    /// <summary>
    /// Occurs when the AriaMinSplitSizeReset button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void AriaMinSplitSizeReset(object sender, RoutedEventArgs e) => TxtAriaMinSplitSize.Value = 20;
}
