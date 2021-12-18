using FluentAvalonia.UI.Controls;
using Nickvision.Avalonia.Extensions;
using Nickvision.Avalonia.Models;
using Nickvision.Avalonia.MVVM;
using Nickvision.Avalonia.MVVM.Commands;
using Nickvision.Avalonia.MVVM.Services;
using Nickvision.Avalonia.Update;
using NickvisionTubeConverter.Models;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Threading.Tasks;
using Xabe.FFmpeg;
using Xabe.FFmpeg.Enums;

namespace NickvisionTubeConverter.ViewModels
{
    public class MainWindowViewModel : ViewModelBase
    {
        private ServiceCollection _serviceCollection;
        private HttpClient _httpClient;
        private bool _opened;
        private string _status;
        private string _videoURL;
        private string _saveFolder;
        private string _fileFormat;
        private string _newFilename;
        private int _activeDownloadsCount;

        public ObservableCollection<string> FileFormats { get; init; }
        public ObservableCollection<Download> Downloads { get; init; }
        public DelegateAsyncCommand<object> OpenedCommand { get; init; }
        public DelegateAsyncCommand<object> SelectSaveFolderCommand { get; init; }
        public DelegateCommand<object> GoToSaveFolderCommand { get; init; }
        public DelegateCommand<ICloseable> ExitCommand { get; init; }
        public DelegateAsyncCommand<object> SettingsCommand { get; init; }
        public DelegateAsyncCommand<object> DownloadVideoCommand { get; init; }
        public DelegateCommand<object> ClearCompletedDownloadsCommand { get; init; }
        public DelegateCommand<object> GoBackCommand { get; init; }
        public DelegateCommand<object> GoForwardCommand { get; init; }
        public DelegateCommand<object> RefreshCommand { get; init; }
        public DelegateCommand<object> GoHomeCommand { get; init; }
        public DelegateAsyncCommand<ICloseable> CheckForUpdatesCommand { get; init; }
        public DelegateCommand<object> GitHubRepoCommand { get; init; }
        public DelegateCommand<object> ReportABugCommand { get; init; }
        public DelegateAsyncCommand<object> ChangelogCommand { get; init; }
        public DelegateAsyncCommand<object> AboutCommand { get; init; }

        public MainWindowViewModel(ServiceCollection serviceCollection)
        {
            Title = "Nickvision Tube Converter";
            _serviceCollection = serviceCollection;
            _httpClient = new HttpClient();
            _opened = false;
            _activeDownloadsCount = 0;
            FileFormats = new ObservableCollection<string>();
            Downloads = new ObservableCollection<Download>();
            OpenedCommand = new DelegateAsyncCommand<object>(Opened);
            SelectSaveFolderCommand = new DelegateAsyncCommand<object>(SelectSaveFolder);
            GoToSaveFolderCommand = new DelegateCommand<object>(GoToSaveFolder, () => !string.IsNullOrEmpty(SaveFolder));
            ExitCommand = new DelegateCommand<ICloseable>(Exit);
            SettingsCommand = new DelegateAsyncCommand<object>(Settings);
            DownloadVideoCommand = new DelegateAsyncCommand<object>(DownloadVideo, () => !string.IsNullOrEmpty(VideoURL) && (VideoURL.StartsWith("https://www.youtube.com/watch?v=") || VideoURL.StartsWith("http://www.youtube.com/watch?v=")) && !string.IsNullOrEmpty(SaveFolder) && !string.IsNullOrEmpty(FileFormat) && !string.IsNullOrEmpty(NewFilename) && FileFormat != "--------------");
            ClearCompletedDownloadsCommand = new DelegateCommand<object>(ClearCompletedDownloads, () => Downloads.Count != 0);
            GoBackCommand = new DelegateCommand<object>(GoBack);
            GoForwardCommand = new DelegateCommand<object>(GoForward);
            RefreshCommand = new DelegateCommand<object>(Refresh);
            GoHomeCommand = new DelegateCommand<object>(GoHome);
            CheckForUpdatesCommand = new DelegateAsyncCommand<ICloseable>(CheckForUpdates);
            GitHubRepoCommand = new DelegateCommand<object>(GitHubRepo);
            ReportABugCommand = new DelegateCommand<object>(ReportABug);
            ChangelogCommand = new DelegateAsyncCommand<object>(Changelog);
            AboutCommand = new DelegateAsyncCommand<object>(About);
        }

        public string Status
        {
            get => _status;

            set => SetProperty(ref _status, value);
        }

        public string VideoURL
        {
            get => _videoURL;

            set
            {
                var url = value;
                if(!url.StartsWith("https://www.youtube.com/") && !url.StartsWith("http://www.youtube.com/"))
                {
                    url = "https://www.youtube.com/";
                }
                SetProperty(ref _videoURL, url);
                DownloadVideoCommand.RaiseCanExecuteChanged();
            }
        }

