#ifndef URLINFO_H
#define URLINFO_H

#include <string>
#include <vector>
#include <boost/json.hpp>
#include "media.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    class UrlInfo
    {
    public:
        /**
         * @brief Constructs a UrlInfo.
         * @param url The Url
         * @param info The media information json object from yt-dlp
         */
        UrlInfo(const std::string& url, boost::json::object info);
        /**
         * @brief Constructs a UrlInfo.
         * @brief This method constructs a single playlist UrlInfo from a group of UrlInfo.
         * @param url The source url of the group
         * @param title The title of the group
         * @param urlInfos A list of UrlInfo's to combine
         */
        UrlInfo(const std::string& url, const std::string& title, const std::vector<UrlInfo>& urlInfos);
        /**
         * @brief Gets the title.
         * @return The title
         */
        const std::string& getTitle() const;
        /**
         * @brief Gets the Url.
         * @return The Url
         */
        const std::string& getUrl() const;
        /**
         * @brief Gets whether the Url is a playlist.
         * @return True if playlist, else false
         */
        bool isPlaylist() const;
        /**
         * @brief Gets whether or not the Url has a suggested save folder.
         * @return True if it has a suggested save folder, else false
         **/
        bool hasSuggestedSaveFolder() const;
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

    private:
        std::string m_url;
        std::string m_title;
        bool m_hasSuggestedSaveFolder;
        std::vector<Media> m_media;
    };
}

#endif //URLINFO_H
