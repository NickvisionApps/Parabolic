using Microsoft.Extensions.Logging;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.Keyring;
using Nickvision.Desktop.Notifications;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections;
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
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly IJsonFileService _jsonFileService;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IThumbnailService _thumbnailService;
    private readonly ITranslationService _translationService;
    private readonly Dictionary<int, DiscoveryContext> _discoveryContextMap;

    public PreviousDownloadOptions PreviousDownloadOptions { get; }
    public IReadOnlyList<SelectionItem<Credential?>> AvailableCredentials { get; }
    public IReadOnlyList<SelectionItem<PostProcessorArgument?>> AvailablePostProcessorArguments { get; }

    public AddDownloadDialogController(ILogger<AddDownloadDialogController> logger, IDiscoveryService discoveryService, IDownloadService downloadService, IKeyringService keyringService, IJsonFileService jsonFileService, INotificationService notificationService, IThumbnailService thumbnailService, ITranslationService translationService)
    {
        _logger = logger;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _thumbnailService = thumbnailService;
        _translationService = translationService;
        _discoveryContextMap = new Dictionary<int, DiscoveryContext>();
        PreviousDownloadOptions = _jsonFileService.Load(ApplicationJsonContext.Default.PreviousDownloadOptions, PreviousDownloadOptions.Key);
        AvailableCredentials = new List<SelectionItem<Credential?>>(_keyringService.Credentials.Count() + 1)
        {
            new SelectionItem<Credential?>(null, _translationService._("Use manual credential"), true)
        };
        foreach (var credential in _keyringService.Credentials)
        {
            (AvailableCredentials as IList)!.Add(new SelectionItem<Credential?>(credential, credential.Name, false));
        }
        var postprocessingArguments = _jsonFileService.Load(ApplicationJsonContext.Default.Configuration, Configuration.Key).PostprocessingArguments;
        AvailablePostProcessorArguments = new List<SelectionItem<PostProcessorArgument?>>(postprocessingArguments.Count + 1);
        foreach (var argument in postprocessingArguments)
        {
            (AvailablePostProcessorArguments as IList)!.Add(new SelectionItem<PostProcessorArgument?>(argument, argument.Name, PreviousDownloadOptions.PostProcessorArgumentName == argument.Name));
        }
        (AvailablePostProcessorArguments as IList)!.Insert(0, new SelectionItem<PostProcessorArgument?>(null, _translationService._("None"), !AvailablePostProcessorArguments.Any(x => x.ShouldSelect)));
    }

    public async Task AddPlaylistDownloadsAsync(DiscoveryContext context, IEnumerable<MediaSelectionItem> items, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<VideoResolution> selectedVideoResoltuion, SelectionItem<double> selectedAudioBitrate, bool reverseDownloadOrder, bool numberTitles, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool exportM3U, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument)
    {
        var downloader = (await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key)).DownloaderOptions;
        var m3uFile = new M3UFile(context.Title, context.Media.Any(x => !string.IsNullOrEmpty(x.SuggestedSaveFolder)) ? PathType.Absolute : PathType.Relative);
        var options = new List<DownloadOptions>(items.Count());
        var titleNumber = 1;
        foreach (var item in reverseDownloadOrder ? items.Reverse() : items)
        {
            if (item.Value < 0 || item.Value >= context.Media.Count)
            {
                continue;
            }
            var media = context.Media[item.Value];
            options.Add(new DownloadOptions(media.Url)
            {
                Credential = context.Credential,
                SaveFilename = $"{(numberTitles ? $"{titleNumber++} - " : string.Empty)}{(string.IsNullOrEmpty(item.Filename) ? media.Title : item.Filename.SanitizeForFilename(downloader.LimitCharacters))}",
                SaveFolder = Path.Combine(!string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : saveFolder, context.Title.SanitizeForFilename(downloader.LimitCharacters)),
                FileType = selectedFileType.Value.IsVideo && media.Type == MediaType.Audio ? PreviousDownloadOptions.AudioOnlyFileType : selectedFileType.Value,
                PlaylistPosition = media.PlaylistPosition,
                VideoResolution = selectedVideoResoltuion.Value,
                AudioBitrate = selectedAudioBitrate.Value,
                SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).Where(x => media.Subtitles.Contains(x)).ToArray(),
                SplitChapters = splitChapters,
                ExportDescription = exportDescription,
                PostProcessorArgument = selectedPostProcessorArgument.Value,
                TimeFrame = TimeFrame.TryParse(item.StartTime, item.EndTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
            });
            m3uFile.Add(options);
        }
        PreviousDownloadOptions.SaveFolder = saveFolder;
        if (context.Media.Any(m => m.Type == MediaType.Video))
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
        PreviousDownloadOptions.ReverseDownloadOrder = reverseDownloadOrder;
        PreviousDownloadOptions.NumberTitles = numberTitles;
        PreviousDownloadOptions.SplitChapters = splitChapters;
        PreviousDownloadOptions.ExportDescription = exportDescription;
        if (selectedPostProcessorArgument.Value is not null)
        {
            PreviousDownloadOptions.PostProcessorArgumentName = selectedPostProcessorArgument.Value.Name;
        }
        PreviousDownloadOptions.SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).ToArray();
        await _jsonFileService.SaveAsync(PreviousDownloadOptions, ApplicationJsonContext.Default.PreviousDownloadOptions, PreviousDownloadOptions.Key);
        try
        {
            await _downloadService.AddAsync(options, excludeFromHistory);
            if (exportM3U)
            {
                await m3uFile.WriteAsync(Path.Combine(saveFolder, context.Title.SanitizeForFilename(downloader.LimitCharacters), $"{context.Title.SanitizeForFilename(downloader.LimitCharacters)}.m3u"));
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
        var downloader = (await _jsonFileService.LoadAsync(ApplicationJsonContext.Default.Configuration, Configuration.Key)).DownloaderOptions;
        var media = context.Media[0];
        var options = new DownloadOptions(media.Url)
        {
            Credential = context.Credential,
            SaveFilename = string.IsNullOrEmpty(saveFilename) ? media.Title : saveFilename.SanitizeForFilename(downloader.LimitCharacters),
            SaveFolder = saveFolder,
            FileType = selectedFileType.Value,
            PlaylistPosition = media.PlaylistPosition,
            VideoFormat = selectedVideoFormat.Value,
            AudioFormat = selectedAudioFormat.Value,
            SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).ToArray(),
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
        await _jsonFileService.SaveAsync(PreviousDownloadOptions, ApplicationJsonContext.Default.PreviousDownloadOptions, PreviousDownloadOptions.Key);
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
                foreach (var bitrate in res.Media.SelectMany(m => m.Formats).Where(f => f.Bitrate.HasValue).Select(f => f.Bitrate!.Value).Distinct())
                {
                    context.AudioBitrates.Add(new SelectionItem<double>(bitrate, $"{bitrate}k", PreviousDownloadOptions.AudioBitrate == bitrate));
                    matched |= PreviousDownloadOptions.AudioBitrate == bitrate;
                }
                context.AudioBitrates.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(-1.0, _translationService._("Worst"), PreviousDownloadOptions.AudioBitrate == -1.0));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(double.MaxValue, _translationService._("Best"), !matched || PreviousDownloadOptions.AudioBitrate == double.MaxValue));
                matched = false;
                foreach (var resolution in res.Media.SelectMany(m => m.Formats).Where(f => f.VideoResolution is not null).Select(f => f.VideoResolution!).Distinct())
                {
                    context.VideoResolutions.Add(new SelectionItem<VideoResolution>(resolution, resolution.ToString(_translationService), PreviousDownloadOptions.VideoResolution == resolution));
                    matched |= PreviousDownloadOptions.VideoResolution == resolution;
                }
                context.VideoResolutions.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Worst, VideoResolution.Worst.ToString(_translationService), PreviousDownloadOptions.VideoResolution == VideoResolution.Worst));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Best, VideoResolution.Best.ToString(_translationService), !matched || PreviousDownloadOptions.VideoResolution == VideoResolution.Best));
            }
            var hasVideo = res.Media.Any(m => m.Type == MediaType.Video);
            var previousFileType = hasVideo ? PreviousDownloadOptions.FullFileType : PreviousDownloadOptions.AudioOnlyFileType;
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
            foreach (var subtitle in res.Media.SelectMany(m => m.Subtitles).Distinct())
            {
                context.SubtitleLanguages.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(_translationService), PreviousDownloadOptions.SubtitleLanguages.Contains(subtitle)));
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

    public bool GetShouldShowDownloadImmediatelyTeach()
    {
        if (!PreviousDownloadOptions.DownloadImmediately && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.DownloadImmediately))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.DownloadImmediately;
            return true;
        }
        return false;
    }

    public bool GetShouldShowFileTypeTeach(DiscoveryContext context, SelectionItem<MediaFileType> selectedFileType)
    {
        var previousFileType = context.Media.Any(m => m.Type == MediaType.Video) ? PreviousDownloadOptions.FullFileType : PreviousDownloadOptions.AudioOnlyFileType;
        if (!previousFileType.IsGeneric && selectedFileType.Value.IsGeneric && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.FileType))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.FileType;
            return true;
        }
        return false;
    }

    public bool GetShouldShowNumberTitlesTeach()
    {
        if (!PreviousDownloadOptions.NumberTitles && !_shownTeachTypeFlag.HasFlag(AddDownloadTeachType.NumberTitles))
        {
            _shownTeachTypeFlag |= AddDownloadTeachType.NumberTitles;
            return true;
        }
        return false;
    }

    public Task<byte[]> GetThumbnailImageBytesAsync(DiscoveryContext context) => _thumbnailService.GetImageBytesAsync(context.Media[0].Url);

    public Task<MemoryStream> GetThumbnailImageStreamAsync(DiscoveryContext context) => _thumbnailService.GetImageStreamAsync(context.Media[0].Url);
}
