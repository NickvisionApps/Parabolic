using Nickvision.Desktop.Filesystem;
using Nickvision.Desktop.Globalization;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.Shared.Controllers;

public class PreferencesViewController
{
    private IJsonFileService _jsonFileService;
    private Configuration _configuration;

    public ITranslationService Translator { get; }
    public List<string> AvailableTranslationLanguages { get; }

    public PreferencesViewController(IJsonFileService jsonFileService, ITranslationService translationService)
    {
        _jsonFileService = jsonFileService;
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

    public async Task SaveConfigurationAsync() => await _jsonFileService.SaveAsync(_configuration, Configuration.Key);
}
