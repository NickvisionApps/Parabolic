#ifndef DOWNLOADEROPTIONS_H
#define DOWNLOADEROPTIONS_H

#include <filesystem>
#include <string>
#include <boost/json.hpp>
#include "browser.h"
#include "subtitleformat.h"
#include "videocodec.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of options for a Downloader.
     */
    class DownloaderOptions
    {
    public:
        /**
         * @brief Construct a DownloaderOptions.
         */
        DownloaderOptions();
        /**
         * @brief Gets whether or not to overwrite existing files.
         * @return True to overwrite existing files, else false
         */
        bool getOverwriteExistingFiles() const;
        /**
         * @brief Sets whether or not to overwrite existing files.
         * @param overwrite True to overwrite existing files, else false
         */
        void setOverwriteExistingFiles(bool overwrite);
        /**
         * @brief Gets the maximum number of active downloads.
         * @brief Should be between 1 and 10.
         * @return The maximum number of active downloads
         */
        int getMaxNumberOfActiveDownloads() const;
        /**
         * @brief Sets the maximum number of active downloads.
         * @param max The new maximum number of active downloads
         */
        void setMaxNumberOfActiveDownloads(int max);
        /**
         * @brief Gets whether or not to limit character to those supported by Windows only.
         * @return True to limit characters, else false
         */
        bool getLimitCharacters() const;
        /**
         * @brief Sets whether or not to limit character to those supported by Windows only.
         * @param limit True to limit characters, else false
         */
        void setLimitCharacters(bool limit);
        /**
         * @brief Gets whether or not to include auto-generated subtitles.
         * @return True to include auto-generated subtitles, else false
         */
        bool getIncludeAutoGeneratedSubtitles() const;
        /**
         * @brief Sets whether or not to include auto-generated subtitles.
         * @param include True to include auto-generated subtitles, else false
         */
        void setIncludeAutoGeneratedSubtitles(bool include);
        /**
         * @brief Gets the preferred video codec.
         * @return The preferred video codec
         */
        VideoCodec getPreferredVideoCodec() const;
        /**
         * @brief Sets the preferred video codec.
         * @param codec The new preferred video codec
         */
        void setPreferredVideoCodec(VideoCodec codec);
        /**
         * @brief Gets the preferred subtitle format.
         * @return The preferred subtitle format
         */
        SubtitleFormat getPreferredSubtitleFormat() const;
        /**
         * @brief Sets the preferred subtitle format.
         * @param format The new preferred subtitle format
         */
        void setPreferredSubtitleFormat(SubtitleFormat format);
        /**
         * @brief Gets whether or not to recover crashed downloads.
         * @return True to recover crashed downloads, else false
         */
        bool getRecoverCrashedDownloads() const;
        /**
         * @brief Sets whether or not to recover crashed downloads.
         * @param recoverCrashedDownloads True to recover crashed downloads, else false
         */
        void setRecoverCrashedDownloads(bool recoverCrashedDownloads);
        /**
         * @brief Gets whether or not to use aria2 for downloading.
         * @return True to use aria2, else false
         */
        bool getUseAria() const;
        /**
         * @brief Sets whether or not to use aria2 for downloading.
         * @param useAria True to use aria2, else false
         */
        void setUseAria(bool useAria);
        /**
         * @brief Gets the maximum number of connections per server for each aria2 download.
         * @brief This is equivalent to the -x flag in aria2.
         * @brief Should be between 1 and 16.
         * @return The maximum number of connections per server
         */
        int getAriaMaxConnectionsPerServer() const;
        /**
         * @brief Sets the maximum number of connections per server for each aria2 download.
         * @brief This is equivalent to the -x flag in aria2.
         * @brief Should be between 1 and 16.
         * @param maxConnections The new maximum number of connections per server
         */
        void setAriaMaxConnectionsPerServer(int maxConnections);
        /**
         * @brief Gets the minimum split size for each aria2 download.
         * @brief This is equivalent to the -k flag in aria2.
         * @brief Should be in MiB/s.
         * @brief Should be between 1 and 1024.
         * @return The minimum split size
         */
        int getAriaMinSplitSize() const;
        /**
         * @brief Sets the minimum split size for each aria2 download.
         * @brief This is equivalent to the -k flag in aria2.
         * @brief Should be in MiB/s.
         * @brief Should be between 1 and 1024.
         * @param minSplitSize The new minimum split size
         */
        void setAriaMinSplitSize(int minSplitSize);
        /**
         * @brief Gets whether or not to log verbose output.
         * @return True to log verbose output, else false
         */
        bool getVerboseLogging() const;
        /**
         * @brief Sets whether or not to log verbose output.
         * @param verbose True to log verbose output, else false
         */
        void setVerboseLogging(bool verbose);
        /**
         * @brief Gets the speed limit for each download.
         * @brief Should be in KiB/s.
         * @brief Should be between 512 and 10240.
         * @return The speed limit
         */
        int getSpeedLimit() const;
        /**
         * @brief Sets the speed limit for each download.
         * @brief Should be in KiB/s.
         * @brief Should be between 512 and 10240.
         * @param speedLimit The new speed limit
         */
        void setSpeedLimit(int speedLimit);
        /**
         * @brief Gets the proxy server url to use for downloading.
         * @return The proxy server url
         */
        const std::string& getProxyUrl() const;
        /**
         * @brief Sets the proxy server url to use for downloading.
         * @param proxyUrl The new proxy server url
         */
        void setProxyUrl(const std::string& proxyUrl);
        /**
         * @brief Gets the browser to fetch cookies from for the downloader.
         * @return The cookies browser
         */
        Browser getCookiesBrowser() const;
        /**
         * @brief Sets the browser to fetch cookies from for the downloader.
         * @param browser The new cookies browser
         */
        void setCookiesBrowser(Browser browser);
        /**
         * @brief Gets the path to the cookies file.
         * @return The path to the cookies file
         */
        const std::filesystem::path& getCookiesPath() const;
        /**
         * @brief Sets the path to the cookies file.
         * @param path The new path to the cookies file
         */
        void setCookiesPath(const std::filesystem::path& path);
        /**
         * @brief Gets whether or not to use the YouTube SponsorBlock extension.
         * @return True to use SponsorBlock, else false
         */
        bool getYouTubeSponsorBlock() const;
        /**
         * @brief Sets whether or not to use the YouTube SponsorBlock extension.
         * @param sponsorBlock True to use SponsorBlock, else false
         */
        void setYouTubeSponsorBlock(bool sponsorBlock);
        /**
         * @brief Gets whether or not to embed metadata.
         * @return True to embed metadata, else false
         */
        bool getEmbedMetadata() const;
        /**
         * @brief Sets whether or not to embed metadata.
         * @param embedMetadata True to embed metadata, else false
         */
        void setEmbedMetadata(bool embedMetadata);
        /**
         * @brief Gets whether or not to crop thumbnails as square for audio files.
         * @return True to crop audio thumbnails, else false
         */
        bool getCropAudioThumbnails() const;
        /**
         * @brief Sets whether or not to crop thumbnails as square for audio files.
         * @param cropAudioThumbnails True to crop audio thumbnails, else false
         */
        void setCropAudioThumbnails(bool cropAudioThumbnails);
        /**
         * @brief Gets whether or not to remove source data from metadata.
         * @return True to remove source data, else false
         */
        bool getRemoveSourceData() const;
        /**
         * @brief Sets whether or not to remove source data from metadata.
         * @param removeSourceData True to remove source data, else false
         */
        void setRemoveSourceData(bool removeSourceData);
        /**
         * @brief Gets whether or not to embed chapters.
         * @return True to embed chapters, else false
         */
        bool getEmbedChapters() const;
        /**
         * @brief Sets whether or not to embed chapters.
         * @param embedChapters True to embed chapters, else false
         */
        void setEmbedChapters(bool embedChapters);
        /**
         * @brief Gets whether or not to embed subtitles.
         * @return True to embed subtitles, else false
         */
        bool getEmbedSubtitles() const;
        /**
         * @brief Sets whether or not to embed subtitles.
         * @param embedSubtitles True to embed subtitles, else false
         */
        void setEmbedSubtitles(bool embedSubtitles);
        /**
         * @brief Gets the number of threads to use for postprocessing operations.
         * @return The number of threads to use for postprocessing operations
         */
        int getPostprocessingThreads() const;
        /**
         * @brief Sets the number of threads to use for postprocessing operations.
         * @param threads The new number of threads to use for postprocessing operations
         */
        void setPostprocessingThreads(int threads);

    private:
        bool m_overwriteExistingFiles;
        int m_maxNumberOfActiveDownloads;
        bool m_limitCharacters;
        bool m_includeAutoGeneratedSubtitles;
        VideoCodec m_preferredVideoCodec;
        SubtitleFormat m_preferredSubtitleFormat;
        bool m_recoverCrashedDownloads;
        bool m_useAria;
        int m_ariaMaxConnectionsPerServer;
        int m_ariaMinSplitSize;
        bool m_verboseLogging;
        int m_speedLimit;
        std::string m_proxyUrl;
        Browser m_cookiesBrowser;
        std::filesystem::path m_cookiesPath;
        bool m_youTubeSponsorBlock;
        bool m_embedMetadata;
        bool m_cropAudioThumbnails;
        bool m_removeSourceData;
        bool m_embedChapters;
        bool m_embedSubtitles;
        int m_postprocessingThreads;
    };
}

#endif //DOWNLOADEROPTIONS_H