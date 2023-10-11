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
        //Load Config
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        TglAutomaticallyCheckForUpdates.IsOn = _controller.AutomaticallyCheckForUpdates;
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
}
