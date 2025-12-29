using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class AddDownloadDialogController
{
    private static readonly Dictionary<AddDownloadTeachType, bool> ShowTeachMap;

    private readonly IJsonFileService _jsonFileService;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly Dictionary<int, Credential> _credentialMap;
    private readonly Dictionary<int, string> _titlesMap;
    private readonly Dictionary<int, IReadOnlyCollection<Media>> _mediaMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<double>>> _audioBitratesMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<Format>>> _audioFormatsMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<MediaFileType>>> _fileTypesMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<SubtitleLanguage>>> _subtitleLanguagesMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<Format>>> _videoFormatsMap;
    private readonly Dictionary<int, IReadOnlyCollection<SelectionItem<VideoResolution>>> _videoResolutionsMap;

    public ITranslationService Translator { get; }
    public PreviousDownloadOptions PreviousDownloadOptions { get; }
    public IReadOnlyCollection<SelectionItem<Credential?>> AvailableCredentials { get; }
    public IReadOnlyCollection<SelectionItem<PostProcessorArgument?>> AvailablePostProcessorArguments { get; }

    static AddDownloadDialogController()
    {
        ShowTeachMap = new Dictionary<AddDownloadTeachType, bool>();
    }

    public AddDownloadDialogController(IJsonFileService jsonFileService, ITranslationService translationService, IKeyringService keyringService, INotificationService notificationService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _credentialMap = new Dictionary<int, Credential>();
        _titlesMap = new Dictionary<int, string>();
        _mediaMap = new Dictionary<int, IReadOnlyCollection<Media>>();
        _audioBitratesMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<double>>>();
        _audioFormatsMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<Format>>>();
        _fileTypesMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<MediaFileType>>>();
        _subtitleLanguagesMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<SubtitleLanguage>>>();
        _videoFormatsMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<Format>>>();
        _videoResolutionsMap = new Dictionary<int, IReadOnlyCollection<SelectionItem<VideoResolution>>>();
        Translator = translationService;
        PreviousDownloadOptions = _jsonFileService.Load<PreviousDownloadOptions>(PreviousDownloadOptions.Key);
        var availableCredentials = new List<SelectionItem<Credential?>>(_keyringService.Credentials.Count() + 1)
        {
            new SelectionItem<Credential?>(null, Translator._("Use manual credential"), true)
        };
        foreach (var credential in _keyringService.Credentials)
        {
            availableCredentials.Add(new SelectionItem<Credential?>(credential, credential.Name, false));
        }
        AvailableCredentials = availableCredentials;
        var postprocessingArguments = _jsonFileService.Load<Configuration>(Configuration.Key).PostprocessingArguments;
        var availablePostProcessorArguments = new List<SelectionItem<PostProcessorArgument?>>(postprocessingArguments.Count + 1);
        foreach (var argument in postprocessingArguments)
        {
            availablePostProcessorArguments.Add(new SelectionItem<PostProcessorArgument?>(argument, argument.Name, PreviousDownloadOptions.PostProcessorArgumentName == argument.Name));
        }
        availablePostProcessorArguments.Insert(0, new SelectionItem<PostProcessorArgument?>(null, Translator._("None"), !availablePostProcessorArguments.Any(x => x.ShouldSelect)));
        AvailablePostProcessorArguments = availablePostProcessorArguments;
    }

    public async Task AddPlaylistDownloadsAsync(int discoveryId, IReadOnlyCollection<MediaSelectionItem> items, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<VideoResolution> selectedVideoResoltuion, SelectionItem<double> selectedAudioBitrate, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool exportM3U, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument)
    {
        if (!_titlesMap.TryGetValue(discoveryId, out var title) || !_mediaMap.TryGetValue(discoveryId, out var list))
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding playlist downloads"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = "Unable to find the discovered media."
            });
            return;
        }
        var downloader = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var m3uFile = new M3UFile(title, list.Any(x => !string.IsNullOrEmpty(x.SuggestedSaveFolder)) ? PathType.Absolute : PathType.Relative);
        var options = new List<DownloadOptions>(items.Count);
        foreach (var item in items)
        {
            if (item.Value < 0 || item.Value >= list.Count)
            {
                continue;
            }
            var media = list.ElementAt(item.Value);
            options.Add(new DownloadOptions(media.Url)
            {
                Credential = _credentialMap.TryGetValue(discoveryId, out var credential) ? credential : null,
                SaveFilename = string.IsNullOrEmpty(item.Filename) ? media.Title : item.Filename.SanitizeForFilename(downloader.LimitCharacters),
                SaveFolder = Path.Combine(!string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : saveFolder, title.SanitizeForFilename(downloader.LimitCharacters)),
                FileType = selectedFileType.Value.IsVideo && media.Type == MediaType.Audio ? PreviousDownloadOptions.AudioOnlyFileType : selectedFileType.Value,
                PlaylistPosition = media.PlaylistPosition,
                VideoResolution = selectedVideoResoltuion.Value,
                AudioBitrate = selectedAudioBitrate.Value,
                SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).Where(x => media.Subtitles.Contains(x)),
                SplitChapters = splitChapters,
                ExportDescription = exportDescription,
                PostProcessorArgument = selectedPostProcessorArgument.Value,
                TimeFrame = TimeFrame.TryParse(item.StartTime, item.EndTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
            });
            m3uFile.Add(options);
        }
        PreviousDownloadOptions.SaveFolder = saveFolder;
        if (list.Any(m => m.Type == MediaType.Video))
        {
            PreviousDownloadOptions.FullFileType = selectedFileType.Value;
        }
        else
        {
            PreviousDownloadOptions.AudioOnlyFileType = selectedFileType.Value;
        }
        PreviousDownloadOptions.VideoResolution = selectedVideoResoltuion.Value;
        PreviousDownloadOptions.AudioBitrate = selectedAudioBitrate.Value;
        PreviousDownloadOptions.ExportM3U = exportM3U;
        PreviousDownloadOptions.SplitChapters = splitChapters;
        PreviousDownloadOptions.ExportDescription = exportDescription;
        if (selectedPostProcessorArgument.Value is not null)
        {
            PreviousDownloadOptions.PostProcessorArgumentName = selectedPostProcessorArgument.Value.Name;
        }
        PreviousDownloadOptions.SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value);
        await _jsonFileService.SaveAsync(PreviousDownloadOptions, PreviousDownloadOptions.Key);
        try
        {
            await _downloadService.AddAsync(options, excludeFromHistory);
            if (exportM3U)
            {
                await m3uFile.WriteAsync(Path.Combine(saveFolder, title.SanitizeForFilename(downloader.LimitCharacters), $"{title.SanitizeForFilename(downloader.LimitCharacters)}.m3u"));
            }
        }
        catch (Exception e)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding playlist downloads"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
        }
        _credentialMap.Remove(discoveryId);
        _titlesMap.Remove(discoveryId);
        _mediaMap.Remove(discoveryId);
        _audioBitratesMap.Remove(discoveryId);
        _fileTypesMap.Remove(discoveryId);
        _subtitleLanguagesMap.Remove(discoveryId);
        _videoResolutionsMap.Remove(discoveryId);
    }

    public async Task AddSingleDownloadAsync(int discoveryId, string saveFilename, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<Format> selectedVideoFormat, SelectionItem<Format> selectedAudioFormat, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument, string startTime, string endTime)
    {
        if (!_mediaMap.TryGetValue(discoveryId, out var list))
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding the single download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = "Unable to find the discovered media."
            });
            return;
        }
        var downloader = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var media = list.ElementAt(0);
        var options = new DownloadOptions(media.Url)
        {
            Credential = _credentialMap.TryGetValue(discoveryId, out var credential) ? credential : null,
            SaveFilename = string.IsNullOrEmpty(saveFilename) ? media.Title : saveFilename.SanitizeForFilename(downloader.LimitCharacters),
            SaveFolder = saveFolder,
            FileType = selectedFileType.Value,
            PlaylistPosition = media.PlaylistPosition,
            VideoFormat = selectedVideoFormat.Value,
            AudioFormat = selectedAudioFormat.Value,
            SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value),
            SplitChapters = splitChapters,
            ExportDescription = exportDescription,
            PostProcessorArgument = selectedPostProcessorArgument.Value,
            TimeFrame = TimeFrame.TryParse(startTime, endTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
        };
        PreviousDownloadOptions.SaveFolder = options.SaveFolder;
        if (media.Type == MediaType.Video)
        {
            PreviousDownloadOptions.FullFileType = options.FileType;
        }
        else
        {
            PreviousDownloadOptions.AudioOnlyFileType = options.FileType;
        }
        if (media.Formats.HasFormats(MediaType.Video))
        {
            PreviousDownloadOptions.VideoFormatIds[options.FileType] = options.VideoFormat?.Id ?? PreviousDownloadOptions.VideoFormatIds[options.FileType];
        }
        if (media.Formats.HasFormats(MediaType.Audio))
        {
            PreviousDownloadOptions.AudioFormatIds[options.FileType] = options.AudioFormat?.Id ?? PreviousDownloadOptions.AudioFormatIds[options.FileType];
        }
        PreviousDownloadOptions.SplitChapters = options.SplitChapters;
        PreviousDownloadOptions.ExportDescription = options.ExportDescription;
        PreviousDownloadOptions.PostProcessorArgumentName = options.PostProcessorArgument?.Name ?? PreviousDownloadOptions.PostProcessorArgumentName;
        PreviousDownloadOptions.SubtitleLanguages = options.SubtitleLanguages;
        await _jsonFileService.SaveAsync(PreviousDownloadOptions, PreviousDownloadOptions.Key);
        try
        {
            await _downloadService.AddAsync(options, excludeFromHistory);
        }
        catch (Exception e)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding the single download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
        }
        _credentialMap.Remove(discoveryId);
        _titlesMap.Remove(discoveryId);
        _mediaMap.Remove(discoveryId);
        _audioFormatsMap.Remove(discoveryId);
        _fileTypesMap.Remove(discoveryId);
        _subtitleLanguagesMap.Remove(discoveryId);
        _videoFormatsMap.Remove(discoveryId);
    }

    public async Task<(int Id, IReadOnlyList<MediaSelectionItem>)> DiscoverAsync(Uri url, Credential? credential, CancellationToken cancellationToken)
    {
        try
        {
            var res = url.ToString().StartsWith("file://") ? await _discoveryService.GetForBatchFileAsync(url.ToString().Substring(8), credential, cancellationToken) : await _discoveryService.GetForUrlAsync(url, credential, cancellationToken);
            if (res.Media.Count == 0)
            {
                _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Warning)
                {
                    Action = "error",
                    ActionParam = Translator._("No media found at the provided URL.")
                });
                return (-1, []);
            }
            if (credential is not null)
            {
                _credentialMap[res.Id] = credential;
            }
            _titlesMap[res.Id] = res.Title;
            _mediaMap[res.Id] = res.Media;
            if (!res.IsPlaylist)
            {
                var audioFormats = new List<SelectionItem<Format>>();
                var videoFormats = new List<SelectionItem<Format>>();
                foreach (var format in res.Media[0].Formats)
                {
                    var formatSelectionItem = new SelectionItem<Format>(format, format.ToString(Translator), false);
                    if (format.Type == MediaType.Audio)
                    {
                        audioFormats.Add(formatSelectionItem);
                    }
                    else if (format.Type == MediaType.Video)
                    {
                        videoFormats.Add(formatSelectionItem);
                    }
                }
                _audioFormatsMap[res.Id] = audioFormats;
                _videoFormatsMap[res.Id] = videoFormats;
            }
            else
            {
                var audioBitrates = new List<SelectionItem<double>>();
                foreach (var bitrate in res.Media.SelectMany(m => m.Formats).Where(f => f.Bitrate.HasValue).Select(f => f.Bitrate!.Value).Distinct())
                {
                    audioBitrates.Add(new SelectionItem<double>(bitrate, $"{bitrate}k", PreviousDownloadOptions.AudioBitrate == bitrate));
                }
                audioBitrates.Sort((a, b) => a.Value.CompareTo(b.Value));
                audioBitrates.Insert(0, new SelectionItem<double>(-1.0, Translator._("Worst"), PreviousDownloadOptions.AudioBitrate == -1.0));
                audioBitrates.Insert(0, new SelectionItem<double>(double.MaxValue, Translator._("Best"), PreviousDownloadOptions.AudioBitrate == double.MaxValue));
                var videoResolutions = new List<SelectionItem<VideoResolution>>();
                foreach (var resolution in res.Media.SelectMany(m => m.Formats).Where(f => f.VideoResolution is not null).Select(f => f.VideoResolution!).Distinct())
                {
                    videoResolutions.Add(new SelectionItem<VideoResolution>(resolution, resolution.ToString(Translator), PreviousDownloadOptions.VideoResolution == resolution));
                }
                videoResolutions.Sort((a, b) => a.Value.CompareTo(b.Value));
                videoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Worst, VideoResolution.Worst.ToString(Translator), PreviousDownloadOptions.VideoResolution == VideoResolution.Worst));
                videoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Best, VideoResolution.Best.ToString(Translator), PreviousDownloadOptions.VideoResolution == VideoResolution.Best));
                var playlistItems = new List<SelectionItem<(int Index, string Filename, string StartTime, string EndTime)>>(res.Media.Count);
                _audioBitratesMap[res.Id] = audioBitrates;
                _videoResolutionsMap[res.Id] = videoResolutions;
            }
            var fileTypes = new List<SelectionItem<MediaFileType>>(res.Media.Any(m => m.Type == MediaType.Video) ? 13 : 7);
            var previousFileType = fileTypes.Capacity == 13 ? PreviousDownloadOptions.FullFileType : PreviousDownloadOptions.AudioOnlyFileType;
            if (fileTypes.Capacity == 13)
            {
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Video, Translator._("Video (Generic)"), previousFileType == MediaFileType.Video));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP4, Translator._("MP4 (Video)"), previousFileType == MediaFileType.MP4));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WEBM, Translator._("WEBM (Video)"), previousFileType == MediaFileType.WEBM));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MKV, Translator._("MKV (Video)"), previousFileType == MediaFileType.MKV));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MOV, Translator._("MOV (Video)"), previousFileType == MediaFileType.MOV));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.AVI, Translator._("AVI (Video)"), previousFileType == MediaFileType.AVI));
            }
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Audio, Translator._("Audio (Generic)"), previousFileType == MediaFileType.Audio));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP3, Translator._("MP3 (Audio)"), previousFileType == MediaFileType.MP3));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.M4A, Translator._("M4A (Audio)"), previousFileType == MediaFileType.M4A));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OPUS, Translator._("OPUS (Audio)"), previousFileType == MediaFileType.OPUS));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.FLAC, Translator._("FLAC (Audio)"), previousFileType == MediaFileType.FLAC));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WAV, Translator._("WAV (Audio)"), previousFileType == MediaFileType.WAV));
            fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OGG, Translator._("OGG (Audio)"), previousFileType == MediaFileType.OGG));
            var subtitleLanguages = new List<SelectionItem<SubtitleLanguage>>();
            foreach (var subtitle in res.Media.SelectMany(m => m.Subtitles).Distinct())
            {
                subtitleLanguages.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(Translator), PreviousDownloadOptions.SubtitleLanguages.Contains(subtitle)));
            }
            subtitleLanguages.Sort((a, b) => a.Value.CompareTo(b.Value));
            _fileTypesMap[res.Id] = fileTypes;
            _subtitleLanguagesMap[res.Id] = subtitleLanguages;
            var items = new List<MediaSelectionItem>(res.Media.Count);
            for (var i = 0; i < res.Media.Count; i++)
            {
                var media = res.Media[i];
                items.Add(new MediaSelectionItem(i, media.Title, media.TimeFrame.StartString, media.TimeFrame.EndString));
            }
            return (res.Id, items);
        }
        catch (TaskCanceledException)
        {
            return (-1, []);
        }
        catch (Exception e)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
            return (-1, []);
        }
    }

    public IReadOnlyCollection<SelectionItem<double>> GetAvailableAudioBitrates(int discoveryId)
    {
        if (!_audioBitratesMap.TryGetValue(discoveryId, out var bitrates))
        {
            return [];
        }
        return bitrates;
    }

    public IReadOnlyCollection<SelectionItem<Format>> GetAvailableAudioFormats(int discoveryId, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_audioFormatsMap.TryGetValue(discoveryId, out var formats))
        {
            return [];
        }
        var previousId = PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value];
        foreach (var format in formats)
        {
            format.ShouldSelect = format.Value.Id == previousId;
        }
        return formats;
    }

    public IReadOnlyCollection<SelectionItem<MediaFileType>> GetAvailableFileTypes(int discoveryId)
    {
        if (!_fileTypesMap.TryGetValue(discoveryId, out var fileTypes))
        {
            return [];
        }
        return fileTypes;
    }

    public IReadOnlyCollection<SelectionItem<SubtitleLanguage>> GetAvailableSubtitleLanguages(int discoveryId)
    {
        if (!_subtitleLanguagesMap.TryGetValue(discoveryId, out var languages))
        {
            return [];
        }
        return languages;
    }

    public IReadOnlyCollection<SelectionItem<Format>> GetAvailableVideoFormats(int discoveryId, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_videoFormatsMap.TryGetValue(discoveryId, out var formats))
        {
            return [];
        }
        var previousId = PreviousDownloadOptions.VideoFormatIds[selectedFileType.Value];
        foreach (var format in formats)
        {
            format.ShouldSelect = format.Value.Id == previousId;
        }
        return formats;
    }

    public IReadOnlyCollection<SelectionItem<VideoResolution>> GetAvailableVideoResolutions(int discoveryId)
    {
        if (!_videoResolutionsMap.TryGetValue(discoveryId, out var resolutions))
        {
            return [];
        }
        return resolutions;
    }

    public string GetMediaTitle(int discoveryId, int mediaIndex)
    {
        if (!_mediaMap.TryGetValue(discoveryId, out var media) || mediaIndex < 0 || mediaIndex > media.Count)
        {
            return string.Empty;
        }
        return media.ElementAt(mediaIndex).Title;
    }

    public bool GetShouldShowDownloadImmediatelyTeach()
    {
        if (!PreviousDownloadOptions.DownloadImmediately && !ShowTeachMap.TryGetValue(AddDownloadTeachType.DownloadImmediately, out var _))
        {
            ShowTeachMap[AddDownloadTeachType.DownloadImmediately] = false;
            return true;
        }
        return false;
    }

    public bool GetShouldShowFileTypeTeach(int discoveryId, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_mediaMap.TryGetValue(discoveryId, out var media))
        {
            return false;
        }
        var previousFileType = media.Any(m => m.Type == MediaType.Video) ? PreviousDownloadOptions.FullFileType : PreviousDownloadOptions.AudioOnlyFileType;
        if (!previousFileType.IsGeneric && selectedFileType.Value.IsGeneric && !ShowTeachMap.TryGetValue(AddDownloadTeachType.FileType, out var _))
        {
            ShowTeachMap[AddDownloadTeachType.FileType] = false;
            return true;
        }
        return false;
    }
}
