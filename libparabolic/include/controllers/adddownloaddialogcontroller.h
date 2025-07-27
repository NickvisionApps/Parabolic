#ifndef ADDDOWNLOADDIALOGCONTROLLER_H
#define ADDDOWNLOADDIALOGCONTROLLER_H

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>
#include <libnick/app/cancellationtoken.h>
#include <libnick/app/datafilemanager.h>
#include <libnick/events/event.h>
#include <libnick/events/parameventargs.h>
#include <libnick/keyring/keyring.h>
#include "models/downloadmanager.h"
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
         * @brief Gets the list of video formats as strings.
         * @param type The selected MediaFileType
         * @param previousIndex The out parameter that will contain the previous selected video format index if available, else index will be 0
         * @return The list of video formats as strings
         */
        std::vector<std::string> getVideoFormatStrings(const Models::MediaFileType& type, size_t& previousIndex) const;
        /**
         * @brief Gets the list of audio formats as strings.
         * @param type The selected MediaFileType
         * @param previousIndex The out parameter that will contain the previous selected audio format index if available, else index will be 0
         * @return The list of audio formats as strings
         */
        std::vector<std::string> getAudioFormatStrings(const Models::MediaFileType& type, size_t& previousIndex) const;
        /**
         * @brief Gets the list of subtitles languages as strings.
         * @return The list of subtitles languages as strings
         */
        std::vector<std::string> getSubtitleLanguageStrings() const;
        /**
         * @brief Gets the list of postprocessing arguments as strings.
         * @return The list of postprocessing arguments as strings
         */
        std::vector<std::string> getPostprocessingArgumentNames() const;
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
         * @brief Sets the previous number titles option.
         * @param number True to number titles, else false
         */
        void setPreviousNumberTitles(bool number);
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
         * @param videoFormatIndex The index of the selected video format
         * @param audioFormatIndex The index of the selected audio format
         * @param subtitleLanguages The list of selected subtitle languages
         * @param splitChapters Whether or not to split the video by chapters
         * @param exportDescription Whether or not to export the media description to a file
         * @param excludeFromHistory Whether or not to exclude the download from the history
         * @param postProcessorArgumentIndex The index of the selected post processor argument
         * @param startTime The start time of the download
         * @param endTime The end time of the download
         */
        void addSingleDownload(const std::filesystem::path& saveFolder, const std::string& filename, size_t fileTypeIndex, size_t videoFormatIndex, size_t audioFormatIndex, const std::vector<std::string>& subtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, size_t postProcessorArgumentIndex, const std::string& startTime, const std::string& endTime);
        /**
         * @brief Adds a playlist download to the download manager.
         * @param saveFolder The folder to save the downloads to
         * @param filenames The filenames to save the downloads as with their respective indices (Excluded indices will not be downloaded)
         * @param fileTypeIndex The index of the selected file type
         * @param splitChapters Whether or not to split the video by chapters
         * @param exportDescription Whether or not to export the media description to a file
         * @param writePlaylistFile Whether or not to write a m3u playlist file
         * @param excludeFromHistory Whether or not to exclude the download from the history
         * @param postProcessorArgumentIndex The index of the selected post processor argument
         */
        void addPlaylistDownload(const std::filesystem::path& saveFolder, const std::map<size_t, std::string>& filenames, size_t fileTypeIndex, bool splitChapters, bool exportDescription, bool writePlaylistFile, bool excludeFromHistory, size_t postProcessorArgumentIndex);

    private:
        Models::DownloadManager& m_downloadManager;
        Models::PreviousDownloadOptions& m_previousOptions;
        Keyring::Keyring& m_keyring;
        App::CancellationToken m_validationCancellationToken;
        std::optional<Models::UrlInfo> m_urlInfo;
        std::optional<Keyring::Credential> m_credential;
        mutable std::unordered_map<size_t, size_t> m_videoFormatMap;
        mutable std::unordered_map<size_t, size_t> m_audioFormatMap;
        Nickvision::Events::Event<Nickvision::Events::ParamEventArgs<bool>> m_urlValidated;
    };
}

#endif //ADDDOWNLOADDIALOGCONTROLLER_H
