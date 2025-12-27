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
    private readonly Dictionary<int, DiscoveryResult> _discoveryResultMap;
    private readonly Dictionary<int, Credential> _credentialMap;

    public ITranslationService Translator { get; }
    public PreviousDownloadOptions PreviousDownloadOptions { get; }

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
        _discoveryResultMap = new Dictionary<int, DiscoveryResult>();
        _credentialMap = new Dictionary<int, Credential>();
        Translator = translationService;
        PreviousDownloadOptions = _jsonFileService.Load<PreviousDownloadOptions>(PreviousDownloadOptions.Key);
    }

    public async Task AddSingleDownloadAsync(int discoveryId, int mediaIndex, string saveFilename, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<Format> selectedVideoFormat, SelectionItem<Format> selectedAudioFormat, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument, string startTime, string endTime)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding the download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = "Unable to find the discovered media."
            });
            return;
        }
        var postProcessorArguments = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).PostprocessingArguments;
        var media = result.Media[mediaIndex];
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
            TimeFrame = TimeFrame.TryParse(startTime, endTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != result.Media[mediaIndex].TimeFrame ? timeFrame : null
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
        _discoveryResultMap.Remove(discoveryId);
        _credentialMap.Remove(discoveryId);
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
            _discoveryResultMap[res.Id] = res;
            if (credential is not null)
            {
                _credentialMap[res.Id] = credential;
            }
            return _discoveryResultMap[res.Id];
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

    public IEnumerable<SelectionItem<Format>> GetAvailableAudioFormats(int discoveryId, int mediaIndex, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem<Format>>();
        var previousId = PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value];
        foreach(var format in result.Media[mediaIndex].Formats)
        {
            if (format.Type != MediaType.Audio)
            {
                continue;
            }
            items.Add(new SelectionItem<Format>(format, format.ToString(Translator), format.Id == previousId));
        }
        return items;
    }

    public IEnumerable<SelectionItem<Credential?>> GetAvailableCredentials()
    {
        var items = new List<SelectionItem<Credential?>>(_keyringService.Credentials.Count() + 1)
        {
            new SelectionItem<Credential?>(null, Translator._("Use manual credential"), true)
        };
        foreach (var credential in _keyringService.Credentials)
        {
            items.Add(new SelectionItem<Credential?>(credential, credential.Name, false));
        }
        return items;
    }

    public IEnumerable<SelectionItem<MediaFileType>> GetAvailableFileTypes(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        bool includeVideoStrings = result.Media[mediaIndex].Type == MediaType.Video;
        var items = new List<SelectionItem<MediaFileType>>(includeVideoStrings ? 13 : 7);
        if (includeVideoStrings)
        {
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.Video, Translator._("Video (Generic)"), PreviousDownloadOptions.FileType == MediaFileType.Video));
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.MP4, Translator._("MP4 (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MP4));
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.WEBM, Translator._("WEBM (Video)"), PreviousDownloadOptions.FileType == MediaFileType.WEBM));
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.MKV, Translator._("MKV (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MKV));
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.MOV, Translator._("MOV (Video)"), PreviousDownloadOptions.FileType == MediaFileType.MOV));
            items.Add(new SelectionItem<MediaFileType>(MediaFileType.AVI, Translator._("AVI (Video)"), PreviousDownloadOptions.FileType == MediaFileType.AVI));
        }
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.Audio, Translator._("Audio (Generic)"), PreviousDownloadOptions.FileType == MediaFileType.Audio));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.MP3, Translator._("MP3 (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.MP3));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.M4A, Translator._("M4A (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.M4A));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.OPUS, Translator._("OPUS (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.OPUS));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.FLAC, Translator._("FLAC (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.FLAC));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.WAV, Translator._("WAV (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.WAV));
        items.Add(new SelectionItem<MediaFileType>(MediaFileType.OGG, Translator._("OGG (Audio)"), PreviousDownloadOptions.FileType == MediaFileType.OGG));
        return items;
    }

    public async Task<IEnumerable<SelectionItem<PostProcessorArgument?>>> GetAvailablePostProcessorArgumentsAsync()
    {
        var postProcessorArguments = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).PostprocessingArguments;
        var items = new List<SelectionItem<PostProcessorArgument?>>(postProcessorArguments.Count + 1);
        foreach (var argument in postProcessorArguments)
        {
            items.Add(new SelectionItem<PostProcessorArgument?>(argument, argument.Name, PreviousDownloadOptions.PostProcessorArgumentName == argument.Name));
        }
        items.Insert(0, new SelectionItem<PostProcessorArgument?>(null, Translator._("None"), !items.Any(x => x.ShouldSelect)));
        return items;
    }

    public IEnumerable<SelectionItem<SubtitleLanguage>> GetAvailableSubtitleLanguages(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem<SubtitleLanguage>>();
        foreach (var subtitle in result.Media[mediaIndex].Subtitles)
        {
            items.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(Translator), PreviousDownloadOptions.SubtitleLanguages.Contains(subtitle)));
        }
        return items;
    }

    public IEnumerable<SelectionItem<Format>> GetAvailableVideoFormats(int discoveryId, int mediaIndex, SelectionItem<MediaFileType> selectedFileType)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem<Format>>();
        var previousId = PreviousDownloadOptions.VideoFormatIds[selectedFileType.Value];
        foreach(var format in result.Media[mediaIndex].Formats)
        {
            if (format.Type != MediaType.Video)
            {
                continue;
            }
            items.Add(new SelectionItem<Format>(format, format.ToString(Translator), format.Id == previousId));
        }
        return items;
    }

    public string GetMediaTitle(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return string.Empty;
        }
        return result.Media[mediaIndex].Title;
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
