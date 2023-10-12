using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Nickvision.Aura;
using Python.Runtime;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A dialog for viewing information about an app
/// </summary>
public sealed partial class AboutDialog : ContentDialog
{
    private AppInfo _appInfo;

    /// <summary>
    /// Constructs an AboutDialog
    /// </summary>
    public AboutDialog(AppInfo appInfo)
    {
        InitializeComponent();
        _appInfo = appInfo;
        //Localize Strings
        Title = _appInfo.ShortName;
        CloseButtonText = _("OK");
        LblChangelogTitle.Text = _("Changelog");
        LblCreditsTitle.Text = _("Credits");
        InfoBar.Message = _("Copied debug info to clipboard.");
        //Load AppInfo
        LblDescription.Text = _appInfo.Description;
        LblVersion.Text = _appInfo.Version;
        LblChangelog.Text = _appInfo.Changelog;
        LblCredits.Text = _("Developers:\n{0}\n\nDesigners:\n{1}\n\nArtists:\n{2}\n\nTranslators:\n{3}", string.Join("\n", _appInfo.Developers.Keys), string.Join("\n", _appInfo.Designers.Keys), string.Join("\n", _appInfo.Artists.Keys), string.Join("\n", _appInfo.TranslatorNames.Where(x => x != "translator-credits")));
    }

    /// <summary>
    /// Occurs when the ScrollViewer's size is changed
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">SizeChangedEventArgs</param>
    private void ScrollViewer_SizeChanged(object sender, SizeChangedEventArgs e) => StackPanel.Margin = new Thickness(0, 0, ScrollViewer.ComputedVerticalScrollBarVisibility == Visibility.Visible ? 14 : 0, 0);

    /// <summary>
    /// Occurs when the version button is clicked
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">RoutedEventArgs</param>
    private async void CopySystemInformation(object sender, RoutedEventArgs e)
    {
        var info = $"{_appInfo.ID}\n{_appInfo.Version}\n\n{System.Environment.OSVersion}\n{CultureInfo.CurrentCulture.Name}";
        var py = Task.Run(() =>
        {
            using (Py.GIL())
            {
                try
                {
                    dynamic yt_dlp = Py.Import("yt_dlp");
                    info += $"\nyt-dlp {yt_dlp.version.__version__.As<string>()}";
                }
                catch
                {
                    info += "\nyt-dlp not found";
                }
                try
                {
                    dynamic psutil = Py.Import("psutil");
                    info += $"\npsutil {psutil.__version__.As<string>()}";
                }
                catch
                {
                    info += "\npsutil not found";
                }
            }
        });
        var ffmpeg = Task.Run(() =>
        {
            using var ffmpegProcess = new Process()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = DependencyLocator.Find("ffmpeg"),
                    Arguments = "-version",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true
                }
            };
            try
            {
                ffmpegProcess.Start();
                var ffmpegVersion = ffmpegProcess.StandardOutput.ReadToEnd();
                ffmpegProcess.WaitForExit();
                ffmpegVersion = ffmpegVersion.Remove(ffmpegVersion.IndexOf("\n"))
                                             .Remove(ffmpegVersion.IndexOf("Copyright"))
                                             .Trim();
                info += $"\n{ffmpegVersion}";
            }
            catch
            {
                info += "\nffmpeg not found";
            }
        });
        var aria = Task.Run(() =>
        {
            using var ariaProcess = new Process()
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = DependencyLocator.Find("aria2c"),
                    Arguments = "--version",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true
                }
            };
            try
            {
                ariaProcess.Start();
                var ariaVersion = ariaProcess.StandardOutput.ReadToEnd();
                ariaProcess.WaitForExit();
                ariaVersion = ariaVersion.Remove(ariaVersion.IndexOf("\n")).Trim();
                info += $"\n{ariaVersion}";
            }
            catch
            {
                info += "\naria2c not found";
            }
        });
        await py;
        await ffmpeg;
        await aria;
        var dataPackage = new DataPackage();
        dataPackage.SetText(info);
        Clipboard.SetContent(dataPackage);
        InfoBar.IsOpen = true;
    }
}
