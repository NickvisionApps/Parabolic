using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class PreferencesViewController
{
    private readonly IJsonFileService _jsonFileService;
    private readonly IHistoryService _historyService;
    private readonly Configuration _configuration;

    public ITranslationService Translator { get; }
    public IReadOnlyList<SelectionItem<AudioCodec>> AudioCodecs { get; }
    public IReadOnlyList<SelectionItem<string>> AvailableTranslationLanguages { get; }
    public IReadOnlyList<SelectionItem<Browser>> Browsers { get; }
    public IReadOnlyList<SelectionItem<Executable>> Executables { get; }
    public IReadOnlyList<SelectionItem<HistoryLength>> HistoryLengths { get; }
    public IReadOnlyList<SelectionItem<PostProcessor>> PostProcessors { get; }
    public ObservableCollection<PostProcessorArgument> PostprocessingArguments { get; }
    public IReadOnlyList<SelectionItem<SubtitleFormat>> SubtitleFormats { get; }
    public IReadOnlyList<SelectionItem<Theme>> Themes { get; }
    public IReadOnlyList<SelectionItem<VideoCodec>> VideoCodecs { get; }

    public PreferencesViewController(IJsonFileService jsonFileService, ITranslationService translationService, IHistoryService historyService)
    {
        _jsonFileService = jsonFileService;
        _historyService = historyService;
        _configuration = _jsonFileService.Load<Configuration>(Configuration.Key);
        var selectedHistoryLength = _historyService.Length;
        Translator = translationService;
        AudioCodecs = new List<SelectionItem<AudioCodec>>()
        {
            new SelectionItem<AudioCodec>(AudioCodec.Any, Translator._("Any"), _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.Any),
            new SelectionItem<AudioCodec>(AudioCodec.FLAC, Translator._("FLAC (ALAC)"), _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.FLAC),
            new SelectionItem<AudioCodec>(AudioCodec.WAV, Translator._("WAV (AIFF)"), _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.WAV),
            new SelectionItem<AudioCodec>(AudioCodec.OPUS, "OPUS", _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.OPUS),
            new SelectionItem<AudioCodec>(AudioCodec.AAC, "AAC", _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.AAC),
            new SelectionItem<AudioCodec>(AudioCodec.MP4A, "MP4A", _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.MP4A),
            new SelectionItem<AudioCodec>(AudioCodec.MP3, "MP3", _configuration.DownloaderOptions.PreferredAudioCodec == AudioCodec.MP3)
        };
        AvailableTranslationLanguages = new List<SelectionItem<string>>()
        {
            new SelectionItem<string>(string.Empty, Translator._("System"), string.IsNullOrEmpty(_configuration.TranslationLanguage)),
            new SelectionItem<string>("C", "en_US", _configuration.TranslationLanguage == "C")
        };
        var languages = Translator.AvailableLanguages.ToList();
        languages.Sort();
        foreach (var language in languages)
        {
            (AvailableTranslationLanguages as IList)!.Add(new SelectionItem<string>(language, language, _configuration.TranslationLanguage == language));
        }
        Browsers = new List<SelectionItem<Browser>>(OperatingSystem.IsWindows() ? 2 : 9)
        {
            new SelectionItem<Browser>(Browser.None, Translator._("None"), _configuration.DownloaderOptions.CookiesBrowser == Browser.None),
            new SelectionItem<Browser>(Browser.Firefox, Translator._("Firefox"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Firefox)
        };
        if(!OperatingSystem.IsWindows())
        {
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Brave, Translator._("Brave"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Brave));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Chrome, Translator._("Chrome"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Chrome));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Chromium, Translator._("Chromium"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Chromium));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Edge, Translator._("Edge"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Edge));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Opera, Translator._("Opera"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Opera));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Vivaldi, Translator._("Vivaldi"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Vivaldi));
            (Browsers as IList)!.Add(new SelectionItem<Browser>(Browser.Whale, Translator._("Whale"), _configuration.DownloaderOptions.CookiesBrowser == Browser.Whale));
        }
        Executables = new List<SelectionItem<Executable>>()
        {
            new SelectionItem<Executable>(Executable.None, Translator._("None"), true)
        };
        foreach (var executable in Enum.GetValues<Executable>())
        {
            if (executable == Executable.None)
            {
                continue;
            }
            (Executables as IList)!.Add(new SelectionItem<Executable>(executable, executable.ToString(), false));
        }
        HistoryLengths = new List<SelectionItem<HistoryLength>>()
        {
            new SelectionItem<HistoryLength>(Models.HistoryLength.Never, Translator._("Never"), selectedHistoryLength == Models.HistoryLength.Never),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneDay, Translator._("1 Day"), selectedHistoryLength == Models.HistoryLength.OneDay),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneWeek, Translator._("1 Week"), selectedHistoryLength == Models.HistoryLength.OneWeek),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneMonth, Translator._("1 Month"), selectedHistoryLength == Models.HistoryLength.OneMonth),
            new SelectionItem<HistoryLength>(Models.HistoryLength.ThreeMonths, Translator._("3 Months"), selectedHistoryLength == Models.HistoryLength.ThreeMonths),
            new SelectionItem<HistoryLength>(Models.HistoryLength.SixMonths, Translator._("6 Months"), selectedHistoryLength == Models.HistoryLength.SixMonths),
            new SelectionItem<HistoryLength>(Models.HistoryLength.OneYear, Translator._("1 Year"), selectedHistoryLength == Models.HistoryLength.OneYear),
            new SelectionItem<HistoryLength>(Models.HistoryLength.Forever, Translator._("Forever"), selectedHistoryLength == Models.HistoryLength.Forever),
        };
        PostProcessors = new List<SelectionItem<PostProcessor>>()
        {
            new SelectionItem<PostProcessor>(PostProcessor.None, Translator._("None"), true)
        };
        foreach (var processor in Enum.GetValues<PostProcessor>())
        {
            if (processor == PostProcessor.None)
            {
                continue;
            }
            (PostProcessors as IList)!.Add(new SelectionItem<PostProcessor>(processor, processor.ToString(), false));
        }
        PostprocessingArguments = new ObservableCollection<PostProcessorArgument>(_configuration.PostprocessingArguments);
        SubtitleFormats = new List<SelectionItem<SubtitleFormat>>()
        {
            new SelectionItem<SubtitleFormat>(SubtitleFormat.Any, Translator._("Any"), _configuration.DownloaderOptions.PreferredSubtitleFormat == SubtitleFormat.Any),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.VTT, "VTT", _configuration.DownloaderOptions.PreferredSubtitleFormat == SubtitleFormat.VTT),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.SRT, "SRT", _configuration.DownloaderOptions.PreferredSubtitleFormat == SubtitleFormat.SRT),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.ASS, "ASS", _configuration.DownloaderOptions.PreferredSubtitleFormat == SubtitleFormat.ASS),
            new SelectionItem<SubtitleFormat>(SubtitleFormat.LRC, "LRC", _configuration.DownloaderOptions.PreferredSubtitleFormat == SubtitleFormat.LRC)
        };
        Themes = new List<SelectionItem<Theme>>()
        {
            new SelectionItem<Theme>(Models.Theme.Light, Translator._p("Theme", "Light"), _configuration.Theme == Models.Theme.Light),
            new SelectionItem<Theme>(Models.Theme.Dark, Translator._p("Theme", "Dark"), _configuration.Theme == Models.Theme.Dark),
            new SelectionItem<Theme>(Models.Theme.System, Translator._p("Theme", "System"), _configuration.Theme == Models.Theme.System),
        };
        VideoCodecs = new List<SelectionItem<VideoCodec>>()
        {
            new SelectionItem<VideoCodec>(VideoCodec.Any, Translator._("Any"), _configuration.DownloaderOptions.PreferredVideoCodec == VideoCodec.Any),
            new SelectionItem<VideoCodec>(VideoCodec.VP9, "VP9", _configuration.DownloaderOptions.PreferredVideoCodec == VideoCodec.VP9),
            new SelectionItem<VideoCodec>(VideoCodec.AV01, "AV1", _configuration.DownloaderOptions.PreferredVideoCodec == VideoCodec.AV01),
            new SelectionItem<VideoCodec>(VideoCodec.H264, Translator._("H.264 (AVC)"), _configuration.DownloaderOptions.PreferredVideoCodec == VideoCodec.H264),
            new SelectionItem<VideoCodec>(VideoCodec.H265, Translator._("H.265 (HEVC)"), _configuration.DownloaderOptions.PreferredVideoCodec == VideoCodec.H265)
        };
    }

    public bool AllowPreviewUpdates
    {
        get => _configuration.AllowPreviewUpdates;

        set => _configuration.AllowPreviewUpdates = value;
    }

    public int AriaMaxConnectionsPerServer
    {
        get => _configuration.AriaMaxConnectionsPerServer;

        set => _configuration.AriaMaxConnectionsPerServer = value;
    }

    public int AriaMinSplitSize
    {
        get => _configuration.AriaMinSplitSize;

        set => _configuration.AriaMinSplitSize = value;
    }

    public bool CropAudioThumbnails
    {
        get => _configuration.CropAudioThumbnails;

        set => _configuration.CropAudioThumbnails = value;
    }

    public SelectionItem<Browser> CookiesBrowser
    {
        set => _configuration.CookiesBrowser = value.Value;
    }

    public string CookiesPath
    {
        get => _configuration.CookiesPath;

        set => _configuration.CookiesPath = value;
    }

    public bool EmbedChapters
    {
        get => _configuration.EmbedChapters;

        set => _configuration.EmbedChapters = value;
    }

    public bool EmbedMetadata
    {
        get => _configuration.EmbedMetadata;

        set => _configuration.EmbedMetadata = value;
    }

    public bool EmbedSubtitles
    {
        get => _configuration.EmbedSubtitles;

        set => _configuration.EmbedSubtitles = value;
    }

    public bool EmbedThumbnails
    {
        get => _configuration.EmbedThumbnails;

        set => _configuration.EmbedThumbnails = value;
    }

    public SelectionItem<HistoryLength> HistoryLength
    {
        set => _historyService.Length = value.Value;
    }

    public bool IncludeAutoGeneratedSubtitles
    {
        get => _configuration.IncludeAutoGeneratedSubtitles;

        set => _configuration.IncludeAutoGeneratedSubtitles = value;
    }

    public bool IncludeMediaIdInTitle
    {
        get => _configuration.IncludeMediaIdInTitle;

        set => _configuration.IncludeMediaIdInTitle = value;
    }

    public bool LimitCharacters
    {
        get => _configuration.LimitCharacters;

        set => _configuration.LimitCharacters = value;
    }

    public int MaxNumberOfActiveDownloads
    {
        get => _configuration.MaxNumberOfActiveDownloads;

        set => _configuration.MaxNumberOfActiveDownloads = value;
    }

    public bool OverwriteExistingFiles
    {
        get => _configuration.OverwriteExistingFiles;

        set => _configuration.OverwriteExistingFiles = value;
    }

    public int PostprocessingThreads
    {
        get => _configuration.PostprocessingThreads;

        set => _configuration.PostprocessingThreads = value;
    }

    public SelectionItem<AudioCodec> PreferredAudioCodec
    {
        set => _configuration.PreferredAudioCodec = value.Value;
    }

    public SelectionItem<SubtitleFormat> PreferredSubtitleFormat
    {
        set => _configuration.PreferredSubtitleFormat = value.Value;
    }

    public SelectionItem<VideoCodec> PreferredVideoCodec
    {
        set => _configuration.PreferredVideoCodec = value.Value;
    }

    public bool PreventSuspend
    {
        get => _configuration.PreventSuspend;

        set => _configuration.PreventSuspend = value;
    }

    public string ProxyUrl
    {
        get => _configuration.ProxyUrl;

        set => _configuration.ProxyUrl = value;
    }

    public bool RemoveSourceData
    {
        get => _configuration.RemoveSourceData;

        set => _configuration.RemoveSourceData = value;
    }

    public bool ShowDislcaimerOnStartup
    {
        get => _configuration.ShowDislcaimerOnStartup;

        set => _configuration.ShowDislcaimerOnStartup = value;
    }

    public int? SpeedLimit
    {
        get => _configuration.SpeedLimit;

        set => _configuration.SpeedLimit = value;
    }

    public SelectionItem<Theme> Theme
    {
        set => _configuration.Theme = value.Value;
    }

    public SelectionItem<string> TranslationLanguage
    {
        set => _configuration.TranslationLanguage = value.Value;
    }

    public bool UseAria
    {
        get => _configuration.UseAria;

        set => _configuration.UseAria = value;
    }

    public bool UsePartFiles
    {
        get => _configuration.UsePartFiles;

        set => _configuration.UsePartFiles = value;
    }

    public bool YouTubeSponsorBlock
    {
        get => _configuration.YouTubeSponsorBlock;

        set => _configuration.YouTubeSponsorBlock = value;
    }

    public string YtdlpDiscoveryArgs
    {
        get => _configuration.YtdlpDiscoveryArgs;

        set => _configuration.YtdlpDiscoveryArgs = value;
    }

    public string YtdlpDownloadArgs
    {
        get => _configuration.YtdlpDownloadArgs;

        set => _configuration.YtdlpDownloadArgs = value;
    }

    public async Task<string?> AddPostprocessingArgumentAsync(string name, SelectionItem<PostProcessor> selectedPostProcessor, SelectionItem<Executable> selectedExecutable, string arguments)
    {
        if (_configuration.PostprocessingArguments.Any(arg => arg.Name == name))
        {
            return Translator._("An argument with that name already exists");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return Translator._("The name of the argument cannot be empty");
        }
        else if (string.IsNullOrEmpty(arguments))
        {
            return Translator._("The arguments of the argument cannot be empty");
        }
        var argument = new PostProcessorArgument(name, selectedPostProcessor.Value, selectedExecutable.Value, arguments);
        PostprocessingArguments.Add(argument);
        _configuration.PostprocessingArguments.Add(argument);
        await SaveConfigurationAsync();
        return null;
    }

    public async Task DeletePostprocessingArgumentAsync(string name)
    {
        var argument = _configuration.PostprocessingArguments.FirstOrDefault(arg => arg.Name == name);
        if (argument is null)
        {
            return;
        }
        PostprocessingArguments.Remove(argument);
        _configuration.PostprocessingArguments.Remove(argument);
        await SaveConfigurationAsync();
    }

    public async Task<string?> UpdatePostprocessingArgumentAsync(string name, SelectionItem<PostProcessor> selectedPostProcessor, SelectionItem<Executable> selectedExecutable, string arguments)
    {
        var index = _configuration.PostprocessingArguments.FindIndex(arg => arg.Name == name);
        if (index == -1)
        {
            return Translator._("An argument with that name does not exist");
        }
        else if (string.IsNullOrEmpty(name))
        {
            return Translator._("The name of the argument cannot be empty");
        }
        else if (string.IsNullOrEmpty(arguments))
        {
            return Translator._("The arguments of the argument cannot be empty");
        }
        var argument = new PostProcessorArgument(name, selectedPostProcessor.Value, selectedExecutable.Value, arguments);
        PostprocessingArguments[index] = argument;
        _configuration.PostprocessingArguments[index] = argument;
        await SaveConfigurationAsync();
        return null;
    }

    public async Task SaveConfigurationAsync() => await _jsonFileService.SaveAsync(_configuration, Configuration.Key);
}
