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
    private readonly ITranslationService _translator;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly Dictionary<int, DiscoveryResult> _discoveryResultMap;
    private readonly Dictionary<int, Credential> _credentialMap;

    public PreviousDownloadOptions PreviousDownloadOptions { get; }

    static AddDownloadDialogController()
    {
        ShowTeachMap = new Dictionary<AddDownloadTeachType, bool>();
    }

    public AddDownloadDialogController(IJsonFileService jsonFileService, ITranslationService translationService, IKeyringService keyringService, INotificationService notificationService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _translator = translationService;
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _discoveryResultMap = new Dictionary<int, DiscoveryResult>();
        _credentialMap = new Dictionary<int, Credential>();
        PreviousDownloadOptions = _jsonFileService.Load<PreviousDownloadOptions>(PreviousDownloadOptions.Key);
    }

    public ITranslationService Translator => _translator;

    public async Task AddSingleDownloadAsync(int discoveryId, int mediaIndex, string saveFilename, string saveFolder, SelectionItem selectedFileType, SelectionItem selectedVideoFormat, SelectionItem selectedAudioFormat, IEnumerable<SelectionItem> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem selectedPostProcessorArgument, string startTime, string endTime)
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
            FileType = GetMediaFileType(discoveryId, mediaIndex, selectedFileType),
            PlaylistPosition = media.PlaylistPosition,
            VideoFormat = media.Formats[selectedVideoFormat.Index],
            AudioFormat = media.Formats[selectedAudioFormat.Index],
            SubtitleLanguages = media.Subtitles.Where((_, index) => selectedSubtitleLanguages.Any(s => s.Index == index)),
            SplitChapters = splitChapters,
            ExportDescription = exportDescription,
            PostProcessorArgument = selectedPostProcessorArgument.Index == 0 ? null : postProcessorArguments[selectedPostProcessorArgument.Index - 1],
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
        PreviousDownloadOptions.SubtitleLanguageStrings = options.SubtitleLanguages.Select(s => s.ToString(_translator));
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

    public async Task<DiscoveryResult?> DiscoverAsync(Uri url, string credentialName, CancellationToken cancellationToken)
    {
        Credential? credential = null;
        if (!string.IsNullOrEmpty(credentialName))
        {
            credential = _keyringService.Credentials.FirstOrDefault(c => c.Name == credentialName);
        }
        return await DiscoverAsync(url, credential, cancellationToken);
    }

    public IEnumerable<SelectionItem> GetAvailableAudioFormats(int discoveryId, int mediaIndex, SelectionItem selectedFileType)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem>();
        var previousId = PreviousDownloadOptions.AudioFormatIds[GetMediaFileType(discoveryId, mediaIndex, selectedFileType)];
        for (int i = 0; i < result.Media[mediaIndex].Formats.Count; i++)
        {
            var format = result.Media[mediaIndex].Formats[i];
            if (format.Type != MediaType.Audio)
            {
                continue;
            }
            items.Add(new SelectionItem(i, format.ToString(_translator), format.Id == previousId));
        }
        return items;
    }

    public IEnumerable<SelectionItem> GetAvailableCredentials()
    {
        var items = new List<SelectionItem>(_keyringService.Credentials.Count() + 1)
        {
            new SelectionItem(0, _translator._("Use manual credential"), true)
        };
        foreach (var credential in _keyringService.Credentials)
        {
            items.Add(new SelectionItem(items.Count, credential.Name, false));
        }
        return items;
    }

    public IEnumerable<SelectionItem> GetAvailableFileTypes(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        bool includeVideoStrings = result.Media[mediaIndex].Type == MediaType.Video;
        var previousIndex = (int)PreviousDownloadOptions.FileType;
        var items = new List<SelectionItem>(includeVideoStrings ? 13 : 7);
        if (includeVideoStrings)
        {
            items.Add(new SelectionItem(items.Count, Translator._("Video (Generic)"), previousIndex == items.Count));
            items.Add(new SelectionItem(items.Count, Translator._("MP4 (Video)"), previousIndex == items.Count));
            items.Add(new SelectionItem(items.Count, Translator._("WEBM (Video)"), previousIndex == items.Count));
            items.Add(new SelectionItem(items.Count, Translator._("MKV (Video)"), previousIndex == items.Count));
            items.Add(new SelectionItem(items.Count, Translator._("MOV (Video)"), previousIndex == items.Count));
            items.Add(new SelectionItem(items.Count, Translator._("AVI (Video)"), previousIndex == items.Count));
        }
        items.Add(new SelectionItem(items.Count, Translator._("Audio (Generic)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("MP3 (Audio)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("M4A (Audio)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("OPUS (Audio)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("FLAC (Audio)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("WAV (Audio)"), previousIndex == items.Count));
        items.Add(new SelectionItem(items.Count, Translator._("OGG (Audio)"), previousIndex == items.Count));
        return items;
    }

    public async Task<IEnumerable<SelectionItem>> GetAvailablePostProcessorArgumentsAsync()
    {
        var postProcessorArguments = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).PostprocessingArguments;
        var items = new List<SelectionItem>(postProcessorArguments.Count + 1);
        foreach (var argument in postProcessorArguments)
        {
            items.Add(new SelectionItem(items.Count + 1, argument.Name, PreviousDownloadOptions.PostProcessorArgumentName == argument.Name));
        }
        items.Insert(0, new SelectionItem(0, _translator._("None"), !items.Any(x => x.ShouldSelect)));
        return items;
    }

    public IEnumerable<SelectionItem> GetAvailableSubtitleLanguages(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem>();
        foreach (var subtitle in result.Media[mediaIndex].Subtitles)
        {
            var label = subtitle.ToString(_translator);
            items.Add(new SelectionItem(items.Count, label, PreviousDownloadOptions.SubtitleLanguageStrings.Contains(label)));
        }
        return items;
    }

    public IEnumerable<SelectionItem> GetAvailableVideoFormats(int discoveryId, int mediaIndex, SelectionItem selectedFileType)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var items = new List<SelectionItem>();
        var previousId = PreviousDownloadOptions.VideoFormatIds[GetMediaFileType(discoveryId, mediaIndex, selectedFileType)];
        for (int i = 0; i < result.Media[mediaIndex].Formats.Count; i++)
        {
            var format = result.Media[mediaIndex].Formats[i];
            if (format.Type != MediaType.Video)
            {
                continue;
            }
            items.Add(new SelectionItem(i, format.ToString(_translator), format.Id == previousId));
        }
        return items;
    }

    public MediaFileType GetMediaFileType(int discoveryId, int mediaIndex, SelectionItem selectedFileType)
    {
        if (!_discoveryResultMap.TryGetValue(discoveryId, out var result))
        {
            return MediaFileType.Video;
        }
        var index = selectedFileType.Index;
        if (!result.IsPlaylist && result.Media[mediaIndex].Type != MediaType.Video)
        {
            index += MediaFileType.VideoCount;
        }
        return (MediaFileType)index;
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

    public bool GetShouldShowFileTypeTeach(int discoveryId, int mediaIndex, SelectionItem selectedFileType)
    {
        var type = GetMediaFileType(discoveryId, mediaIndex, selectedFileType);
        if (!PreviousDownloadOptions.FileType.IsGeneric && type.IsGeneric && !ShowTeachMap.TryGetValue(AddDownloadTeachType.FileType, out var _))
        {
            ShowTeachMap[AddDownloadTeachType.FileType] = false;
            return true;
        }
        return false;
    }
}