        public string SaveFolder
        {
            get => _saveFolder;

            set
            {
                SetProperty(ref _saveFolder, value);
                GoToSaveFolderCommand.RaiseCanExecuteChanged();
            }
        }

        public string FileFormat
        {
            get => _fileFormat;

            set
            {
                SetProperty(ref _fileFormat, value);
                DownloadVideoCommand.RaiseCanExecuteChanged();
                var configuration = Configuration.Load();
                configuration.PreviousFileFormat = FileFormat;
                configuration.Save();
            }
        }

        public string NewFilename
        {
            get => _newFilename;

            set
            {
                SetProperty(ref _newFilename, value);
                DownloadVideoCommand.RaiseCanExecuteChanged();
            }
        }

        private async Task Opened(object parameter)
        {
            if (!_opened)
            {
                FileFormats.Add("MP4 - Video");
                FileFormats.Add("MOV - Video");
                FileFormats.Add("AVI - Video");
                FileFormats.Add("--------------");
                FileFormats.Add("MP3 - Audio");
                FileFormats.Add("WAV - Audio");
                FileFormats.Add("WMA - Audio");
                FileFormats.Add("OGG - Audio");
                FileFormats.Add("FLAC - Audio");
                _serviceCollection.GetService<IWebViewService>().HomePage = new Uri("https://www.youtube.com/");
                _serviceCollection.GetService<IWebViewService>().GoHome();
                FFmpeg.ExecutablesPath = $"{Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData)}{Path.DirectorySeparatorChar}Nickvision{Path.DirectorySeparatorChar}NickvisionTubeConverter";
                await _serviceCollection.GetService<IProgressDialogService>().ShowAsync("Downloading required dependencies...", async () =>
                {
                    try
                    {
                        await FFmpeg.GetLatestVersion(FFmpegVersion.Official);
                    }
                    catch
                    {
                        _serviceCollection.GetService<IInfoBarService>().ShowCloseableNotification("Download Failed", "Unable To download required dependencies. Please make sure you are connect to the internet and restart the application to try again.", InfoBarSeverity.Error);
                    }
                });
                var configuration = await Configuration.LoadAsync();
                try
                {
                    _serviceCollection.GetService<IThemeService>().ForceNativeTitleBarTheme();
                }
                catch { }
                _serviceCollection.GetService<IThemeService>().ChangeTheme(configuration.Theme);
                _serviceCollection.GetService<IThemeService>().ChangeAccentColor(configuration.AccentColor);
                if(Directory.Exists(configuration.PreviousSaveFolder))
                {
                    SaveFolder = configuration.PreviousSaveFolder;
                }
                FileFormat = configuration.PreviousFileFormat;
                Status = "Remaining Downloads: 0";
                _opened = true;
            }
        }

        private async Task SelectSaveFolder(object parameter)
        {
            var result = await _serviceCollection.GetService<IIOService>().ShowOpenFolderDialogAsync("Select Save Folder");
            if(result != null)
            {
                SaveFolder = result;
                var configuration = await Configuration.LoadAsync();
                configuration.PreviousSaveFolder = SaveFolder;
                await configuration.SaveAsync();
            }
        }

        private void GoToSaveFolder(object parameter) => Process.Start(new ProcessStartInfo(SaveFolder) { UseShellExecute = true });

        private void Exit(ICloseable window) => window.Close();

        private async Task Settings(object parameter) => await _serviceCollection.GetService<IContentDialogService>().ShowCustomAsync(new SettingsDialogViewModel(_serviceCollection));

