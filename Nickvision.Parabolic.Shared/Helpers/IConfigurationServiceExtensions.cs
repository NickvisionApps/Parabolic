using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Models;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class IConfigurationServiceExtensions
{
    extension(IConfigurationService configurationService)
    {
        public bool AllowPreviewUpdates
        {
            get => configurationService.Get("AllowPreviewUpdates", false);

            set => configurationService.Set("AllowPreviewUpdates", value);
        }

        public bool ShowDisclaimerOnStartup
        {
            get => configurationService.Get("ShowDisclaimerOnStartup", true);

            set => configurationService.Set("ShowDisclaimerOnStartup", value);
        }

        public Theme Theme
        {
            get => (Theme)configurationService.Get("Theme", 2);

            set => configurationService.Set("Theme", (int)value);
        }

        public string TranslationLanguage
        {
            get => configurationService.Get("TranslationLanguage", string.Empty);

            set => configurationService.Set("TranslationLanguage", value);
        }

        public WindowGeometry WindowGeometry
        {
            get => configurationService.Get("WindowGeometry", new WindowGeometry(), ApplicationJsonContext.Default.WindowGeometry);

            set => configurationService.Set("WindowGeometry", value, ApplicationJsonContext.Default.WindowGeometry);
        }
    }
}
