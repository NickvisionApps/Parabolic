#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include <optional>
#include <string>
#include <vector>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/keyring/keyring.h>
#include "models/downloaderoptions.h"
#include "models/timeframe.h"
#include "models/urlinfo.h"
#include "models/previousdownloadoptions.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for the AddDownloadDialog.
     */
    class AddDownloadDialogController
    {
    public:
        /**
         * @brief Constructs an AddDownloadDialogController.
         * @param downloaderOptions The DownloaderOptions from the configuration
         * @param keyring The Keyring to use
         * @param configuration The Configuration to use
         */
        AddDownloadDialogController(const Models::DownloaderOptions& downloaderOptions, Models::PreviousDownloadOptions& previousOptions, std::optional<Keyring::Keyring>& keyring);
        /**
         * @brief Destructs the AddDownloadDialogController.
         */
        ~AddDownloadDialogController();
        /**
         * @brief Gets the event for when a url is validated.
         * @return The url validated event
         */
        Events::Event<Events::ParamEventArgs<bool>>& urlValidated();
        /**
         * @brief Gets the PreviousDownloadOptions.
         * @return The PreviousDownloadOptions
         */
        const Models::PreviousDownloadOptions& getPreviousDownloadOptions() const;
        /**
         * @briefs Gets the list of names of credentials in the keyring.
         * @return The list of credential names in the keyring
         */
        std::vector<std::string> getKeyringCredentialNames() const;
        /**
         * @brief Gets whether or not a valid url has been validated.
         * @return True if valid url, else false
         */
        bool isUrlValid() const;
        /**
         * @brief Gets whether or not the url is a playlist.
         * @return True if playlist, else false
         */
        bool isUrlPlaylist() const;
        /**
         * @brief Gets the count of media found at the url.
         * @return The count of media found at the url
         */
        size_t getMediaCount() const;
        /**
         * @brief Gets the list of file types as strings.
         * @return The list of file types as strings
         */
        std::vector<std::string> getFileTypeStrings() const;
        /**
         * @brief Gets the list of qualities as strings.
         * @param index The index of the selected file type
         * @return The list of qualities as strings
         */
        std::vector<std::string> getQualityStrings(size_t index) const;
        /**
         * @brief Gets the list of audio languages as strings.
         * @return The list of audio languages as strings
         */
        std::vector<std::string> getAudioLanguageStrings() const;
        /**
         * @brief Gets the title for the media at the specified index.
         * @param index The index of the media
         * @return The title of the media
         */
        const std::string& getMediaTitle(size_t index) const;
        /**
         * @brief Gets the TimeFrame for the media at the specified index.
         * @param index The index of the media
         * @return The TimeFrame of the media
         */
        const Models::TimeFrame& getMediaTimeFrame(size_t index) const;
        /**
         * @brief Validates a url.
         * @brief This method will invoke the urlValidated event with the list of media found at the url.
         * @param url The url to validate
         * @param credential An optional credential to use when accessing the url
         */
        void validateUrl(const std::string& url, const std::optional<Keyring::Credential>& credential);
        /**
         * @brief Validates a url.
         * @brief This method will invoke the urlValidated event with the list of media found at the url.
         * @param url The url to validate
         * @param credentialNameIndex The index of the name of the credential to use when accessing the url
         */
        void validateUrl(const std::string& url, size_t credentialNameIndex);

    private:
        Models::DownloaderOptions m_downloaderOptions;
        Models::PreviousDownloadOptions& m_previousOptions;
        std::optional<Keyring::Keyring>& m_keyring;
        std::optional<Models::UrlInfo> m_urlInfo;
        Events::Event<Events::ParamEventArgs<bool>> m_urlValidated;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H