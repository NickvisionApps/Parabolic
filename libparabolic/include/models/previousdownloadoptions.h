#ifndef PREVIOUSDOWNLOADOPTIONS_H
#define PREVIOUSDOWNLOADOPTIONS_H

#include <filesystem>
#include <string>
#include <vector>
#include <libnick/app/datafilebase.h>
#include "mediafiletype.h"
#include "subtitlelanguage.h"

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
         * @param isPortable The isPortable to pass to the DataFileBase
         */
        PreviousDownloadOptions(const std::string& key, const std::string& appName, bool isPortable);
        /**
         * @brief Gets the previous save folder.
         * @return The previous save folder
         */
        std::filesystem::path getSaveFolder() const;
        /**
         * @brief Sets the previous save folder.
         * @param previousSaveFolder The new previous save folder
         */
        void setSaveFolder(std::filesystem::path previousSaveFolder);
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
         * @brief Gets the previous video format id for the media file type.
         * @param type The MediaFileType
         * @return The previous video format id for the media file type
         */
        std::string getVideoFormatId(const MediaFileType& type) const;
        /**
         * @brief Sets the previous video format id for the media file type.
         * @param type The MediaFileType
         * @param videoFormatId The new previous video format id
         */
        void setVideoFormatId(const MediaFileType& type, const std::string& videoFormatId);
        /**
         * @brief Gets the previous audio format id for the media file type.
         * @param type The MediaFileType
         * @return The previous audio format id for the media file type
         */
        std::string getAudioFormatId(const MediaFileType& type) const;
        /**
         * @brief Sets the previous audio format id for the media file type.
         * @param type The MediaFileType
         * @param audioFormatId The new previous audio format id
         */
        void setAudioFormatId(const MediaFileType& type, const std::string& audioFormatId);
        /**
         * @brief Gets the previous split chapters state.
         * @return The previous split chapters state
         */
        bool getSplitChapters() const;
        /**
         * @brief Sets the previous split chapters state.
         * @param splitChapters The new previous split chapters state
         */
        void setSplitChapters(bool splitChapters);
        /**
         * @brief Gets the previous export description state.
         * @return The previous export description state
         */
        bool getExportDescription() const;
        /**
         * @brief Sets the previous export description state.
         * @param exportDescription The new previous export description state
         */
        void setExportDescription(bool exportDescription);
        /**
         * @brief Gets the previous name of the post processor argument.
         * @return The previous post processor argument name
         */
        std::string getPostProcessorArgument() const;
        /**
         * @brief Sets the previous name of the post processor argument.
         * @param postProcessorArgument The new previous postprocessing argument name
         */
        void setPostProcessorArgument(const std::string& postProcessorArgument);
        /**
         * @brief Gets the previous write playlist file state.
         * @return The previous write playlist file state
         */
        bool getWritePlaylistFile() const;
        /**
         * @brief Sets the previous write playlist file state.
         * @param exportDescription The new previous write playlist file state
         */
        void setWritePlaylistFile(bool writePlaylistFile);
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
        /**
         * @brief Gets the previous subtitle languages.
         * @return The previous subtitle languages
         */
        std::vector<SubtitleLanguage> getSubtitleLanguages() const;
        /**
         * @brief Sets the previous subtitle languages.
         * @param previousSubtitleLanguages The new previous subtitle languages
         */
        void setSubtitleLanguages(const std::vector<SubtitleLanguage>& previousSubtitleLanguages);
    };
}

#endif //PREVIOUSDOWNLOADOPTIONS_H
