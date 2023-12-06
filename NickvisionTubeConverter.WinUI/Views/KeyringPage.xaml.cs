using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Nickvision.Aura.Events;
using Nickvision.Aura.Keyring;
using System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Views;

/// <summary>
/// A page for managing credentials
/// </summary>
public sealed partial class KeyringPage : UserControl
{
    private readonly KeyringDialogController _controller;
    private readonly Button _btnConfirmDisable;
    private readonly Flyout _disableFlyout;

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;
    /// <summary>
    /// Occurs when the keyring is updated
    /// </summary>
    public event EventHandler<KeyringDialogController>? KeyringUpdated;

    /// <summary>
    /// Constructs a KeyringPage
    /// </summary>
    /// <param name="controller">KeyringDialogController</param>
    public KeyringPage(KeyringDialogController controller)
    {
        InitializeComponent();
        _controller = controller;
        //Localize Strings
        LblTitle.Text = _("Keyring");
        IconBtnEnableDisable.Glyph = "\uE785";
        LblBtnEnableDisable.Text = _("Enable");
        StatusPageDisabled.Title = _("Keyring Disabled");
        StatusPageDisabled.Description = _("Use keyring to safely store credentials for sites that require a user name and password to login.");
        BtnAddCredential.Label = _("Add Login");
        StatusPageNoCredentials.Title = _("No Credentials");
        _btnConfirmDisable = new Button()
        {
            Margin = new Thickness(0, 6, 0, 0),
            Content = _("Disable"),
            Style = (Style)Application.Current.Resources["AccentButtonStyle"],
        };
        _btnConfirmDisable.Click += ConfirmDisable;
        _disableFlyout = new Flyout()
        {
            Content = new StackPanel()
            {
                Margin = new Thickness(6, 6, 6, 6),
                Orientation = Orientation.Vertical,
                Spacing = 6,
                MaxWidth = 300,
                Children =
                {
                    new TextBlock()
                    {
                        TextWrapping = TextWrapping.WrapWholeWords,
                        Text = _("Disable Keyring?")
                    },
                    new TextBlock()
                    {
                        TextWrapping = TextWrapping.WrapWholeWords,
                        Text = _("Disabling the keyring will delete all data currently stored inside. Are you sure you want to disable it?"),
                        Foreground = new SolidColorBrush(Colors.Gray)
                    },
                    _btnConfirmDisable
                }
            }
        };
    }

    private void Page_Loaded(object sender, RoutedEventArgs e)
    {

    }

    private void EnableDisable(object sender, RoutedEventArgs e)
    {
        if (LblBtnEnableDisable.Text == _("Enable"))
        {

        }
    }

    private void ConfirmDisable(object sender, RoutedEventArgs e)
    {

    }

    private void AddLogin(object sender, RoutedEventArgs e)
    {

    }
}
