#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <libnick/app/datafilemanager.h>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/keyring/keyring.h>
#include "models/downloadmanager.h"
#include "models/format.h"
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
         * @param downloadManager The DownloadManager to use
         * @param dataFileManager The DataFileManager to use
         * @param keyring The Keyring to use
         */
        AddDownloadDialogController(Models::DownloadManager& downloadManager, App::DataFileManager& dataFileManager, Keyring::Keyring& keyring);
        /**
         * @brief Destructs the AddDownloadDialogController.
         */
        ~AddDownloadDialogController();
        /**
         * @brief Gets the event for when a url is validated.
         * @brief The boolean parameter is true if the url is valid, else false.
         * @return The url validated event
         */
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<bool>>& urlValidated();
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
         * @brief Gets whether or not to download immediately after validation.
         * @return True to download immediately after validation, else false
         */
        bool getDownloadImmediatelyAfterValidation() const;
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
         * @param fileTypeIndex The index of the selected file type
         * @return The list of qualities as strings
         */
        std::vector<std::string> getQualityStrings(size_t fileTypeIndex) const;
        /**
         * @brief Gets the list of audio languages as strings.
         * @return The list of audio languages as strings
         */
        std::vector<std::string> getAudioLanguageStrings() const;
        /**
         * @brief Gets the list of subtitles languages as strings.
         * @return The list of subtitles languages as strings
         */
        std::vector<std::string> getSubtitleLanguageStrings() const;
        /**
         * @brief Gets the url for the media at the specified index.
         * @param index The index of the media
         * @return The url of the media
         */
        const std::string& getMediaUrl(size_t index) const;
        /**
         * @brief Gets the title for the media at the specified index.
         * @param index The index of the media
         * @param numbered Whether or not to number the title
         * @return The title of the media
         */
        std::string getMediaTitle(size_t index, bool numbered = false) const;
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
        /**
         * @brief Validates a batch file.
         * @brief This method will invoke the urlValidated event with the list of media found from the batch file.
         * @param batchFile The path to the batch file to validate
         * @param credential An optional credential to use when accessing the urls in the batch file
         */
        void validateBatchFile(const std::filesystem::path& batchFile, const std::optional<Keyring::Credential>& credential);
        /**
         * @brief Validates a batch file.
         * @brief This method will invoke the urlValidated event with the list of media found from the batch file.
         * @param batchFile The path to the batch file to validate
         * @param credentialNameIndex The index of the name of the credential to use when accessing the urls in the batch file
         */
        void validateBatchFile(const std::filesystem::path& batchFile, size_t credentialNameIndex);
        /**
         * @brief Adds a single download to the download manager.
         * @param saveFolder The folder to save the download to
         * @param filename The filename to save the download as
         * @param fileTypeIndex The index of the selected file type
         * @param qualityIndex The index of the selected quality
         * @param audioLanguageIndex The index of the selected audio language
         * @param subtitleLanguages The list of selected subtitle languages
         * @param splitChapters Whether or not to split the video by chapters
         * @param limitSpeed Whether or not to limit the download speed
         * @param startTime The start time of the download
         * @param endTime The end time of the download
         */
        void addSingleDownload(const std::filesystem::path& saveFolder, const std::string& filename, size_t fileTypeIndex, size_t qualityIndex, size_t audioLanguageIndex, const std::vector<std::string>& subtitleLanguages, bool splitChapters, bool limitSpeed, const std::string& startTime, const std::string& endTime);
        /**
         * @brief Adds a playlist download to the download manager.
         * @param saveFolder The folder to save the downloads to
         * @param filenames The filenames to save the downloads as with their respective indices (Excluded indices will not be downloaded)
         * @param fileTypeIndex The index of the selected file type
         * @param splitChapters Whether or not to split the video by chapters
         * @param limitSpeed Whether or not to limit the download speed
         */
        void addPlaylistDownload(const std::filesystem::path& saveFolder, const std::unordered_map<size_t, std::string>& filenames, size_t fileTypeIndex, bool splitChapters, bool limitSpeed);

    private:
        Models::DownloadManager& m_downloadManager;
        Models::PreviousDownloadOptions& m_previousOptions;
        Keyring::Keyring& m_keyring;
        std::optional<Models::UrlInfo> m_urlInfo;
        std::optional<Keyring::Credential> m_credential;
        bool m_downloadImmediatelyAfterValidation;
        mutable std::unordered_map<size_t, Models::Format> m_qualityFormatMap;
        mutable std::unordered_map<size_t, Models::Format> m_audioLanguageFormatMap;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<bool>> m_urlValidated;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H