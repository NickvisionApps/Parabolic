using Microsoft.UI.Xaml.Controls;
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

    /// <summary>
    /// Occurs when a notification is sent
    /// </summary>
    public event EventHandler<NotificationSentEventArgs>? NotificationSent;

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
    }
}
