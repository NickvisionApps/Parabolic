using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
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
    private readonly Dictionary<(int DiscoveryId, int MediaIndex), Media> _mediaMap;
    private readonly Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<Format>>> _audioFormatsMap;
    private readonly Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<MediaFileType>>> _fileTypesMap;
    private readonly Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<SubtitleLanguage>>> _subtitleLanguagesMap;
    private readonly Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<Format>>> _videoFormatsMap;

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
        _mediaMap = new Dictionary<(int DiscoveryId, int MediaIndex), Media>();
        _audioFormatsMap = new Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<Format>>>();
        _fileTypesMap = new Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<MediaFileType>>>();
        _subtitleLanguagesMap = new Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<SubtitleLanguage>>>();
        _videoFormatsMap = new Dictionary<(int DiscoveryId, int MediaIndex), IReadOnlyCollection<SelectionItem<Format>>>();
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

    public async Task AddSingleDownloadAsync(int discoveryId, int mediaIndex, string saveFilename, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<Format> selectedVideoFormat, SelectionItem<Format> selectedAudioFormat, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument, string startTime, string endTime)
    {
        var key = (discoveryId, mediaIndex);
        if (!_mediaMap.TryGetValue(key, out var media))
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding the download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = "Unable to find the discovered media."
            });
            return;
        }
        var options = new DownloadOptions(media.Url)
        {
            Credential = _credentialMap.ContainsKey(discoveryId) ? _credentialMap[discoveryId] : null,
            SaveFilename = saveFilename,
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
        PreviousDownloadOptions.FileType = options.FileType;
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
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding the download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
        }
        _credentialMap.Remove(discoveryId);
        _mediaMap.Remove(key);
        _audioFormatsMap.Remove(key);
        _fileTypesMap.Remove(key);
        _subtitleLanguagesMap.Remove(key);
        _videoFormatsMap.Remove(key);
    }

    public async Task<DiscoveryResult?> DiscoverAsync(Uri url, Credential? credential, CancellationToken cancellationToken)
    {
        try
        {
            var res = url.ToString().StartsWith("file://") ? await _discoveryService.GetForBatchFileAsync(url.ToString().Substring(7), credential, cancellationToken) : await _discoveryService.GetForUrlAsync(url, credential, cancellationToken);
            if (res.Media.Count == 0)
            {
                _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Warning)
                {
                    Action = "error",
                    ActionParam = Translator._("No media found at the provided URL.")
                });
                return null;
            }
            if (credential is not null)
            {
                _credentialMap[res.Id] = credential;
            }
            var mediaIndex = 0;
            foreach(var media in res.Media)
            {
                var key = (res.Id, mediaIndex);
                _mediaMap[key] = media;
                var audioFormats = new List<SelectionItem<Format>>();
                var videoFormats = new List<SelectionItem<Format>>();
                foreach(var format in media.Formats)
                {
                    var formatSelectionItem = new SelectionItem<Format>(format, format.ToString(Translator), false);
                    if (format.Type == MediaType.Audio)
                    {
                        audioFormats.Add(formatSelectionItem);
                    }
                    else if(format.Type == MediaType.Video)
                    {
                        videoFormats.Add(formatSelectionItem);
                    }
                }
                var fileTypes = new List<SelectionItem<MediaFileType>>(media.Type == MediaType.Video ? 13 : 7);
                if(media.Type == MediaType.Video)
                {
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Video, Translator._("Video (Generic)"), PreviousDownloadOptions.FileType == MediaFileType.Video));
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP4, Translator._("MP4 (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MP4));
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WEBM, Translator._("WEBM (Video)"), PreviousDownloadOptions.FileType == MediaFileType.WEBM));
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MKV, Translator._("MKV (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MKV));
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MOV, Translator._("MOV (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MOV));
                    fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.AVI, Translator._("AVI (Video)"), PreviousDownloadOptions.FileType == MediaFileType.AVI));
                }
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Audio, Translator._("Audio (Generic)"), PreviousDownloadOptions.FileType == MediaFileType.Audio));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP3, Translator._("MP3 (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.MP3));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.M4A, Translator._("M4A (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.M4A));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OPUS, Translator._("OPUS (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.OPUS));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.FLAC, Translator._("FLAC (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.FLAC));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WAV, Translator._("WAV (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.WAV));
                fileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OGG, Translator._("OGG (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.OGG));
                var subtitleLanguages = new List<SelectionItem<SubtitleLanguage>>();
                foreach (var subtitle in media.Subtitles)
                {
                    subtitleLanguages.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(Translator), PreviousDownloadOptions.SubtitleLanguages.Contains(subtitle)));
                }
                _audioFormatsMap[key] = audioFormats;
                _fileTypesMap[key] = fileTypes;
                _subtitleLanguagesMap[key] = subtitleLanguages;
                _videoFormatsMap[key] = videoFormats;
                mediaIndex++;
            }
            return res;
        }
        catch (TaskCanceledException)
        {
            return null;
        }
        catch (Exception e)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
            return null;
        }
    }

    public IReadOnlyCollection<SelectionItem<Format>> GetAvailableAudioFormats(int discoveryId, int mediaIndex, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_audioFormatsMap.TryGetValue((discoveryId, mediaIndex), out var formats))
        {
            return [];
        }
        var previousId = PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value];
        foreach(var format in formats)
        {
            format.ShouldSelect = format.Value.Id == previousId;
        }
        return formats;
    }

    public IReadOnlyCollection<SelectionItem<MediaFileType>> GetAvailableFileTypes(int discoveryId, int mediaIndex)
    {
        if (!_fileTypesMap.TryGetValue((discoveryId, mediaIndex), out var fileTypes))
        {
            return [];
        }
        return fileTypes;
    }

    public IReadOnlyCollection<SelectionItem<SubtitleLanguage>> GetAvailableSubtitleLanguages(int discoveryId, int mediaIndex)
    {
        if (!_subtitleLanguagesMap.TryGetValue((discoveryId, mediaIndex), out var languages))
        {
            return [];
        }
        return languages;
    }

    public IReadOnlyCollection<SelectionItem<Format>> GetAvailableVideoFormats(int discoveryId, int mediaIndex, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_videoFormatsMap.TryGetValue((discoveryId, mediaIndex), out var formats))
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

    public string GetMediaTitle(int discoveryId, int mediaIndex)
    {
        if (!_mediaMap.TryGetValue((discoveryId, mediaIndex), out var media))
        {
            return string.Empty;
        }
        return media.Title;
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

    public bool GetShouldShowFileTypeTeach(int discoveryId, int mediaIndex, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!PreviousDownloadOptions.FileType.IsGeneric && selectedFileType.Value.IsGeneric && !ShowTeachMap.TryGetValue(AddDownloadTeachType.FileType, out var _))
        {
            ShowTeachMap[AddDownloadTeachType.FileType] = false;
            return true;
        }
        return false;
    }
}
