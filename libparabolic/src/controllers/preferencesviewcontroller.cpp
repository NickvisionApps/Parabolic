#include "controllers/preferencesviewcontroller.h"
#include <libnick/app/aura.h>
#include "models/configuration.h"

using namespace Nickvision::App;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    const std::string& PreferencesViewController::getId() const
    {
        return Aura::getActive().getAppInfo().getId();
    }

    const std::string& PreferencesViewController::getCookiesExtensionUrl(Browsers browser) const
    {
        static std::string none{ "" };
        static std::string chrome{ "https://chrome.google.com/webstore/detail/get-cookiestxt-locally/cclelndahbckbenkjhflpdbgdldlbecc" };
        static std::string firefox{ "https://addons.mozilla.org/en-US/firefox/addon/cookies-txt/" };
        switch(browser)
        {
        case Browsers::Chrome:
            return chrome;
        case Browsers::Firefox:
            return firefox;
        default:
            return none;
        }
    }

    Theme PreferencesViewController::getTheme() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getTheme();
    }

    void PreferencesViewController::setTheme(Theme theme)
    {
        Aura::getActive().getConfig<Configuration>("config").setTheme(theme);
    }

    bool PreferencesViewController::getAutomaticallyCheckForUpdates() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getAutomaticallyCheckForUpdates();
    }

    void PreferencesViewController::setAutomaticallyCheckForUpdates(bool check)
    {
        Aura::getActive().getConfig<Configuration>("config").setAutomaticallyCheckForUpdates(check);
    }

    CompletedNotificationPreference PreferencesViewController::getCompletedNotificationPreference() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getCompletedNotificationPreference();
    }

    void PreferencesViewController::setCompletedNotificationPreference(CompletedNotificationPreference preference)
    {
        Aura::getActive().getConfig<Configuration>("config").setCompletedNotificationPreference(preference);
    }

    bool PreferencesViewController::getPreventSuspendWhenDownloading() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getPreventSuspendWhenDownloading();
    }

    void PreferencesViewController::setPreventSuspendWhenDownloading(bool prevent)
    {
        Aura::getActive().getConfig<Configuration>("config").setPreventSuspendWhenDownloading(prevent);
    }

    bool PreferencesViewController::getDisallowConversions() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getDisallowConversions();
    }

    void PreferencesViewController::setDisallowConversions(bool disallowConversions)
    {
        Aura::getActive().getConfig<Configuration>("config").setDisallowConversions(disallowConversions);
    }

    DownloaderOptions PreferencesViewController::getDownloaderOptions() const
    {
        return Aura::getActive().getConfig<Configuration>("config").getDownloaderOptions();
    }

    void PreferencesViewController::setDownloaderOptions(const DownloaderOptions& options)
    {
        Aura::getActive().getConfig<Configuration>("config").setDownloaderOptions(options);
    }

    void PreferencesViewController::saveConfiguration()
    {
        Aura::getActive().getLogger().log(Logging::LogLevel::Debug, "Config saved.");
        Aura::getActive().getConfig<Configuration>("config").save();
    }
}