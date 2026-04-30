using Microsoft.Data.Sqlite;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class PreferencesViewController : IDisposable
{
    private readonly IConfigurationService _configurationService;
    private readonly ITranslationService _translationService;
    private SqliteTransaction? _transaction;

    public IReadOnlyList<SelectionItem<AudioCodec>> AudioCodecs { get; }
    public IReadOnlyList<SelectionItem<string>> AvailableTranslationLanguages { get; }
    public IReadOnlyList<SelectionItem<Browser>> Browsers { get; }
    public IReadOnlyList<SelectionItem<Executable>> Executables { get; }
    public IReadOnlyList<SelectionItem<FrameRate>> FrameRates { get; }
    public IReadOnlyList<SelectionItem<HistoryLength>> HistoryLengths { get; }
    public IReadOnlyList<SelectionItem<PostProcessor>> PostProcessors { get; }
    public ObservableCollection<PostProcessorArgument> PostprocessingArguments { get; }
    public IReadOnlyList<SelectionItem<SubtitleFormat>> SubtitleFormats { get; }
    public IReadOnlyList<SelectionItem<Theme>> Themes { get; }
    public IReadOnlyList<SelectionItem<VideoCodec>> VideoCodecs { get; }

    public PreferencesViewController(IConfigurationService configurationService, ITranslationService translationService)
    {
        _configurationService = configurationService;
        _translationService = translationService;
        _transaction = _configurationService.CreateTransaction();
        var selectedHistoryLength = _configurationService.HistoryLength;
        AudioCodecs = new List<SelectionItem<AudioCodec>>()
        {
            new SelectionItem<AudioCodec>(AudioCodec.Any, _translationService._("Any"), _configurationService.PreferredAudioCodec == AudioCodec.Any),
            new SelectionItem<AudioCodec>(AudioCodec.FLAC, _translationService._("FLAC (ALAC)"), _configurationService.PreferredAudioCodec == AudioCodec.FLAC),
            new SelectionItem<AudioCodec>(AudioCodec.WAV, _translationService._("WAV (AIFF)"), _configurationService.PreferredAudioCodec == AudioCodec.WAV),
            new SelectionItem<AudioCodec>(AudioCodec.OPUS, "OPUS", _configurationService.PreferredAudioCodec == AudioCodec.OPUS),
            new SelectionItem<AudioCodec>(AudioCodec.AAC, "AAC", _configurationService.PreferredAudioCodec == AudioCodec.AAC),
            new SelectionItem<AudioCodec>(AudioCodec.MP4A, "MP4A", _configurationService.PreferredAudioCodec == AudioCodec.MP4A),
            new SelectionItem<AudioCodec>(AudioCodec.MP3, "MP3", _configurationService.PreferredAudioCodec == AudioCodec.MP3)
        };
        AvailableTranslationLanguages = new List<SelectionItem<string>>()
        {
            new SelectionItem<string>(string.Empty, _translationService._("System"), string.IsNullOrEmpty(_configurationService.TranslationLanguage)),
            new SelectionItem<string>("C", "en_US", _configurationService.TranslationLanguage == "C")
        };
        var languages = _translationService.AvailableLanguages.ToList();
        languages.Sort();
        foreach (var language in languages)
        {
            (AvailableTranslationLanguages as IList)!.Add(new SelectionItem<string>(language, language, _configurationService.TranslationLanguage == language));
        }
        Browsers = new List<SelectionItem<Browser>>(OperatingSystem.IsWindows() ? 2 : 9)
        {
            new SelectionItem<Browser>(Browser.None, _translationService._("None"), _configurationService.CookiesBrowser == Browser.None),
            new SelectionItem<Browser>(Browser.Firefox, _translationService._("Firefox"), _configurationService.CookiesBrowser == Browser.Firefox)
        };
        if (!OperatingSystem.IsWindows())
        {
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Brave, _translationService._("Brave"), _configurationService.CookiesBrowser == Browser.Brave));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Chrome, _translationService._("Chrome"), _configurationService.CookiesBrowser == Browser.Chrome));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Chromium, _translationService._("Chromium"), _configurationService.CookiesBrowser == Browser.Chromium));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Edge, _translationService._("Edge"), _configurationService.CookiesBrowser == Browser.Edge));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Opera, _translationService._("Opera"), _configurationService.CookiesBrowser == Browser.Opera));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Vivaldi, _translationService._("Vivaldi"), _configurationService.CookiesBrowser == Browser.Vivaldi));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Whale, _translationService._("Whale"), _configurationService.CookiesBrowser == Browser.Whale));
        }
        Executables = new List<SelectionItem<Executable>>()
        {
            new SelectionItem<Executable>(Executable.None, _translationService._("None"), true)
        };
        foreach (var executable in Enum.GetValues<Executable>())
        {
            if (executable == Executable.None)
            {
                continue;
            }
            (Executables as IList)!.Add(new SelectionItem<Executable>(executable, executable.ToString(), false));
        }
        FrameRates = new List<SelectionItem<FrameRate>>()
        {
            new SelectionItem<FrameRate>(FrameRate.Any, _translationService._("Any"), _configurationService.PreferredFrameRate == FrameRate.Any),
            new SelectionItem<FrameRate>(FrameRate.Fps24, _translationService._("{0} FPS", 24), _configurationService.PreferredFrameRate == FrameRate.Fps24),
            new SelectionItem<FrameRate>(FrameRate.Fps30, _translationService._("{0} FPS", 30), _configurationService.PreferredFrameRate == FrameRate.Fps30),
            new SelectionItem<FrameRate>(FrameRate.Fps60, _translationService._("{0} FPS", 60), _configurationService.PreferredFrameRate == FrameRate.Fps60)
        };
        HistoryLengths = new List<SelectionItem<HistoryLength>>()
        {
            new SelectionItem<HistoryLength>(Models.HistoryLength.Never, _translationService._("Never"), selectedHistoryLength == Models.HistoryLength.Never),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneDay, _translationService._("1 Day"), selectedHistoryLength == Models.HistoryLength.OneDay),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneWeek, _translationService._("1 Week"), selectedHistoryLength == Models.HistoryLength.OneWeek),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneMonth, _translationService._("1 Month"), selectedHistoryLength == Models.HistoryLength.OneMonth),
            new SelectionItem<HistoryLength>(Models.HistoryLength.ThreeMonths, _translationService._("3 Months"), selectedHistoryLength == Models.HistoryLength.ThreeMonths),
            new SelectionItem<HistoryLength>(Models.HistoryLength.SixMonths, _translationService._("6 Months"), selectedHistoryLength == Models.HistoryLength.SixMonths),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneYear, _translationService._("1 Year"), selectedHistoryLength == Models.HistoryLength.OneYear),
            new SelectionItem<HistoryLength>(Models.HistoryLength.Forever, _translationService._("Forever"), selectedHistoryLength == Models.HistoryLength.Forever),
        };
        PostProcessors = new List<SelectionItem<PostProcessor>>()
        {
            new SelectionItem<PostProcessor>(PostProcessor.None, _translationService._("None"), true)
        };
        foreach (var processor in Enum.GetValues<PostProcessor>())
        {
            if (processor == PostProcessor.None)
            {
                continue;
            }
            (PostProcessors as IList)!.Add(new SelectionItem<PostProcessor>(processor, processor.ToString(), false));
        }
        PostprocessingArguments = new ObservableCollection<PostProcessorArgument>(_configurationService.PostprocessingArguments);
        SubtitleFormats = new List<SelectionItem<SubtitleFormat>>()
        {
            new SelectionItem<SubtitleFormat>(SubtitleFormat.Any, _translationService._("Any"), _configurationService.PreferredSubtitleFormat == SubtitleFormat.Any),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.VTT, "VTT", _configurationService.PreferredSubtitleFormat == SubtitleFormat.VTT),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.SRT, "SRT", _configurationService.PreferredSubtitleFormat == SubtitleFormat.SRT),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.ASS, "ASS", _configurationService.PreferredSubtitleFormat == SubtitleFormat.ASS),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.LRC, "LRC", _configurationService.PreferredSubtitleFormat == SubtitleFormat.LRC)
        };
        Themes = new List<SelectionItem<Theme>>()
        {
            new SelectionItem<Theme>(Models.Theme.Light, _translationService._p("Theme", "Light"), _configurationService.Theme == Models.Theme.Light),
            new SelectionItem<Theme>(Models.Theme.Dark, _translationService._p("Theme", "Dark"), _configurationService.Theme == Models.Theme.Dark),
            new SelectionItem<Theme>(Models.Theme.System, _translationService._p("Theme", "System"), _configurationService.Theme == Models.Theme.System),
        };
        VideoCodecs = new List<SelectionItem<VideoCodec>>()
        {
            new SelectionItem<VideoCodec>(VideoCodec.Any, _translationService._("Any"), _configurationService.PreferredVideoCodec == VideoCodec.Any),
            new SelectionItem<VideoCodec>(VideoCodec.VP9, "VP9", _configurationService.PreferredVideoCodec == VideoCodec.VP9),
            new SelectionItem<VideoCodec>(VideoCodec.AV01, "AV1", _configurationService.PreferredVideoCodec == VideoCodec.AV01),
            new SelectionItem<VideoCodec>(VideoCodec.H264, _translationService._("H.264 (AVC)"), _configurationService.PreferredVideoCodec == VideoCodec.H264),
            new SelectionItem<VideoCodec>(VideoCodec.H265, _translationService._("H.265 (HEVC)"), _configurationService.PreferredVideoCodec == VideoCodec.H265)
        };
    }

    ~PreferencesViewController()
    {
        Dispose(false);
    }

    public bool AllowPreviewUpdates
    {
        get => _configurationService.AllowPreviewUpdates;

        set => _configurationService.AllowPreviewUpdates = value;
    }

    public int AriaMaxConnectionsPerServer
    {
        get => _configurationService.AriaMaxConnectionsPerServer;

        set => _configurationService.AriaMaxConnectionsPerServer = value;
    }

    public int AriaMinSplitSize
    {
        get => _configurationService.AriaMinSplitSize;

        set => _configurationService.AriaMinSplitSize = value;
    }

    public bool CropAudioThumbnails
    {
        get => _configurationService.CropAudioThumbnails;

        set => _configurationService.CropAudioThumbnails = value;
    }

    public SelectionItem<Browser> CookiesBrowser
    {
        set => _configurationService.CookiesBrowser = value.Value;
    }

    public string CookiesPath
    {
        get => _configurationService.CookiesPath;

        set => _configurationService.CookiesPath = value;
    }

    public bool EmbedChapters
    {
        get => _configurationService.EmbedChapters;

        set => _configurationService.EmbedChapters = value;
    }

    public bool EmbedMetadata
    {
        get => _configurationService.EmbedMetadata;

        set => _configurationService.EmbedMetadata = value;
    }

    public bool EmbedSubtitles
    {
        get => _configurationService.EmbedSubtitles;

        set => _configurationService.EmbedSubtitles = value;
    }

    public bool EmbedThumbnails
    {
        get => _configurationService.EmbedThumbnails;

        set => _configurationService.EmbedThumbnails = value;
    }

    public SelectionItem<HistoryLength> HistoryLength
    {
        set => _configurationService.HistoryLength = value.Value;
    }

    public bool IncludeAutoGeneratedSubtitles
    {
        get => _configurationService.IncludeAutoGeneratedSubtitles;

        set => _configurationService.IncludeAutoGeneratedSubtitles = value;
    }

    public bool IncludeMediaIdInTitle
    {
        get => _configurationService.IncludeMediaIdInTitle;

        set => _configurationService.IncludeMediaIdInTitle = value;
    }

    public bool IncludeSuperResolutions
    {
        get => _configurationService.IncludeSuperResolutions;

        set => _configurationService.IncludeSuperResolutions = value;
    }

    public bool LimitCharacters
    {
        get => _configurationService.LimitCharacters;

        set => _configurationService.LimitCharacters = value;
    }

    public int MaxNumberOfActiveDownloads
    {
        get => _configurationService.MaxNumberOfActiveDownloads;

        set => _configurationService.MaxNumberOfActiveDownloads = value;
    }

    public bool OverwriteExistingFiles
    {
        get => _configurationService.OverwriteExistingFiles;

        set => _configurationService.OverwriteExistingFiles = value;
    }

    public int PostprocessingThreads
    {
        get => _configurationService.PostprocessingThreads;

        set => _configurationService.PostprocessingThreads = value;
    }

    public SelectionItem<AudioCodec> PreferredAudioCodec
    {
        set => _configurationService.PreferredAudioCodec = value.Value;
    }

    public SelectionItem<FrameRate> PreferredFrameRate
    {
        set => _configurationService.PreferredFrameRate = value.Value;
    }

    public SelectionItem<SubtitleFormat> PreferredSubtitleFormat
    {
        set => _configurationService.PreferredSubtitleFormat = value.Value;
    }

    public SelectionItem<VideoCodec> PreferredVideoCodec
    {
        set => _configurationService.PreferredVideoCodec = value.Value;
    }

    public bool PreventSuspend
    {
        get => _configurationService.PreventSuspend;

        set => _configurationService.PreventSuspend = value;
    }

    public string ProxyUrl
    {
        get => _configurationService.ProxyUrl;

        set => _configurationService.ProxyUrl = value;
    }

    public bool RemoveSourceData
    {
        get => _configurationService.RemoveSourceData;

        set => _configurationService.RemoveSourceData = value;
    }

    public int? SpeedLimit
    {
        get => _configurationService.SpeedLimit;

        set => _configurationService.SpeedLimit = value;
    }

    public SelectionItem<Theme> Theme
    {
        set => _configurationService.Theme = value.Value;
    }

    public bool TranslateMetadataAndChapters
    {
        get => _configurationService.TranslateMetadataAndChapters;

        set => _configurationService.TranslateMetadataAndChapters = value;
    }

    public SelectionItem<string> TranslationLanguage
    {
        set => _configurationService.TranslationLanguage = value.Value;
    }

    public bool UseAria
    {
        get => _configurationService.UseAria;

        set => _configurationService.UseAria = value;
    }

    public bool UsePartFiles
    {
        get => _configurationService.UsePartFiles;

        set => _configurationService.UsePartFiles = value;
    }

    public bool YouTubeSponsorBlock
    {
        get => _configurationService.YouTubeSponsorBlock;

        set => _configurationService.YouTubeSponsorBlock = value;
    }

    public string YtdlpDiscoveryArgs
    {
        get => _configurationService.YtdlpDiscoveryArgs;

        set => _configurationService.YtdlpDiscoveryArgs = value;
    }

    public string YtdlpDownloadArgs
    {
        get => _configurationService.YtdlpDownloadArgs;

        set => _configurationService.YtdlpDownloadArgs = value;
    }

    public async Task<string?> AddPostprocessingArgumentAsync(string name, SelectionItem<PostProcessor> selectedPostProcessor, SelectionItem<Executable> selectedExecutable, string arguments)
    {
        if (_configurationService.PostprocessingArguments.Any(arg => arg.Name == name))
        {
            return _translationService._("An argument with that name already exists");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return _translationService._("The name of the argument cannot be empty");
        }
        else if (string.IsNullOrEmpty(arguments))
        {
            return _translationService._("The arguments of the argument cannot be empty");
        }
        var argument = new PostProcessorArgument(name, selectedPostProcessor.Value, selectedExecutable.Value, arguments);
        PostprocessingArguments.Add(argument);
        _configurationService.PostprocessingArguments = PostprocessingArguments.ToList();
        await SaveConfigurationAsync();
        return null;
    }

    public async Task DeletePostprocessingArgumentAsync(string name)
    {
        var argument = _configurationService.PostprocessingArguments.FirstOrDefault(arg => arg.Name == name);
        if (argument is null)
        {
            return;
        }
        PostprocessingArguments.Remove(argument);
        _configurationService.PostprocessingArguments = PostprocessingArguments.ToList();
        await SaveConfigurationAsync();
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    public async Task<string?> UpdatePostprocessingArgumentAsync(string name, SelectionItem<PostProcessor> selectedPostProcessor, SelectionItem<Executable> selectedExecutable, string arguments)
    {
        var index = _configurationService.PostprocessingArguments.FindIndex(arg => arg.Name == name);
        if (index == -1)
        {
            return _translationService._("An argument with that name does not exist");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return _translationService._("The name of the argument cannot be empty");
        }
        else if (string.IsNullOrEmpty(arguments))
        {
            return _translationService._("The arguments of the argument cannot be empty");
        }
        var argument = new PostProcessorArgument(name, selectedPostProcessor.Value, selectedExecutable.Value, arguments);
        PostprocessingArguments[index] = argument;
        _configurationService.PostprocessingArguments = PostprocessingArguments.ToList();
        await SaveConfigurationAsync();
        return null;
    }

    public async Task SaveConfigurationAsync()
    {
        if (_transaction is not null)
        {
            await _transaction.CommitAsync();
            await _transaction.DisposeAsync();
            _transaction = null;
        }
    }

    private void Dispose(bool disposing)
    {
        if (!disposing)
        {
            return;
        }
        _transaction?.Dispose();
    }
}
