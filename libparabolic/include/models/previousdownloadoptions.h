#ifndef PREVIOUSDOWNLOADOPTIONS_H
#define PREVIOUSDOWNLOADOPTIONS_H

#include <filesystem>
#include <string>
#include <libnick/app/datafilebase.h>
#include "mediafiletype.h"
#include "videoresolution.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of the most recently used download options.
     */
    class PreviousDownloadOptions : public Nickvision::App::DataFileBase
    {
    public:
        /**
         * @brief Constructs a PreviousDownloadOptions.
         * @param key The key to pass to the DataFileBase
         * @param appName The name of the application to pass to the DataFileBase
         */
        PreviousDownloadOptions(const std::string& key, const std::string& appName);
        /**
         * @brief Gets the previous save folder.
         * @return The previous save folder
         */
        std::filesystem::path getSaveFolder() const;
        /**
         * @brief Sets the previous save folder.
         * @param previousSaveFolder The new previous save folder
         */
        void setSaveFolder(const std::filesystem::path& previousSaveFolder);
        /**
         * @brief Gets the previous media file type.
         * @return The previous media file type
         */
        MediaFileType getFileType() const;
        /**
         * @brief Sets the previous media file type.
         * @param previousMediaFileType The new previous media file type
         */
        void setFileType(const MediaFileType& previousMediaFileType);
        /**
         * @brief Gets the previous video resolution.
         * @return The previous video resolution
         */
        VideoResolution getVideoResolution() const;
        /**
         * @brief Sets the previous video resolution.
         * @param previousVideoResolution The new previous video resolution
         */
        void setVideoResolution(const VideoResolution& previousVideoResolution);
        /**
         * @brief Gets the previous download subtitles state.
         * @return The previous download subtitles state
         */
        bool getDownloadSubtitles() const;
        /**
         * @brief Sets the previous download subtitles state.
         * @param previousSubtitleState The new previous download subtitles state
         */
        void setDownloadSubtitles(bool previousSubtitleState);
        /**
         * @brief Gets the previous prefer AV1 state.
         * @return The previous prefer AV1 state
         */
        bool getPreferAV1State() const;
        /**
         * @brief Sets the previous prefer AV1 state.
         * @param previousPreferAV1State The new previous prefer AV1 state
         */
        void setPreferAV1State(bool previousPreferAV1State);
        /**
         * @brief Gets the previous number titles state.
         * @return The previous number titles state
         */
        bool getNumberTitles() const;
        /**
         * @brief Sets the previous number titles state.
         * @param previousNumberTitles The new previous number titles state
         */
        void setNumberTitles(bool numberTitles);
    };
}

#endif //PREVIOUSDOWNLOADOPTIONS_H