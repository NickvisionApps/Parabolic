#ifndef URLINFO_H
#define URLINFO_H

#include <optional>
#include <ostream>
#include <string>
#include <vector>
#include <json/json.h>
#include <libnick/keyring/credential.h>
#include "downloaderoptions.h"
#include "media.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    class UrlInfo
    {
    public:
        /**
         * @brief Fetches media information for a Url.
         * @param url The Url to fetch media information for
         * @param options The DownloaderOptions
         * @param credential The credential to use for authentication
         * @return The UrlInfo object on success, else std::nullopt
         */
        static std::optional<UrlInfo> fetch(const std::string& url, const DownloaderOptions& options, const std::optional<Keyring::Credential>& credential = std::nullopt);
        /**
         * @brief Gets the Url.
         * @return The Url
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets whether the Url is a playlist.
         * @return True if playlist, false otherwise
         */
        bool isPlaylist() const;
        /**
         * @brief Gets the number of media belonging to the Url.
         * @return The number of media
         */
        size_t count() const;
        /**
         * @brief Gets the media at the specified index.
         * @param index The index
         */
        Media& get(size_t index);
        /**
         * @brief Gets the media at the specified index.
         * @param index The index
         */
        const Media& get(size_t index) const;
        /**
         * @brief Gets the media at the specified index.
         * @param index The index
         */
        Media& operator[](size_t index);
        /**
         * @brief Gets the media at the specified index.
         * @param index The index
         */
        const Media& operator[](size_t index) const;
        /**
         * @brief Outputs the UrlInfo to an output stream.
         * @param os The output stream
         * @param info The UrlInfo
         * @return The output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const UrlInfo& info);

    private:
        /**
         * @brief Constructs a UrlInfo.
         * @param url The Url
         * @param info The media information json object from yt-dlp
         */
        UrlInfo(const std::string& url, const Json::Value& info);
        std::string m_url;
        bool m_isPlaylist;
        std::vector<Media> m_media;
    };
}

#endif //URLINFO_H