#include "controllers/preferencesviewcontroller.h"
#include <thread>

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    PreferencesViewController::PreferencesViewController(Configuration& configuration, Models::DownloadHistory& downloadHistory)
        : m_configuration{ configuration },
        m_downloadHistory{ downloadHistory }
    {

    }

    int PreferencesViewController::getMaxPostprocessingThreads() const
    {
        return static_cast<int>(std::thread::hardware_concurrency());
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

    bool PreferencesViewController::getPreventSuspend() const
    {
        return m_configuration.getPreventSuspend();
    }

    void PreferencesViewController::setPreventSuspend(bool prevent)
    {
        m_configuration.setPreventSuspend(prevent);
    }

    bool PreferencesViewController::getRecoverCrashedDownloads() const
    {
        return m_configuration.getRecoverCrashedDownloads();
    }

    void PreferencesViewController::setRecoverCrashedDownloads(bool recover)
    {
        m_configuration.setRecoverCrashedDownloads(recover);
    }

    DownloaderOptions PreferencesViewController::getDownloaderOptions() const
    {
        return m_configuration.getDownloaderOptions();
    }

    void PreferencesViewController::setDownloaderOptions(const DownloaderOptions& options)
    {
        m_configuration.setDownloaderOptions(options);
    }

    size_t PreferencesViewController::getHistoryLengthIndex() const
    {
        switch(m_downloadHistory.getLength())
        {
        case HistoryLength::Never:
            return 0;
        case HistoryLength::OneDay:
            return 1;
        case HistoryLength::OneWeek:
            return 2;
        case HistoryLength::OneMonth:
            return 3;
        case HistoryLength::ThreeMonths:
            return 4;
        case HistoryLength::Forever:
            return 5;
        default:
            return 2;
        }
    }

    void PreferencesViewController::setHistoryLengthIndex(size_t length)
    {
        switch(length)
        {
        case 0:
            m_downloadHistory.setLength(HistoryLength::Never);
            break;
        case 1:
            m_downloadHistory.setLength(HistoryLength::OneDay);
            break;
        case 2:
            m_downloadHistory.setLength(HistoryLength::OneWeek);
            break;
        case 3:
            m_downloadHistory.setLength(HistoryLength::OneMonth);
            break;
        case 4:
            m_downloadHistory.setLength(HistoryLength::ThreeMonths);
            break;
        case 5:
            m_downloadHistory.setLength(HistoryLength::Forever);
            break;
        default:
            m_downloadHistory.setLength(HistoryLength::OneWeek);
            break;
        }
    }

    bool PreferencesViewController::getDownloadImmediatelyAfterValidation() const
    {
        return m_configuration.getDownloadImmediatelyAfterValidation();
    }

    void PreferencesViewController::setDownloadImmediatelyAfterValidation(bool downloadImmediatelyAfterValidation)
    {
        m_configuration.setDownloadImmediatelyAfterValidation(downloadImmediatelyAfterValidation);
    }

    void PreferencesViewController::saveConfiguration()
    {
        m_configuration.save();
    }
}
