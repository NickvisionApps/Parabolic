#ifndef BATCHFILEENTRY_H
#define BATCHFILEENTRY_H

#include <filesystem>
#include <string>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of an entry in a batch file.
     */
    class BatchFileEntry
    {
    public:
        /**
         * @brief Constructs a BatchFileEntry.
         * @param url The URL of the entry
         */
        BatchFileEntry(const std::string& url);
        /**
         * @brief Gets the URL of the entry.
         * @return The URL of the entry
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets the suggested save folder of the entry.
         * @return The suggested save folder of the entry
         * @return Empty path if none was specified
         */
        const std::filesystem::path& getSuggestedSaveFolder() const;
        /**
         * @brief Sets the suggested save folder of the entry.
         * @param suggestedSaveFolder The suggested save folder of the entry
         */
        void setSuggestedSaveFolder(const std::filesystem::path& suggestedSaveFolder);
        /**
         * @brief Gets the suggested filename of the entry.
         * @return The suggested filename of the entry
         * @return Empty string if none was specified
         */
        const std::string& getSuggestedFilename() const;
        /**
         * @brief Sets the suggested filename of the entry.
         * @param suggestedFilename The suggested filename of the entry
         */
        void setSuggestedFilename(const std::string& suggestedFilename);

    private:
        std::string m_url;
        std::filesystem::path m_suggestedSaveFolder;
        std::string m_suggestedFilename;
    };
}

#endif //BATCHFILEENTRY_H