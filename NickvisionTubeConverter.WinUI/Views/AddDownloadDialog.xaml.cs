using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Aura;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.WinUI.Controls;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public sealed partial class AddDownloadDialog : ContentDialog
{
    private readonly AddDownloadDialogController _controller;
    private readonly List<string> _audioQualities;
    private string _saveFolderString;

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    /// <param name="controller">AddDownloadDialogController</param>
    public AddDownloadDialog(AddDownloadDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        _audioQualities = new List<string>() { _("Best"), _("Worst") };
        //Localize Strings
        Title = _("Add Download");
        CloseButtonText = _("Close");
        LblBtnBack.Text = _("Back");
        CardUrl.Header = _("Media URL");
        TxtUrl.PlaceholderText = _("Ender media url here");
        CardAuthenticate.Header = _("Authenticate");
        TglAuthenticate.OnContent = _("On");
        TglAuthenticate.OffContent = _("Off");
        CardKeyringCredentials.Header = _("Keyring Credential");
        CardUsername.Header = _("Username");
        TxtUsername.PlaceholderText = _("Enter user name here");
        CardPassword.Header = _("Password");
        TxtPassword.PlaceholderText = _("Enter password here");
        BtnValidate.Content = _("Validate");
        //Load
        ViewStack.CurrentPageName = "Url";
        if (Directory.Exists(_controller.PreviousSaveFolder))
        {
            _saveFolderString = _controller.PreviousSaveFolder;
        }
        else
        {
            _saveFolderString = UserDirectories.Downloads;
        }
    }

    /// <summary>
    /// Show the dialog
    /// </summary>
    /// <returns>ContentDialogResult</returns>
    public new async Task<ContentDialogResult> ShowAsync() => await ShowAsync(null);

    /// <summary>
    /// Show the dialog
    /// </summary>
    /// <param name="url">A url to validate on startup</param>
    /// <returns>ContentDialogResult</returns>
    public async Task<ContentDialogResult> ShowAsync(string? url)
    {
        //Validated from startup
        if (!string.IsNullOrEmpty(url))
        {
            TxtUrl.Text = url;
            await SearchUrlAsync(url);
        }
        else
        {
            //Validate Clipboard
            try
            {
                var clipboardText = await Clipboard.GetContent().GetTextAsync();
                if (!string.IsNullOrEmpty(clipboardText))
                {
                    var result = Uri.TryCreate(clipboardText, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
                    if (result)
                    {
                        TxtUrl.Text = clipboardText;
                        TxtUrl.Select(clipboardText.Length, 0);
                    }
                }
            }
            catch { }
        }
        //Keyring
        var names = await _controller.GetKeyringCredentialNamesAsync();
        if (names.Count > 0)
        {
            CmbKeyringCredentials.ItemsSource = names;
            CmbKeyringCredentials.SelectedIndex = names.Count > 1 ? 1 : 0;
        }
        else
        {
            CardKeyringCredentials.Visibility = Visibility.Collapsed;
            CardUsername.Visibility = Visibility.Visible;
            CardPassword.Visibility = Visibility.Visible;
        }
        var res = await base.ShowAsync();
        if (res == ContentDialogResult.Primary)
        {
            //TODO: Populate downloads
        }
        return res;
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => ViewStack.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the back button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void Back(object sender, RoutedEventArgs e) => ViewStack.CurrentPageName = "Downloads";

    /// <summary>
    /// Occurs when the ViewStack's page is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">PageChangedEventArgs</param>
    private void ViewStack_PageChanged(object sender, PageChangedEventArgs e)
    {
        BtnBack.Visibility = ViewStack.CurrentPageName == "Playlist" || ViewStack.CurrentPageName == "Advanced" ? Visibility.Visible : Visibility.Collapsed;
        Title = ViewStack.CurrentPageName switch
        {
            "Playlist" => _("Playlist"),
            "Advanced" => _("Advanced Options"),
            _ => _("Add Download")
        };
    }

    /// <summary>
    /// Occurs when TxtUrl's text is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">TextChangedEventArgs</param>
    private void TxtUrl_TextChanged(object sender, TextChangedEventArgs e)
    {
        if (!string.IsNullOrEmpty(TxtUrl.Text))
        {
            var result = Uri.TryCreate(TxtUrl.Text, UriKind.Absolute, out var uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps);
            BtnValidate.IsEnabled = result;
        }
    }

    /// <summary>
    /// Occurs when TglAuthenticate is toggled
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private void TglAuthenticate_Toggled(object sender, RoutedEventArgs e)
    {
        TxtUsername.Text = "";
        TxtPassword.Password = "";
    }

    /// <summary>
    /// Occurs when CmbKeyringCredentials' selection is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SelectionChangedEventArgs</param>
    private void CmbKeyringCredentials_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (CmbKeyringCredentials.SelectedIndex == 0)
        {
            TxtUsername.Visibility = Visibility.Visible;
            TxtPassword.Visibility = Visibility.Visible;
        }
        else
        {
            TxtUsername.Visibility = Visibility.Collapsed;
            TxtPassword.Visibility = Visibility.Collapsed;
        }
    }

    /// <summary>
    /// Occurs when the validate button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void Validate(object sender, RoutedEventArgs e) => await SearchUrlAsync(TxtUrl.Text);

    /// <summary>
    /// Searches for information about a URL in the dialog
    /// </summary>
    /// <param name="url">The URL to search</param>
    private async Task SearchUrlAsync(string url)
    {
        CardUrl.Header = _("Media URL");
        ProgRingUrl.Visibility = Visibility.Visible;
        BtnValidate.IsEnabled = false;
        try
        {
            if (CmbKeyringCredentials.SelectedIndex == 0 || CmbKeyringCredentials.SelectedIndex == -1 || !TglAuthenticate.IsOn)
            {
                await _controller.SearchUrlAsync(url, TxtUsername.Text, TxtPassword.Password);
            }
            else
            {
                await _controller.SearchUrlAsync(url, CmbKeyringCredentials.SelectedIndex - 1);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
            Console.WriteLine(ex.StackTrace);
        }
        ProgRingUrl.Visibility = Visibility.Collapsed;
        BtnValidate.IsEnabled = true;
        if (!_controller.HasMediaInfo)
        {
            CardUrl.Header = _("Media URL (Invalid)");
            if (TglAuthenticate.IsOn)
            {
                InfoBar.Content = _("Ensure credentials are correct.");
                InfoBar.Severity = InfoBarSeverity.Warning;
                InfoBar.IsOpen = true;
            }
        }
        else
        {
            ViewStack.CurrentPageName = "Download";
            PrimaryButtonText = _("Download");
            DefaultButton = ContentDialogButton.Primary;
        }
    }
}
