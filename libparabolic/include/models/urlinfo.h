#ifndef URLINFO_H
#define URLINFO_H

#include <optional>
#include <string>
#include <vector>
#include "media.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    class UrlInfo
    {
    public:
        /**
         * @brief Fetches media information for a URL.
         * @param url The URL to fetch media information for
         * @return The UrlInfo object on success, else std::nullopt
         */
        static std::optional<UrlInfo> fetch(const std::string& url);
        /**
         * @brief Gets the URL.
         * @return The URL
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets whether the URL is a playlist.
         * @return True if playlist, false otherwise
         */
        bool isPlaylist() const;
        /**
         * @brief Gets the list of media belonging to the URL.
         * @return The list of media belonging to the URL
         */
        const std::vector<Media>& getMedia() const;

    private:
        /**
         * @brief Constructs a UrlInfo.
         * @param url The URL
         */
        UrlInfo(const std::string& url);
        std::string m_url;
        bool m_isPlaylist;
        std::vector<Media> m_media;
    };
}

#endif //URLINFO_H