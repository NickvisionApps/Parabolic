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
    private readonly IJsonFileService _jsonFileService;
    private readonly ITranslationService _translator;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly Dictionary<int, DiscoveryResult> _discoveryResults;
    private readonly Dictionary<int, Credential> _credentials;
    private readonly Dictionary<int, Dictionary<int, int>> _audioFormatMaps;
    private readonly Dictionary<int, Dictionary<int, int>> _videoFormatMaps;

    public PreviousDownloadOptions PreviousDownloadOptions { get; }

    public AddDownloadDialogController(IJsonFileService jsonFileService, ITranslationService translationService, IKeyringService keyringService, INotificationService notificationService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _translator = translationService;
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _discoveryResults = new Dictionary<int, DiscoveryResult>();
        _credentials = new Dictionary<int, Credential>();
        _audioFormatMaps = new Dictionary<int, Dictionary<int, int>>();
        _videoFormatMaps = new Dictionary<int, Dictionary<int, int>>();
        PreviousDownloadOptions = _jsonFileService.Load<PreviousDownloadOptions>(PreviousDownloadOptions.Key);
    }

    public IEnumerable<string> CredentialNames
    {
        get
        {
            var names = new List<string>(_keyringService.Credentials.Count() + 1);
            foreach (var credential in _keyringService.Credentials)
            {
                names.Add(credential.Name);
            }
            names.Sort();
            names.Insert(0, _translator._("Use manual credential"));
            return names;
        }
    }

    public IEnumerable<string> PostProcessorArgumentNames
    {
        get
        {
            var postProcessorArguments = _jsonFileService.Load<Configuration>(Configuration.Key).PostprocessingArguments;
            var names = new List<string>(postProcessorArguments.Count + 1);
            foreach (var argument in postProcessorArguments)
            {
                names.Add(argument.Name);
            }
            names.Sort();
            names.Insert(0, _translator._("None"));
            return names;
        }
    }

    public ITranslationService Translator => _translator;

    public async Task AddSingleDownloadAsync(int discoverId, int mediaId, string saveFilename, string saveFolder, int fileTypeIndex, int videoFormatIndex, int audioFormatIndex, IEnumerable<string> subtitleLanguages, bool splitChapters, bool exportDescription, string postProcessorArgumentName, string startTime, string endTime)
    {

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
            _discoveryResults[res.Id] = res;
            if (credential is not null)
            {
                _credentials[res.Id] = credential;
            }
            _audioFormatMaps[res.Id] = new Dictionary<int, int>();
            _videoFormatMaps[res.Id] = new Dictionary<int, int>();
            return _discoveryResults[res.Id];
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

    public IEnumerable<string> GetDiscoveredMediaAudioFormatStrings(int discoveryId, int mediaIndex, int fileTypeIndex, out int previousIndex)
    {
        previousIndex = 0;
        _audioFormatMaps[discoveryId].Clear();
        if (!_discoveryResults.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var strings = new List<string>();
        var previousId = PreviousDownloadOptions.AudioFormatIds[GetDiscoveredMediaFileTypeByIndex(discoveryId, mediaIndex, fileTypeIndex)];
        for (int i = 0; i < result.Media[mediaIndex].Formats.Count; i++)
        {
            var format = result.Media[mediaIndex].Formats[i];
            if (format.Type != MediaType.Audio)
            {
                continue;
            }
            _audioFormatMaps[discoveryId][strings.Count] = i;
            if (format.Id == previousId)
            {
                previousIndex = strings.Count;
            }
            strings.Add(format.ToString(_translator));
        }
        return strings;
    }

    public MediaFileType GetDiscoveredMediaFileTypeByIndex(int discoveryId, int mediaIndex, int fileTypeIndex)
    {
        if (!_discoveryResults.TryGetValue(discoveryId, out var result))
        {
            return MediaFileType.Video;
        }
        if (!result.IsPlaylist && result.Media[mediaIndex].Type != MediaType.Video)
        {
            fileTypeIndex += MediaFileType.VideoCount;
        }
        return (MediaFileType)fileTypeIndex;
    }

    public IEnumerable<string> GetDiscoveredMediaFileTypeStrings(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResults.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        bool includeVideoStrings = result.Media[mediaIndex].Type == MediaType.Video;
        var strings = new List<string>(includeVideoStrings ? 13 : 7);
        if (includeVideoStrings)
        {
            strings.Add(Translator._("Video (Generic)"));
            strings.Add(Translator._("MP4 (Video)"));
            strings.Add(Translator._("WEBM (Video)"));
            strings.Add(Translator._("MKV (Video)"));
            strings.Add(Translator._("MOV (Video)"));
            strings.Add(Translator._("AVI (Video)"));
        }
        strings.Add(Translator._("Audio (Generic)"));
        strings.Add(Translator._("MP3 (Audio)"));
        strings.Add(Translator._("M4A (Audio)"));
        strings.Add(Translator._("OPUS (Audio)"));
        strings.Add(Translator._("FLAC (Audio)"));
        strings.Add(Translator._("WAV (Audio)"));
        strings.Add(Translator._("OGG (Audio)"));
        return strings;
    }

    public IEnumerable<string> GetDiscoveredMediaSubtitleLanguageStrings(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResults.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var strings = new List<string>();
        foreach (var subtitle in result.Media[mediaIndex].Subtitles)
        {
            strings.Add(subtitle.ToString(_translator));
        }
        return strings;
    }

    public string GetDiscoveredMediaTitle(int discoveryId, int mediaIndex)
    {
        if (!_discoveryResults.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return string.Empty;
        }
        return result.Media[mediaIndex].Title;
    }

    public IEnumerable<string> GetDiscoveredMediaVideoFormatStrings(int discoveryId, int mediaIndex, int fileTypeIndex, out int previousIndex)
    {
        previousIndex = 0;
        _videoFormatMaps[discoveryId].Clear();
        if (!_discoveryResults.TryGetValue(discoveryId, out var result) || mediaIndex < 0 || mediaIndex >= result.Media.Count)
        {
            return [];
        }
        var strings = new List<string>();
        var previousId = PreviousDownloadOptions.VideoFormatIds[GetDiscoveredMediaFileTypeByIndex(discoveryId, mediaIndex, fileTypeIndex)];
        for (int i = 0; i < result.Media[mediaIndex].Formats.Count; i++)
        {
            var format = result.Media[mediaIndex].Formats[i];
            if (format.Type != MediaType.Video)
            {
                continue;
            }
            _videoFormatMaps[discoveryId][strings.Count] = i;
            if (format.Id == previousId)
            {
                previousIndex = strings.Count;
            }
            strings.Add(format.ToString(_translator));
        }
        return strings;
    }
}
