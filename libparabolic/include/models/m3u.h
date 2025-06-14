#ifndef M3U_H
#define M3U_H

#include <filesystem>
#include <sstream>
#include <string>
#include "downloadoptions.h"
#include "pathtype.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a M3U playlist file.
     */
    class M3U
    {
    public:
        /**
         * @brief Constructs a M3U.
         * @param title The title of the playlist
         * @param pathType The type of paths to use in the playlist
         */
        M3U(const std::string& title, PathType pathType);
        /**
         * @brief Adds a download to the m3u file.
         * @brief Generic downloads are not supported as their file type is unknown.
         * @param options The DownloadOptions
         * @return True if the download was added
         * @return False if the download was not added
         */
        bool add(const DownloadOptions& options);
        /**
         * @brief Writes the m3u file to disk.
         * @param path The path of the file on disk
         * @return True if the file was written
         * @return False if the file was not written
         */
        bool write(std::filesystem::path path) const;

    private:
        PathType m_pathType;
        std::stringstream m_builder;
    };
}

#endif //M3U_H
