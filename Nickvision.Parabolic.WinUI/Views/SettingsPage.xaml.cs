using Microsoft.UI.Xaml.Controls;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.WinUI.Views;

public sealed partial class SettingsPage : Page
{
    private PreferencesViewController _controller;
    private bool _constructing;

    public SettingsPage(PreferencesViewController controller)
    {
        InitializeComponent();
        _controller = controller;
        _constructing = true;
        // Translations
        LblSettings.Text = _controller.Translator._("Settings");
        SelectorUI.Text = _controller.Translator._("User Interface");
        RowTheme.Header = _controller.Translator._("Theme");
        CmbTheme.Items.Add(_controller.Translator._p("Theme", "Light"));
        CmbTheme.Items.Add(_controller.Translator._p("Theme", "Dark"));
        CmbTheme.Items.Add(_controller.Translator._p("Theme", "System"));
        RowTranslationLanguage.Header = _controller.Translator._("Translation Language");
        RowTranslationLanguage.Description = _controller.Translator._("An application restart is required for a change to take effect");
        RowPreviewUpdates.Header = _controller.Translator._("Receive Preview Updates");
        TglPreviewUpdates.OnContent = _controller.Translator._("On");
        TglPreviewUpdates.OffContent = _controller.Translator._("Off");
        // Configuration
        CmbTheme.SelectedIndex = (int)_controller.Theme;
        foreach (var language in _controller.AvailableTranslationLanguages)
        {
            CmbTranslationLanguage.Items.Add(language);
        }
        CmbTranslationLanguage.SelectedItem = _controller.TranslationLanguage;
        TglPreviewUpdates.IsOn = _controller.AllowPreviewUpdates;
        _constructing = false;
    }

    private void SelectorBar_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        var index = sender.Items.IndexOf(sender.SelectedItem);
        ViewStack.SelectedIndex = index == -1 ? 0 : index;
    }

    private async void Cmb_SelectionChanged(object sender, SelectionChangedEventArgs e) => await ApplyChangesAsync();

    private async void Tgl_Toggled(object sender, Microsoft.UI.Xaml.RoutedEventArgs e) => await ApplyChangesAsync();

    private async Task ApplyChangesAsync()
    {
        if (_constructing)
        {
            return;
        }
        _controller.Theme = (Theme)CmbTheme.SelectedIndex;
        _controller.TranslationLanguage = (CmbTranslationLanguage.SelectedItem as string)!;
        _controller.AllowPreviewUpdates = TglPreviewUpdates.IsOn;
        await _controller.SaveConfigurationAsync();
    }
}
