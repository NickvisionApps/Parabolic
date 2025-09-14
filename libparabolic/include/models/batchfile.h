#ifndef BATCHFILE_H
#define BATCHFILE_H

#include <filesystem>
#include <vector>
#include "batchfileentry.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a batch file containing multiple download entries.
     * @brief Follows the format "URL | Suggested Save Folder | Suggested Filename" per line.
     */
    class BatchFile
    {
    public:
        /**
         * @brief Constructs a BatchFile.
         * @brief This will scan the file and attempt to parse the entries.
         * @brief Lines that do not contain valid entries will be ignored.
         * @param path The path to the batch file
         */
        BatchFile(const std::filesystem::path& path);
        /**
         * @brief Gets the path to the batch file.
         * @return The path to the batch file
         */
        const std::filesystem::path& getPath() const;
        /**
         * @brief Gets the entries in the batch file.
         * @return The entries in the batch file
         */
        const std::vector<BatchFileEntry>& getEntries() const;


    private:
        std::filesystem::path m_path;
        std::vector<BatchFileEntry> m_entries;
    };
}

#endif //BATCHFILE_H