using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;

namespace Nickvision.Parabolic.WinUI.Controls;

public sealed partial class AboutDialog : ContentDialog
{
    private AppInfo _appInfo;
    private ITranslationService _translator;

    public AboutDialog(AppInfo appInfo, string debugInfo, ITranslationService translator)
    {
        InitializeComponent();
        _appInfo = appInfo;
        _translator = translator;
        Title = _translator._("About {0}", appInfo.ShortName!);
        CloseButtonText = _translator._("Close");
        DefaultButton = ContentDialogButton.Close;
        SelectorGeneral.Text = _translator._("General");
        SelectorChangelog.Text = _translator._("Changelog");
        SelectorDebugging.Text = _translator._("Debugging");
        SelectorCredits.Text = _translator._("Credits");
        LblAppName.Text = appInfo.ShortName;
        LblAppDescription.Text = appInfo.Description;
        LblAppVersion.Text = appInfo.Version!.ToString();
        LblAppCopyright.Text = "© Nickvision 2021-2026";
        LblChangelog.Text = appInfo.Changelog!;
        LblCopyDebugInformation.Text = _translator._("Copy Debug Information");
        LblDebugInformation.Text = debugInfo;
        if (string.IsNullOrEmpty(_appInfo.TranslationCredits) || _appInfo.TranslationCredits == "translation-credits")
        {
            LblCredits.Text = _translator._("Developers:\n{0}\n\nDesigners:\n{1}\n\nArtists:\n{2}", _appInfo.Developers.Keys.Aggregate((current, next) => $"{current}\n{next}"), _appInfo.Designers.Keys.Aggregate((current, next) => $"{current}\n{next}"), _appInfo.Artists.Keys.Aggregate((current, next) => $"{current}\n{next}"));
        }
        else
        {
            LblCredits.Text = _translator._("Developers:\n{0}\n\nDesigners:\n{1}\n\nArtists:\n{2}\n\nTranslators:\n{3}", _appInfo.Developers.Keys.Aggregate((current, next) => $"{current}\n{next}"), _appInfo.Designers.Keys.Aggregate((current, next) => $"{current}\n{next}"), _appInfo.Artists.Keys.Aggregate((current, next) => $"{current}\n{next}"), _appInfo.TranslationCredits!);
        }
    }

    private void SelectorBar_SelectionChanged(SelectorBar sender, SelectorBarSelectionChangedEventArgs args)
    {
        var index = sender.Items.IndexOf(sender.SelectedItem);
        ViewStack.SelectedIndex = index == -1 ? 0 : index;
    }

    private async void CopyDebugInformation(object? sender, RoutedEventArgs args)
    {
        var package = new DataPackage();
        package.SetText(LblDebugInformation.Text);
        Clipboard.SetContent(package);
        IconCopyDebugInformation.Glyph = "\uE73E";
        await Task.Delay(2000).ContinueWith(_ =>
        {
            DispatcherQueue.TryEnqueue(() =>
            {
                IconCopyDebugInformation.Glyph = "\uE8C8";
            });
        });
    }
}
