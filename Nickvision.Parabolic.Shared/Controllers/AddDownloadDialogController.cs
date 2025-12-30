using Nickvision.Desktop.Application;
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
    private static AddDownloadTeachType _shownTeachTypeFlag;

    private readonly IJsonFileService _jsonFileService;
    private readonly IKeyringService _keyringService;
    private readonly INotificationService _notificationService;
    private readonly IDiscoveryService _discoveryService;
    private readonly IDownloadService _downloadService;
    private readonly Dictionary<int, DiscoveryContext> _discoveryContextMap;

    public ITranslationService Translator { get; }
    public PreviousDownloadOptions PreviousDownloadOptions { get; }
    public IReadOnlyList<SelectionItem<Credential?>> AvailableCredentials { get; }
    public IReadOnlyList<SelectionItem<PostProcessorArgument?>> AvailablePostProcessorArguments { get; }

    public AddDownloadDialogController(IJsonFileService jsonFileService, ITranslationService translationService, IKeyringService keyringService, INotificationService notificationService, IDiscoveryService discoveryService, IDownloadService downloadService)
    {
        _jsonFileService = jsonFileService;
        _keyringService = keyringService;
        _notificationService = notificationService;
        _discoveryService = discoveryService;
        _downloadService = downloadService;
        _discoveryContextMap = new Dictionary<int, DiscoveryContext>();
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

    public async Task AddPlaylistDownloadsAsync(DiscoveryContext context, IEnumerable<MediaSelectionItem> items, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<VideoResolution> selectedVideoResoltuion, SelectionItem<double> selectedAudioBitrate, bool numberTitles, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool exportM3U, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument)
    {
        var downloader = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
        var m3uFile = new M3UFile(context.Title, context.Media.Any(x => !string.IsNullOrEmpty(x.SuggestedSaveFolder)) ? PathType.Absolute : PathType.Relative);
        var options = new List<DownloadOptions>(items.Count());
        var titleNumber = 1;
        foreach (var item in items)
        {
            if (item.Value < 0 || item.Value >= context.Media.Count)
            {
                continue;
            }
            var media = context.Media[item.Value];
            options.Add(new DownloadOptions(media.Url)
            {
                Credential = context.Credential,
                SaveFilename = $"{(numberTitles ? $"{titleNumber} - " : string.Empty)}{(string.IsNullOrEmpty(item.Filename) ? media.Title : item.Filename.SanitizeForFilename(downloader.LimitCharacters))}",
                SaveFolder = Path.Combine(!string.IsNullOrEmpty(media.SuggestedSaveFolder) ? media.SuggestedSaveFolder : saveFolder, context.Title.SanitizeForFilename(downloader.LimitCharacters)),
                FileType = selectedFileType.Value.IsVideo && media.Type == MediaType.Audio ? PreviousDownloadOptions.AudioOnlyFileType : selectedFileType.Value,
                PlaylistPosition = numberTitles ? titleNumber : media.PlaylistPosition,
                VideoResolution = selectedVideoResoltuion.Value,
                AudioBitrate = selectedAudioBitrate.Value,
                SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).Where(x => media.Subtitles.Contains(x)).ToArray(),
                SplitChapters = splitChapters,
                ExportDescription = exportDescription,
                PostProcessorArgument = selectedPostProcessorArgument.Value,
                TimeFrame = TimeFrame.TryParse(item.StartTime, item.EndTime, media.TimeFrame.Duration, out var timeFrame) && timeFrame != media.TimeFrame ? timeFrame : null
            });
            m3uFile.Add(options);
            titleNumber++;
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
        PreviousDownloadOptions.NumberTitles = numberTitles;
        PreviousDownloadOptions.SplitChapters = splitChapters;
        PreviousDownloadOptions.ExportDescription = exportDescription;
        if (selectedPostProcessorArgument.Value is not null)
        {
            PreviousDownloadOptions.PostProcessorArgumentName = selectedPostProcessorArgument.Value.Name;
        }
        PreviousDownloadOptions.SubtitleLanguages = selectedSubtitleLanguages.Select(x => x.Value).ToArray();
        await _jsonFileService.SaveAsync(PreviousDownloadOptions, PreviousDownloadOptions.Key);
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
            _notificationService.Send(new AppNotification(Translator._("An error occured while adding playlist downloads"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
            });
        }
        _discoveryContextMap.Remove(context.Id);
    }

    public async Task AddSingleDownloadAsync(DiscoveryContext context, string saveFilename, string saveFolder, SelectionItem<MediaFileType> selectedFileType, SelectionItem<Format> selectedVideoFormat, SelectionItem<Format> selectedAudioFormat, IEnumerable<SelectionItem<SubtitleLanguage>> selectedSubtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, SelectionItem<PostProcessorArgument?> selectedPostProcessorArgument, string startTime, string endTime)
    {
        var downloader = (await _jsonFileService.LoadAsync<Configuration>(Configuration.Key)).DownloaderOptions;
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
        _discoveryContextMap.Remove(context.Id);
    }

    public async Task<DiscoveryContext?> DiscoverAsync(Uri url, Credential? credential, CancellationToken cancellationToken)
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
                return null;
            }
            var context = new DiscoveryContext(res.Id, res.Url, res.Title, credential, res.Media);
            if (!res.IsPlaylist)
            {
                foreach (var format in res.Media[0].Formats)
                {
                    var formatSelectionItem = new SelectionItem<Format>(format, format.ToString(Translator), false);
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
                foreach (var bitrate in res.Media.SelectMany(m => m.Formats).Where(f => f.Bitrate.HasValue).Select(f => f.Bitrate!.Value).Distinct())
                {
                    context.AudioBitrates.Add(new SelectionItem<double>(bitrate, $"{bitrate}k", PreviousDownloadOptions.AudioBitrate == bitrate));
                }
                context.AudioBitrates.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(-1.0, Translator._("Worst"), PreviousDownloadOptions.AudioBitrate == -1.0));
                context.AudioBitrates.Insert(0, new SelectionItem<double>(double.MaxValue, Translator._("Best"), PreviousDownloadOptions.AudioBitrate == double.MaxValue));
                foreach (var resolution in res.Media.SelectMany(m => m.Formats).Where(f => f.VideoResolution is not null).Select(f => f.VideoResolution!).Distinct())
                {
                    context.VideoResolutions.Add(new SelectionItem<VideoResolution>(resolution, resolution.ToString(Translator), PreviousDownloadOptions.VideoResolution == resolution));
                }
                context.VideoResolutions.Sort((a, b) => a.Value.CompareTo(b.Value));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Worst, VideoResolution.Worst.ToString(Translator), PreviousDownloadOptions.VideoResolution == VideoResolution.Worst));
                context.VideoResolutions.Insert(0, new SelectionItem<VideoResolution>(VideoResolution.Best, VideoResolution.Best.ToString(Translator), PreviousDownloadOptions.VideoResolution == VideoResolution.Best));
            }
            var hasVideo = res.Media.Any(m => m.Type == MediaType.Video);
            var previousFileType = hasVideo ? PreviousDownloadOptions.FullFileType : PreviousDownloadOptions.AudioOnlyFileType;
            context.FileTypes.EnsureCapacity(hasVideo ? 13 : 7);
            if (hasVideo)
            {
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Video, Translator._("Video (Generic)"), previousFileType == MediaFileType.Video));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP4, Translator._("MP4 (Video)"), previousFileType == MediaFileType.MP4));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WEBM, Translator._("WEBM (Video)"), previousFileType == MediaFileType.WEBM));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MKV, Translator._("MKV (Video)"), previousFileType == MediaFileType.MKV));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MOV, Translator._("MOV (Video)"), previousFileType == MediaFileType.MOV));
                context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.AVI, Translator._("AVI (Video)"), previousFileType == MediaFileType.AVI));
            }
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.Audio, Translator._("Audio (Generic)"), previousFileType == MediaFileType.Audio));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.MP3, Translator._("MP3 (Audio)"), previousFileType == MediaFileType.MP3));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.M4A, Translator._("M4A (Audio)"), previousFileType == MediaFileType.M4A));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OPUS, Translator._("OPUS (Audio)"), previousFileType == MediaFileType.OPUS));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.FLAC, Translator._("FLAC (Audio)"), previousFileType == MediaFileType.FLAC));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.WAV, Translator._("WAV (Audio)"), previousFileType == MediaFileType.WAV));
            context.FileTypes.Add(new SelectionItem<MediaFileType>(MediaFileType.OGG, Translator._("OGG (Audio)"), previousFileType == MediaFileType.OGG));
            foreach (var subtitle in res.Media.SelectMany(m => m.Subtitles).Distinct())
            {
                context.SubtitleLanguages.Add(new SelectionItem<SubtitleLanguage>(subtitle, subtitle.ToString(Translator), PreviousDownloadOptions.SubtitleLanguages.Contains(subtitle)));
            }
            context.SubtitleLanguages.Sort((a, b) => a.Value.CompareTo(b.Value));
            context.Items.EnsureCapacity(res.Media.Count);
            for (var i = 0; i < res.Media.Count; i++)
            {
                var media = res.Media[i];
                context.Items.Add(new MediaSelectionItem(i, media.Title, media.TimeFrame.StartString, media.TimeFrame.EndString, Translator));
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
            _notificationService.Send(new AppNotification(Translator._("An error occured while discovering media"), NotificationSeverity.Error)
            {
                Action = "error",
                ActionParam = e.Message
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
}
