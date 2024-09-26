#ifndef PREVIOUSDOWNLOADOPTIONS_H
#define PREVIOUSDOWNLOADOPTIONS_H

#include <filesystem>
#include <string>
#include <libnick/app/datafilebase.h>
#include "mediafiletype.h"

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
         * @brief Gets the previous limit speed state.
         * @return The previous limit speed state
         */
        bool getLimitSpeed() const;
        /**
         * @brief Sets the previous limit speed state.
         * @param limitSpeed The new previous limit speed state
         */
        void setLimitSpeed(bool limitSpeed);
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