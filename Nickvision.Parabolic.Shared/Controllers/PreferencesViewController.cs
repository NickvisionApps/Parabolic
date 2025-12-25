using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.Shared.Models;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class PreferencesViewController
{
    private IJsonFileService _jsonFileService;
    private IHistoryService _historyService;
    private Configuration _configuration;

    public ITranslationService Translator { get; }
    public List<string> AvailableTranslationLanguages { get; }

    public PreferencesViewController(IJsonFileService jsonFileService, ITranslationService translationService, IHistoryService historyService)
    {
        _jsonFileService = jsonFileService;
        _historyService = historyService;
        _configuration = _jsonFileService.Load<Configuration>(Configuration.Key);
        Translator = translationService;
        AvailableTranslationLanguages = Translator.AvailableLanguages.ToList();
        AvailableTranslationLanguages.Sort();
        AvailableTranslationLanguages.Insert(0, "en_US");
        AvailableTranslationLanguages.Insert(0, Translator._("System"));
    }

    public Theme Theme
    {
        get => _configuration.Theme;

        set => _configuration.Theme = value;
    }

    public string TranslationLanguage
    {
        get
        {
            if (string.IsNullOrEmpty(_configuration.TranslationLanguage))
            {
                return Translator._("System");
            }
            else if (_configuration.TranslationLanguage == "C")
            {
                return "en_US";
            }
            else
            {
                return _configuration.TranslationLanguage;
            }
        }

        set
        {
            _configuration.TranslationLanguage = value switch
            {
                _ when value == Translator._("System") => string.Empty,
                "en_US" => "C",
                _ => value
            };
        }
    }

    public bool AllowPreviewUpdates
    {
        get => _configuration.AllowPreviewUpdates;

        set => _configuration.AllowPreviewUpdates = value;
    }

    public bool PreventSuspend
    {
        get => _configuration.PreventSuspend;

        set => _configuration.PreventSuspend = value;
    }

    public DownloaderOptions DownloaderOptions
    {
        get => _configuration.DownloaderOptions;

        set => _configuration.DownloaderOptions = value;
    }

    public HistoryLength HistoryLength
    {
        get => _historyService.Length;

        set => _historyService.Length = value;
    }

    public IEnumerable<string> ExecutableStrings
    {
        get
        {
            var strings = new List<string>();
            strings.Add(Translator._("None"));
            foreach (var value in Enum.GetValues(typeof(Executable)))
            {
                var executableValue = (Executable)value;
                if (executableValue == Executable.None)
                {
                    continue;
                }
                strings.Add(executableValue.ToYtdlpString());
            }
            return strings;
        }
    }

    public IEnumerable<string> PostProcessorStrings
    {
        get
        {
            var strings = new List<string>();
            strings.Add(Translator._("None"));
            foreach(var value in Enum.GetValues(typeof(PostProcessor)))
            {
                var ppValue = (PostProcessor)value;
                if(ppValue == PostProcessor.None)
                {
                    continue;
                }
                strings.Add(ppValue.ToYtdlpString());
            }
            return strings;
        }
    }

    public async Task SaveConfigurationAsync() => await _jsonFileService.SaveAsync(_configuration, Configuration.Key);
}
