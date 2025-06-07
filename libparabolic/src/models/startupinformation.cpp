#include "models/startupinformation.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    StartupInformation::StartupInformation()
        : m_canDownload{ false },
        m_showDisclaimer{ false },
        m_hasRecoverableDownloads{ false }
    {

    }

    StartupInformation::StartupInformation(const Nickvision::App::WindowGeometry& windowGeometry, bool canDownload, bool showDisclaimer, const std::string& urlToValidate, bool recover)
        : m_windowGeometry{ windowGeometry },
        m_canDownload{ canDownload },
        m_showDisclaimer{ showDisclaimer },
        m_urlToValidate{ urlToValidate },
        m_hasRecoverableDownloads{ recover }
    {

    }

    const Nickvision::App::WindowGeometry& StartupInformation::getWindowGeometry() const
    {
        return m_windowGeometry;
    }

    void StartupInformation::setWindowGeometry(const Nickvision::App::WindowGeometry& windowGeometry)
    {
        m_windowGeometry = windowGeometry;
    }

    bool StartupInformation::canDownload() const
    {
        return m_canDownload;
    }

    void StartupInformation::setCanDownload(bool canDownload)
    {
        m_canDownload = canDownload;
    }

    bool StartupInformation::showDisclaimer() const
    {
        return m_showDisclaimer;
    }

    void StartupInformation::setShowDisclaimer(bool showDisclaimer)
    {
        m_showDisclaimer = showDisclaimer;
    }

    const std::string& StartupInformation::getUrlToValidate() const
    {
        return m_urlToValidate;
    }

    void StartupInformation::setUrlToValidate(const std::string& urlToValidate)
    {
        m_urlToValidate = urlToValidate;
    }

    bool StartupInformation::hasRecoverableDownloads() const
    {
        return m_hasRecoverableDownloads;
    }

    void StartupInformation::setHasRecoverableDownloads(bool recover)
    {
        m_hasRecoverableDownloads = recover;
    }
}
