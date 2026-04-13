using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Controllers;
using System;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class HistoryDialog : ContentDialog
{
    private readonly HistoryViewController _controller;
    private readonly ITranslationService _translationService;

    public HistoryDialog(HistoryViewController controller, ITranslationService translationService)
    {
        InitializeComponent();
        _controller = controller;
        _translationService = translationService;
        Title = _translationService._("History");
        CloseButtonText = _translationService._("Close");
    }

    public new async Task<ContentDialogResult> ShowAsync()
    {
        return await base.ShowAsync();
    }
}