        private async Task DownloadVideo(object parameter)
        {
            if (_activeDownloadsCount < (await Configuration.LoadAsync()).MaxNumberOfActiveDownloads)
            {
                var download = new Download(VideoURL, SaveFolder, NewFilename);
                Downloads.Add(download);
                _activeDownloadsCount++;
                Status = $"Remaining Downloads: {_activeDownloadsCount}";
                NewFilename = "";
                DownloadVideoCommand.IsExecuting = false;
                try
                {
                    if (FileFormat.Contains("MP4"))
                    {
                        await download.DownloadAsVideoAsync("mp4");
                    }
                    else if (FileFormat.Contains("MOV"))
                    {
                        await download.DownloadAsVideoAsync("mov");
                    }
                    else if (FileFormat.Contains("AVI"))
                    {
                        await download.DownloadAsVideoAsync("avi");
                    }
                    else if (FileFormat.Contains("MP3"))
                    {
                        await download.DownloadAsAudioAsync("mp3");
                    }
                    else if (FileFormat.Contains("WAV"))
                    {
                        await download.DownloadAsAudioAsync("wav");
                    }
                    else if (FileFormat.Contains("WMA"))
                    {
                        await download.DownloadAsAudioAsync("wma");
                    }
                    else if (FileFormat.Contains("OGG"))
                    {
                        await download.DownloadAsAudioAsync("ogg");
                    }
                    else if (FileFormat.Contains("FLAC"))
                    {
                        await download.DownloadAsAudioAsync("flac");
                    }
                }
                catch (Exception ex)
                {
                    await _serviceCollection.GetService<IContentDialogService>().ShowMessageAsync(new ContentDialogInfo()
                    {
                        Title = $"Error: {ex.Message}",
                        Description = ex.StackTrace,
                        CloseButtonText = "OK",
                        DefaultButton = ContentDialogButton.Close
                    });
                }
                _activeDownloadsCount--;
                Status = $"Remaining Downloads: {_activeDownloadsCount}";
                ClearCompletedDownloadsCommand.RaiseCanExecuteChanged();
            }
            else
            {
                await _serviceCollection.GetService<IInfoBarService>().ShowDisappearingNotificationAsync("Please Wait", "The max number of active downloads has been reached. Please wait for one to finish before continuing. You can change the max number of active downloads in settings.", InfoBarSeverity.Warning, 3200);
            }
        }

        private void ClearCompletedDownloads(object parameter)
        {
            Downloads.Clear();
            ClearCompletedDownloadsCommand.RaiseCanExecuteChanged();
        }

        private void GoBack(object parameter) => _serviceCollection.GetService<IWebViewService>().GoBack();

        private void GoForward(object parameter) => _serviceCollection.GetService<IWebViewService>().GoForward();

        private void Refresh(object parameter) => _serviceCollection.GetService<IWebViewService>().Refresh();

        private void GoHome(object parameter) => _serviceCollection.GetService<IWebViewService>().GoHome();

        private async Task CheckForUpdates(ICloseable window)
        {
            var updater = new Updater(_httpClient, new Uri("https://raw.githubusercontent.com/nlogozzo/NickvisionTubeConverter/main/UpdateConfig.json"), new Version("2021.12.0"));
            await _serviceCollection.GetService<IProgressDialogService>().ShowAsync("Checking for updates...", async () => await updater.CheckForUpdatesAsync());
            if (updater.UpdateAvailable)
            {
                if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                {
                    var result = await _serviceCollection.GetService<IContentDialogService>().ShowMessageAsync(new ContentDialogInfo()
                    {
                        Title = "Update Available",
                        Description = $"===V{updater.LatestVersion} Changelog===\n{updater.Changelog}\n\nNickvision Tube Converter will automatically download and install the update, please save all work before continuing. Are you ready to update?",
                        PrimaryButtonText = "Yes",
                        CloseButtonText = "No",
                        DefaultButton = ContentDialogButton.Close
                    });
                    if (result == ContentDialogResult.Primary)
                    {
                        await _serviceCollection.GetService<IProgressDialogService>().ShowAsync("Downloading and installing the update...", async () => await updater.WindowsUpdateAsync(window));
                    }
                }
                else
                {
                    await _serviceCollection.GetService<IContentDialogService>().ShowMessageAsync(new ContentDialogInfo()
                    {
                        Title = "Update Available",
                        Description = $"===V{updater.LatestVersion} Changelog===\n{updater.Changelog}\n\nPlease visit the GitHub repo to download the latest release.",
                        CloseButtonText = "OK",
                        DefaultButton = ContentDialogButton.Close
                    });
                }
            }
            else
            {
                _serviceCollection.GetService<IInfoBarService>().ShowCloseableNotification("No Update Available", "There is no update at this time. Please try again later.", InfoBarSeverity.Error);
            }
        }

        private void GitHubRepo(object parameter) => new Uri("https://github.com/nlogozzo/NickvisionTubeConverter").OpenInBrowser();

        private void ReportABug(object parameter) => new Uri("https://github.com/nlogozzo/NickvisionTubeConverter/issues/new").OpenInBrowser();

        private async Task Changelog(object parameter)
        {
            await _serviceCollection.GetService<IContentDialogService>().ShowMessageAsync(new ContentDialogInfo()
            {
                Title = "What's New?",
                Description  = "- Initial Release",
                CloseButtonText = "OK",
                DefaultButton = ContentDialogButton.Close
            });
        }

        private async Task About(object parameter)
        {
            await _serviceCollection.GetService<IContentDialogService>().ShowMessageAsync(new ContentDialogInfo()
            {
                Title = "About",
                Description  = "Nickvision Tube Converter Version 2021.12.0\nAn easy-to-use YouTube video downloader.\n\nUsing Avalonia and .NET 6",
                CloseButtonText = "OK",
                DefaultButton = ContentDialogButton.Close
            });
        }
    }
}
