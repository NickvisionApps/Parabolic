#ifndef DOWNLOADOPTIONS_H
#define DOWNLOADOPTIONS_H

#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <libnick/keyring/credential.h>
#include "downloaderoptions.h"
#include "mediafiletype.h"
#include "quality.h"
#include "timeframe.h"
#include "videocodec.h"
#include "videoresolution.h"

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
         * @brief Gets the quality of the download.
         * @return The quality of the download
         */
        const std::variant<Quality, VideoResolution>& getQuality() const;
        /**
         * @brief Sets the quality of the download.
         * @param quality The quality of the download
         */
        void setQuality(const std::variant<Quality, VideoResolution>& quality);
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
         * @brief Gets the audio language of the download.
         * @return The audio language of the download
         */
        const std::string& getAudioLanguage() const;
        /**
         * @brief Sets the audio language of the download.
         * @param audioLanguage The audio language of the download
         */
        void setAudioLanguage(const std::string& audioLanguage);
        /**
         * @brief Gets whether or not to download subtitles.
         * @return True if downloading subtitles, else false
         */
        bool getDownloadSubtitles() const;
        /**
         * @brief Sets whether or not to download subtitles.
         * @param downloadSubtitles True if downloading subtitles, else false
         */
        void setDownloadSubtitles(bool downloadSubtitles);
        /**
         * @brief Gets whether or not to limit the download speed.
         * @return True if limiting the download speed, else false
         */
        bool getLimitSpeed() const;
        /**
         * @brief Sets whether or not to limit the download speed.
         * @brief Can only be set to true if no time frame was specified.
         * @param limitSpeed True if limiting the download speed, else false
         */
        void setLimitSpeed(bool limitSpeed);
        /**
         * @brief Gets the video codec to prefer.
         * @return The video codec to prefer
         */
        VideoCodec getVideoCodec() const;
        /**
         * @brief Sets the video codec to prefer.
         * @param codec The video codec to prefer
         */
        void setVideoCodec(VideoCodec codec);
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
         * @brief Converts the DownloadOptions to a vector of yt-dlp arguments.
         * @param downloaderOptions The DownloaderOptions to include in the arguments
         * @return The vector of yt-dlp arguments
         */
        std::vector<std::string> toArgumentVector(const DownloaderOptions& downloaderOptions) const;

    private:
        std::string m_url;
        std::optional<Keyring::Credential> m_credential;
        MediaFileType m_fileType;
        std::variant<Quality, VideoResolution> m_quality;
        std::filesystem::path m_saveFolder;
        std::string m_saveFilename;
        std::string m_audioLanguage;
        bool m_downloadSubtitles;
        bool m_limitSpeed;
        VideoCodec m_videoCodec;
        bool m_splitChapters;
        std::optional<TimeFrame> m_timeFrame;
    };
}

#endif //DOWNLOADOPTIONS_H