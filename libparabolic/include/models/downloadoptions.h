#ifndef DOWNLOADOPTIONS_H
#define DOWNLOADOPTIONS_H

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include <libnick/keyring/credential.h>
#include "downloaderoptions.h"
#include "format.h"
#include "mediafiletype.h"
#include "postprocessorargument.h"
#include "subtitlelanguage.h"
#include "timeframe.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of options for a Download.
     */
    class DownloadOptions
    {
    public:
        /**
         * @brief Construct a DownloadOptions.
         */
        DownloadOptions();
        /**
         * @brief Construct a DownloadOptions.
         * @param url The URL of the download
         */
        DownloadOptions(const std::string& url);
        /**
         * @brief Construct a DownloadOptions.
         * @param json The JSON object to construct the DownloadOptions from
         */
        DownloadOptions(boost::json::object json);
        /**
         * @brief Gets the URL of the download.
         * @return The URL of the download
         */
        const std::string& getUrl() const;
        /**
         * @brief Sets the URL of the download.
         * @param url The URL of the download
         */
        void setUrl(const std::string& url);
        /**
         * @brief Gets the credential for the download.
         * @return The credential for the download
         */
        const std::optional<Keyring::Credential>& getCredential() const;
        /**
         * @brief Sets the credential for the download.
         * @param credential The credential for the download
         */
        void setCredential(const std::optional<Keyring::Credential>& credential);
        /**
         * @brief Gets the media file type of the download.
         * @return The media file type of the download
         */
        const MediaFileType& getFileType() const;
        /**
         * @brief Sets the media file type of the download.
         * @param fileType The media file type of the download
         */
        void setFileType(const MediaFileType& fileType);
        /**
         * @brief Gets the available formats of the download.
         * @return The available formats of the download
         */
        const std::vector<Format>& getAvailableFormats() const;
        /**
         * @brief Sets the available formats of the download.
         * @param availableFormats The available formats of the download
         */
        void setAvailableFormats(const std::vector<Format>& availableFormats);
        /**
         * @brief Gets the video format of the download.
         * @return The video format of the download
         */
        const std::optional<Format>& getVideoFormat() const;
        /**
         * @brief Sets the video format of the download.
         * @param videoFormat The video format of the download
         */
        void setVideoFormat(const std::optional<Format>& videoFormat);
        /**
         * @brief Gets the audio format of the download.
         * @return The audio format of the download
         */
        const std::optional<Format>& getAudioFormat() const;
        /**
         * @brief Sets the audio format of the download.
         * @param audioFormat The audio format of the download
         */
        void setAudioFormat(const std::optional<Format>& audioFormat);
        /**
         * @brief Gets the save folder of the download.
         * @return The save folder of the download
         */
        const std::filesystem::path& getSaveFolder() const;
        /**
         * @brief Sets the save folder of the download.
         * @param saveFolder The save folder of the download
         */
        void setSaveFolder(const std::filesystem::path& saveFolder);
        /**
         * @brief Gets the save filename of the download.
         * @return The save filename of the download
         */
        const std::string& getSaveFilename() const;
        /**
         * @brief Sets the save filename of the download.
         * @param saveFilename The save filename of the download
         */
        void setSaveFilename(const std::string& saveFilename);
        /**
         * @brief Gets the list of subtitle languages to download.
         * @return The list of subtitle languages to download
         */
        const std::vector<SubtitleLanguage>& getSubtitleLanguages() const;
        /**
         * @brief Sets the list of subtitle languages to download.
         * @param downloadSubtitles The list of subtitle languages to download
         */
        void setSubtitleLanguages(const std::vector<SubtitleLanguage>& subtitleLanguages);
        /**
         * @brief Gets whether or not to split chapters.
         * @return True if splitting chapters, else false
         */
        bool getSplitChapters() const;
        /**
         * @brief Sets whether or not to split chapters.
         * @param splitChapters True if splitting chapters, else false
         */
        void setSplitChapters(bool splitChapters);
        /**
         * @brief Gets whether or not to export the media description to a file.
         * @return True to export the media description, else false
         */
        bool getExportDescription() const;
        /**
         * @brief Sets whether or not to export the media description to a file.
         * @param exportDescription True to export the media description, else false
         */
        void setExportDescription(bool exportDescription);
        /**
         * @brief Gets the post processor argument for the download.
         * @return The post processor argument for the download
         */
        const std::optional<PostProcessorArgument>& getPostProcessorArgument() const;
        /**
         * @brief Sets the post processor argument for the download.
         * @param postProcessorArgument The post processor argument for the download
         */
        void setPostProcessorArgument(const std::optional<PostProcessorArgument>& postProcessorArgument);
        /**
         * @brief Gets the time frame of the download.
         * @return The time frame of the download
         */
        const std::optional<TimeFrame>& getTimeFrame() const;
        /**
         * @brief Sets the time frame of the download.
         * @brief Can only be set to a value if limit speed is false.
         * @param timeFrame The time frame of the download
         */
        void setTimeFrame(const std::optional<TimeFrame>& timeFrame);
        /**
         * @brief Gets the playlist position of the download.
         * @return The playlist position of the download
         * @return -1 if the download is not part of a playlist
         */
        int getPlaylistPosition() const;
        /**
         * @brief Sets the playlist position of the download.
         * @param position The playlist position of the download
         */
        void setPlaylistPosition(int position);
        /**
         * @brief Converts the DownloadOptions to a vector of yt-dlp arguments.
         * @param downloaderOptions The DownloaderOptions to include in the arguments
         * @return The vector of yt-dlp arguments
         */
        std::vector<std::string> toArgumentVector(const DownloaderOptions& downloaderOptions) const;
        /**
         * @brief Converts the DownloadOptions to a JSON object.
         * @param includeCredential Whether or not to include the credential in the JSON object
         * @return The JSON object
         */
        boost::json::object toJson(bool includeCredential = true) const;

    private:
        /**
         * @brief Ensures that the save filename does not conflict with the selected file type (i.e. the save filename does not contain the file extension of the type selected).
         * @brief Ensures that the save filename and save folder path lengths are within the limits of the operating system. If the lengths are not within the limits, they will be truncated.
         */
        void validateFileNamesAndPaths();
        /**
         * @brief Gets whether or not the download should resume.
         * @brief Checks for existing part files in the save folder.
         * @return True if the download should resume, else false
         */
        bool shouldDownloadResume() const;
        std::string m_url;
        std::optional<Keyring::Credential> m_credential;
        MediaFileType m_fileType;
        std::vector<Format> m_availableFormats;
        std::optional<Format> m_videoFormat;
        std::optional<Format> m_audioFormat;
        std::filesystem::path m_saveFolder;
        std::string m_saveFilename;
        std::vector<SubtitleLanguage> m_subtitleLanguages;
        bool m_splitChapters;
        bool m_exportDescription;
        std::optional<PostProcessorArgument> m_postProcessorArgument;
        std::optional<TimeFrame> m_timeFrame;
        int m_playlistPosition;
    };
}

#endif //DOWNLOADOPTIONS_H
