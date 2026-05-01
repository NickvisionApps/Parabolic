using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
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
    private static AddDownloadTeachType _shownTeachTypeFlag;

    private readonly ILogger<AddDownloadDialogController> _logger;
    private readonly IConfigurationService _configurationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IThumbnailService _thumbnailService;
    private readonly ITranslationService _translationService;
    private readonly Dictionary<int, DiscoveryContext> _discoveryContextMap;

    public Dictionary<MediaFileType, string> PreviousAudioFormatIds => _configurationService.PreviousAudioFormatIds;
    public bool PreviousExportDescription => _configurationService.PreviousExportDescription;
    public bool PreviousExportM3U => _configurationService.PreviousExportM3U;
    public bool PreviousNumberTitles => _configurationService.PreviousNumberTitles;
    public bool PreviousReverseDownloadOrder => _configurationService.PreviousReverseDownloadOrder;
    public string PreviousSaveFolder => _configurationService.PreviousSaveFolder;
    public bool PreviousSplitChapters => _configurationService.PreviousSplitChapters;
    public Dictionary<MediaFileType, string> PreviousVideoFormatIds => _configurationService.PreviousVideoFormatIds;

    public AddDownloadDialogController(ILogger<AddDownloadDialogController> logger, IConfigurationService configurationService, IDiscoveryService discoveryService, IDownloadService downloadService, IKeyringService keyringService, INotificationService notificationService, IThumbnailService thumbnailService, ITranslationService translationService)
    {
        _logger = logger;
        _configurationService = configurationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _thumbnailService = thumbnailService;
        _translationService = translationService;
        _discoveryContextMap = new Dictionary<int, DiscoveryContext>();
    }


    public bool PreviousDownloadImmediatelyAsAudio
    {
        get => _configurationService.PreviousDownloadImmediatelyAsAudio;

        set => _configurationService.PreviousDownloadImmediatelyAsAudio = value;
    }

    public bool PreviousDownloadImmediatelyAsVideo
    {
        get => _configurationService.PreviousDownloadImmediatelyAsVideo;

        set => _configurationService.PreviousDownloadImmediatelyAsVideo = value;
    }

    public async Task AddPlaylistDownloadsAsync(DiscoveryContext context, IReadOnlyList<MediaSelectionItem> items, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<VideoResolution> selectedVideoResoltuion, SelectionItem<double> selectedAudioBitrate, bool reverseDownloadOrder, bool numberTitles, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool exportM3U, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument)
    {
        var hasSuggestedSaveFolder = false;
        var hasVideo = false;
        foreach (var media in context.Media)
        {
            if (!string.IsNullOrEmpty(media.SuggestedSaveFolder))
            {
                hasSuggestedSaveFolder = true;
            }
            if (media.Type == MediaType.Video)
            {
                hasVideo = true;
            }
            if (hasSuggestedSaveFolder && hasVideo)
            {
                break;
            }
        }
        var m3uFile = new M3UFile(context.Title, hasSuggestedSaveFolder ? PathType.Absolute : PathType.Relative);
        var selectedSubtitles = new List<SubtitleLanguage>();
        foreach (var selectedSubtitleLanguage in selectedSubtitleLanguages)
        {
            selectedSubtitles.Add(selectedSubtitleLanguage.Value);
        }
        var options = new List<DownloadOptions>(items.Count);
        var titleNumber = 1;
        for (var i = reverseDownloadOrder ? items.Count - 1 : 0; reverseDownloadOrder ? i >= 0 : i < items.Count; i += reverseDownloadOrder ? -1 : 1)
        {
            var item = items[i];
            if (item.Value < 0 || item.Value >= context.Media.Count)
            {
                return;
            }
            var media = context.Media[item.Value];
            var filteredSubtitles = new List<SubtitleLanguage>(selectedSubtitles.Count);
            foreach (var selectedSubtitle in selectedSubtitles)
            {
                if (media.Subtitles.Contains(selectedSubtitle))
                {
                    filteredSubtitles.Add(selectedSubtitle);
                }
            }
            options.Add(new DownloadOptions(media.Url)
            {
                Credential = context.Credential,
                SaveFilename = $"{(numberTitles ? $"{titleNumber++} - " : string.Empty)}{(string.IsNullOrEmpty(item.Filename) ? media.Title : item.Filename.SanitizeForFilename(_configurationService.LimitCharacters))}",
                SaveFolder = Path.Combine(!string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : saveFolder, context.Title.SanitizeForFilename(_configurationService.LimitCharacters)),
                FileType = selectedFileType.Value.IsVideo && media.Type == MediaType.Audio ? _configurationService.PreviousAudioOnlyFileType : selectedFileType.Value,
                PlaylistPosition = media.PlaylistPosition,
                RequiresPlaylistItems = media.RequiresPlaylistItems,
                VideoResolution = selectedVideoResoltuion.Value,
                AudioBitrate = selectedAudioBitrate.Value,
                SubtitleLanguages = filteredSubtitles,
                SplitChapters = splitChapters,
                ExportDescription = exportDescription,
                PostProcessorArgument = selectedPostProcessorArgument.Value,
                TimeFrame = TimeFrame.TryParse(item.StartTime, item.EndTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
            });
        }
        using var transaction = await _configurationService.CreateTransactionAsync();
        m3uFile.Add(options);
        _configurationService.PreviousSaveFolder = saveFolder;
        if (hasVideo)
        {
            _configurationService.PreviousFullFileType = selectedFileType.Value;
            if (selectedFileType.Value.IsVideo)
            {
                _configurationService.PreviousVideoOnlyFileType = selectedFileType.Value;
            }
            else if (selectedFileType.Value.IsAudio)
            {
                _configurationService.PreviousAudioOnlyFileType = selectedFileType.Value;
            }
        }
        else
        {
            _configurationService.PreviousAudioOnlyFileType = selectedFileType.Value;
        }
        _configurationService.PreviousVideoResolution = selectedVideoResoltuion.Value;
        _configurationService.PreviousAudioBitrate = selectedAudioBitrate.Value;
        _configurationService.PreviousExportM3U = exportM3U;
        _configurationService.PreviousReverseDownloadOrder = reverseDownloadOrder;
        _configurationService.PreviousNumberTitles = numberTitles;
        _configurationService.PreviousSplitChapters = splitChapters;
        _configurationService.PreviousExportDescription = exportDescription;
        if (selectedPostProcessorArgument.Value is not null)
        {
            _configurationService.PreviousPostProcessorArgumentName = selectedPostProcessorArgument.Value.Name;
        }
        _configurationService.PreviousSubtitleLanguages = selectedSubtitles;
        await transaction.CommitAsync();
        try
        {
            await _downloadService.AddAsync(options, excludeFromHistory);
            if (exportM3U)
            {
                await m3uFile.WriteAsync(Path.Combine(saveFolder, context.Title.SanitizeForFilename(_configurationService.LimitCharacters), $"{context.Title.SanitizeForFilename(_configurationService.LimitCharacters)}.m3u"));
            }
        }
        catch (Exception e)
        {
            _logger.LogError($"An error occurred while adding playlist downloads: {e}");
            _notificationService.Send(new AppNotification(_translationService._("An error occurred while adding playlist downloads"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.ToString()
            });
        }
        _discoveryContextMap.Remove(context.Id);
    }

    public async Task AddSingleDownloadAsync(DiscoveryContext context, string saveFilename, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<Format> selectedVideoFormat, SelectionItem<Format> selectedAudioFormat, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument, string startTime, string endTime)
    {
        var media = context.Media[0];
        var subtitleLanguages = new List<SubtitleLanguage>();
        foreach (var selectedSubtitleLanguage in selectedSubtitleLanguages)
        {
            subtitleLanguages.Add(selectedSubtitleLanguage.Value);
        }
        var options = new DownloadOptions(media.Url)
        {
            Credential = context.Credential,
            SaveFilename = string.IsNullOrEmpty(saveFilename) ? media.Title : saveFilename.SanitizeForFilename(_configurationService.LimitCharacters),
            SaveFolder = saveFolder,
            FileType = selectedFileType.Value,
            PlaylistPosition = media.PlaylistPosition,
            RequiresPlaylistItems = media.RequiresPlaylistItems,
            VideoFormat = selectedVideoFormat.Value,
            AudioFormat = selectedAudioFormat.Value,
            SubtitleLanguages = subtitleLanguages,
            SplitChapters = splitChapters,
            ExportDescription = exportDescription,
            PostProcessorArgument = selectedPostProcessorArgument.Value,
            TimeFrame = TimeFrame.TryParse(startTime, endTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
        };
        using var transaction = await _configurationService.CreateTransactionAsync();
        _configurationService.PreviousSaveFolder = options.SaveFolder;
        if (media.Type == MediaType.Video)
        {
            _configurationService.PreviousFullFileType = options.FileType;
            if (selectedFileType.Value.IsVideo)
            {
                _configurationService.PreviousVideoOnlyFileType = selectedFileType.Value;
            }
            else if (selectedFileType.Value.IsAudio)
            {
                _configurationService.PreviousAudioOnlyFileType = selectedFileType.Value;
            }
        }
        else
        {
            _configurationService.PreviousAudioOnlyFileType = options.FileType;
        }
        if (media.Formats.HasFormats(MediaType.Video))
        {
            var previous = _configurationService.PreviousVideoFormatIds;
            previous[options.FileType] = options.VideoFormat?.Id ?? _configurationService.PreviousVideoFormatIds[options.FileType];
            _configurationService.PreviousVideoFormatIds = previous;
        }
        if (media.Formats.HasFormats(MediaType.Audio))
        {
            var previous = _configurationService.PreviousAudioFormatIds;
            previous[options.FileType] = options.AudioFormat?.Id ?? _configurationService.PreviousAudioFormatIds[options.FileType];
            _configurationService.PreviousAudioFormatIds = previous;
        }
        _configurationService.PreviousSplitChapters = options.SplitChapters;
        _configurationService.PreviousExportDescription = options.ExportDescription;
        _configurationService.PreviousPostProcessorArgumentName = options.PostProcessorArgument?.Name ?? _configurationService.PreviousPostProcessorArgumentName;
        _configurationService.PreviousSubtitleLanguages = options.SubtitleLanguages;
        await transaction.CommitAsync();
        try
        {
            await _downloadService.AddAsync(options, excludeFromHistory);
        }
        catch (Exception e)
        {
            _logger.LogError($"An error occurred while adding the single download: {e}");
            _notificationService.Send(new AppNotification(_translationService._("An error occurred while adding the single download"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.ToString()
            });
        }
        _discoveryContextMap.Remove(context.Id);
    }

    public async Task<DiscoveryContext?> DiscoverAsync(Uri url, Credential? credential, CancellationToken cancellationToken)
    {
        try
        {
            var res = url.IsFile ? await _discoveryService.GetForBatchFileAsync(url.LocalPath, credential, cancellationToken) : await _discoveryService.GetForUrlAsync(url, credential, cancellationToken);
            if (res.Media.Count == 0)
            {
                _logger.LogError($"No media was found: {url}");
                _notificationService.Send(new AppNotification(_translationService._("No media was found at the provided URL"), NotificationSeverity.Warning)
                {
                    Action = "error"
                });
                return null;
            }
            var context = new DiscoveryContext(res.Id, res.Url, res.Title, credential, res.Media);
            if (!res.IsPlaylist)
            {
                foreach (var format in res.Media[0].Formats)
                {
                    var formatSelectionItem = new SelectionItem<Format>(format, format.ToString(_translationService), false);
                    if (format.Type == MediaType.Audio)
                    {
                        context.AudioFormats.Add(formatSelectionItem);
                    }
                    else if (format.Type == MediaType.Video)
                    {
                        context.VideoFormats.Add(formatSelectionItem);
                    }
                }
            }
            else
            {
                var matched = false;
                var bitrates = new HashSet<double>();
                var resolutions = new HashSet<VideoResolution>();
                foreach (var media in res.Media)
                {
                    foreach (var format in media.Formats)
                    {
                        if (format.Bitrate.HasValue)
                        {
                            bitrates.Add(format.Bitrate.Value);
                        }
                        if (format.VideoResolution is not null)
                        {
                            resolutions.Add(format.VideoResolution);
                        }
                    }
                }
                foreach (var bitrate in bitrates)
                {
                    var shouldSelect = _configurationService.PreviousAudioBitrate == bitrate;
                    context.AudioBitrates.Add(new SelectionItem<double>(bitrate, $"{bitrate}k", shouldSelect));
                    matched |= shouldSelect;
                }
                context.AudioBitrates.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(-1.0, _translationService._("Worst"), _configurationService.PreviousAudioBitrate == -1.0));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(double.MaxValue, _translationService._("Best"), !matched || _configurationService.PreviousAudioBitrate == double.MaxValue));
                matched = false;
                foreach (var resolution in resolutions)
                {
                    var shouldSelect = _configurationService.PreviousVideoResolution == resolution;
                    context.VideoResolutions.Add(new SelectionItem<VideoResolution>(resolution, resolution.ToString(_translationService), shouldSelect));
                    matched |= shouldSelect;
                }
                context.VideoResolutions.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Worst, VideoResolution.Worst.ToString(_translationService), _configurationService.PreviousVideoResolution == VideoResolution.Worst));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Best, VideoResolution.Best.ToString(_translationService), !matched || _configurationService.PreviousVideoResolution == VideoResolution.Best));
            }
            var hasVideo = false;
            foreach (var media in res.Media)
            {
                if (media.Type == MediaType.Video)
                {
                    hasVideo = true;
                    break;
                }
            }
            var previousFileType = (!hasVideo || _configurationService.PreviousDownloadImmediatelyAsAudio) ? _configurationService.PreviousAudioOnlyFileType : (_configurationService.PreviousDownloadImmediatelyAsVideo ? _configurationService.PreviousVideoOnlyFileType : _configurationService.PreviousFullFileType);
            context.FileTypes.EnsureCapacity(hasVideo ? 13 : 7);
            if (hasVideo)
            {
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Video, _translationService._("Video (Generic)"), previousFileType == MediaFileType.Video));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP4, _translationService._("MP4 (Video)"), previousFileType == MediaFileType.MP4));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WEBM, _translationService._("WEBM (Video)"), previousFileType == MediaFileType.WEBM));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MKV, _translationService._("MKV (Video)"), previousFileType == MediaFileType.MKV));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MOV, _translationService._("MOV (Video)"), previousFileType == MediaFileType.MOV));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.AVI, _translationService._("AVI (Video)"), previousFileType == MediaFileType.AVI));
            }
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Audio, _translationService._("Audio (Generic)"), previousFileType == MediaFileType.Audio));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP3, _translationService._("MP3 (Audio)"), previousFileType == MediaFileType.MP3));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.M4A, _translationService._("M4A (Audio)"), previousFileType == MediaFileType.M4A));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OPUS, _translationService._("OPUS (Audio)"), previousFileType == MediaFileType.OPUS));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.FLAC, _translationService._("FLAC (Audio)"), previousFileType == MediaFileType.FLAC));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WAV, _translationService._("WAV (Audio)"), previousFileType == MediaFileType.WAV));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OGG, _translationService._("OGG (Audio)"), previousFileType == MediaFileType.OGG));
            var subtitles = new HashSet<SubtitleLanguage>();
            foreach (var media in res.Media)
            {
                foreach (var subtitle in media.Subtitles)
                {
                    subtitles.Add(subtitle);
                }
            }
            foreach (var subtitle in subtitles)
            {
                context.SubtitleLanguages.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(_translationService), _configurationService.PreviousSubtitleLanguages.Contains(subtitle)));
            }
            context.SubtitleLanguages.Sort((a, b) => a.Value.CompareTo(b.Value));
            context.Items.EnsureCapacity(res.Media.Count);
            for (var i = 0; i < res.Media.Count; i++)
            {
                var media = res.Media[i];
                context.Items.Add(new MediaSelectionItem(i, media, _translationService));
            }
            _discoveryContextMap[res.Id] = context;
            return context;
        }
        catch (TaskCanceledException)
        {
            return null;
        }
        catch (Exception e)
        {
            if (e is not YtdlpException)
            {
                _logger.LogError($"An error occurred while discovering media ({url}): {e}");
            }
            _notificationService.Send(new AppNotification(_translationService._("An error occurred while discovering media"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.ToString()
            });
            return null;
        }
    }

    public async Task<IReadOnlyList<SelectionItem<Credential?>>> GetAvailableCredentialsAsync()
    {
        var credentials = await _keyringService.GetAllCredentialAsync();
        var res = new List<SelectionItem<Credential?>>(credentials.Count + 1)
        {
            new SelectionItem<Credential?>(null, _translationService._("Use manual credential"), true)
        };
        foreach (var credential in credentials)
        {
            res.Add(new SelectionItem<Credential?>(credential, credential.Name, false));
        }
        return res;
    }

    public IReadOnlyList<SelectionItem<PostProcessorArgument?>> GetAvailablePostProcessorArguments()
    {
        var res = new List<SelectionItem<PostProcessorArgument?>>(_configurationService.PostprocessingArguments.Count + 1);
        var hasSelected = false;
        foreach (var argument in _configurationService.PostprocessingArguments)
        {
            var shouldSelect = _configurationService.PreviousPostProcessorArgumentName == argument.Name;
            hasSelected |= shouldSelect;
            res.Add(new SelectionItem<PostProcessorArgument?>(argument, argument.Name, shouldSelect));
        }
        res.Insert(0, new SelectionItem<PostProcessorArgument?>(null, _translationService._("None"), !hasSelected));
        return res;
    }

    public bool GetShouldShowDownloadImmediatelyTeach()
    {
        if (!_configurationService.PreviousDownloadImmediatelyAsVideo && !_configurationService.PreviousDownloadImmediatelyAsAudio && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.DownloadImmediately))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.DownloadImmediately;
            return true;
        }
        return false;
    }

    public bool GetShouldShowFileTypeTeach(DiscoveryContext context, SelectionItem<MediaFileType> selectedFileType)
    {
        var previousFileType = context.Media.Any(m => m.Type == MediaType.Video) ? _configurationService.PreviousFullFileType : _configurationService.PreviousAudioOnlyFileType;
        if (!previousFileType.IsGeneric && selectedFileType.Value.IsGeneric && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.FileType))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.FileType;
            return true;
        }
        return false;
    }

    public bool GetShouldShowNumberTitlesTeach()
    {
        if (!_configurationService.PreviousNumberTitles && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.NumberTitles))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.NumberTitles;
            return true;
        }
        return false;
    }

    public Task<byte[]> GetThumbnailImageBytesAsync(DiscoveryContext context) => _thumbnailService.GetImageBytesAsync(context.Media[0].Url);

    public Task<MemoryStream> GetThumbnailImageStreamAsync(DiscoveryContext context) => _thumbnailService.GetImageStreamAsync(context.Media[0].Url);
}
