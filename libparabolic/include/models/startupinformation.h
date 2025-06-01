#ifndef STARTUPINFORMATION_H
#define STARTUPINFORMATION_H

#include <string>
#include <libnick/app/windowgeometry.h>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model for the startup information of the application.
     */
    class StartupInformation
    {
    public:
        /**
         * @brief Constructs a StartupInformation.
         */
        StartupInformation();
        /**
         * @brief Constructs a StartupInformation.
         * @param windowGeometry The window geometry
         * @param canDownload Whether or not the application can perform downloads
         * @param showDisclaimer Whether or not to show the legal disclaimer on startup
         * @param urlToValidate The URL to validate
         * @param recover Whether or not there are downloads to recover.
         */
        StartupInformation(const Nickvision::App::WindowGeometry& windowGeometry, bool canDownload, bool showDisclaimer, const std::string& urlToValidate, bool recover);
        /**
         * @brief Gets the window geometry.
         * @return The window geometry
         */
        const Nickvision::App::WindowGeometry& getWindowGeometry() const;
        /**
         * @brief Sets the window geometry.
         * @param windowGeometry The window geometry to set
         */
        void setWindowGeometry(const Nickvision::App::WindowGeometry& windowGeometry);
        /**
         * @brief Gets whether or not the application can perform downloads.
         * @return True if the application can perform downloads, false otherwise
         */
        bool canDownload() const;
        /**
         * @brief Sets whether or not the application can perform downloads.
         * @param canDownload True if the application can perform downloads, false otherwise
         */
        void setCanDownload(bool canDownload);
        /**
         * @brief Gets whether or not to show the legal disclaimer on startup.
         * @return True if the legal disclaimer should be shown, false otherwise
         */
        bool showDisclaimer() const;
        /**
         * @brief Sets whether or not to show the legal disclaimer on startup.
         * @param showDisclaimer True if the legal disclaimer should be shown, false otherwise
         */
        void setShowDisclaimer(bool showDisclaimer);
        /**
         * @brief Gets the URL to validate on startup.
         * @return The URL to validate
         */
        const std::string& getUrlToValidate() const;
        /**
         * @brief Sets the URL to validate on startup.
         * @param urlToValidate The URL to validate
         */
        void setUrlToValidate(const std::string& urlToValidate);
        /**
         * @brief Gets whether or not there are downloads to recover.
         * @return True if there are downloads to recover, else false
         */
        bool hasRecoverableDownloads() const;
        /**
         * @brief Sets whether or not there are downloads to recover.
         * @param recover True if there are downloads to recover, else false
         */
        void setHasRecoverableDownloads(bool recover);

    private:
        Nickvision::App::WindowGeometry m_windowGeometry;
        bool m_canDownload;
        bool m_showDisclaimer;
        std::string m_urlToValidate;
        bool m_hasRecoverableDownloads;
    };
}

#endif //STARTUPINFORMATION_H
