#include "controllers/preferencesviewcontroller.h"

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    PreferencesViewController::PreferencesViewController(Configuration& configuration)
        : m_configuration{ configuration }
    {

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
        return m_configuration.getTheme();
    }

    void PreferencesViewController::setTheme(Theme theme)
    {
        m_configuration.setTheme(theme);
    }

    bool PreferencesViewController::getAutomaticallyCheckForUpdates() const
    {
        return m_configuration.getAutomaticallyCheckForUpdates();
    }

    void PreferencesViewController::setAutomaticallyCheckForUpdates(bool check)
    {
        m_configuration.setAutomaticallyCheckForUpdates(check);
    }

    CompletedNotificationPreference PreferencesViewController::getCompletedNotificationPreference() const
    {
        return m_configuration.getCompletedNotificationPreference();
    }

    void PreferencesViewController::setCompletedNotificationPreference(CompletedNotificationPreference preference)
    {
        m_configuration.setCompletedNotificationPreference(preference);
    }

    bool PreferencesViewController::getPreventSuspendWhenDownloading() const
    {
        return m_configuration.getPreventSuspendWhenDownloading();
    }

    void PreferencesViewController::setPreventSuspendWhenDownloading(bool prevent)
    {
        m_configuration.setPreventSuspendWhenDownloading(prevent);
    }

    bool PreferencesViewController::getDisallowConversions() const
    {
        return m_configuration.getDisallowConversions();
    }

    void PreferencesViewController::setDisallowConversions(bool disallowConversions)
    {
        m_configuration.setDisallowConversions(disallowConversions);
    }

    DownloaderOptions PreferencesViewController::getDownloaderOptions() const
    {
        return m_configuration.getDownloaderOptions();
    }

    void PreferencesViewController::setDownloaderOptions(const DownloaderOptions& options)
    {
        m_configuration.setDownloaderOptions(options);
    }

    void PreferencesViewController::saveConfiguration()
    {
        m_configuration.save();
    }
}