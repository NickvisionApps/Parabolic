#ifndef HISTORICDOWNLOAD_H
#define HISTORICDOWNLOAD_H

#include <filesystem>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Nickvision::TubeConverter::Shared::Models
{
    /**
     * @brief A model of a download that has been completed in the past.
     */
    class HistoricDownload
    {
    public:
        /**
         * @brief Constructs a new HistoricDownload.
         * @param url The URL of the download
         */
        HistoricDownload(const std::string& url);
        /**
         * @brief Constructs a new HistoricDownload.
         * @param url The URL of the download
         * @param title The title of the download
         * @param path The path of the download on disk
         */
        HistoricDownload(const std::string& url, const std::string& title, const std::filesystem::path& path);
        /**
         * @brief Constructs a new HistoricDownload.
         * @param url The URL of the download
         * @param title The title of the download
         * @param path The path of the download on disk
         * @param dateTime The date and time the download was completed
         */
        HistoricDownload(const std::string& url, const std::string& title, const std::filesystem::path& path, const boost::posix_time::ptime& dateTime);
        /**
         * @brief Gets the URL of the download.
         * @return The URL of the download
         */
        const std::string& getUrl() const;
        /**
         * @brief Sets the URL of the download.
         * @param url The URL of the download
         */
        void setUrl(const std::string& url);
        /**
         * @brief Gets the title of the download.
         * @return The title of the download
         */
        const std::string& getTitle() const;
        /**
         * @brief Sets the title of the download.
         * @param title The title of the download
         */
        void setTitle(const std::string& title);
        /**
         * @brief Gets the path of the download on disk.
         * @return The path of the download on disk
         */
        const std::filesystem::path& getPath() const;
        /**
         * @brief Sets the path of the download on disk.
         * @param path The path of the download on disk
         */
        void setPath(const std::filesystem::path& path);
        /**
         * @brief Gets the date and time the download was completed.
         * @return The date and time the download was completed
         */
        const boost::posix_time::ptime& getDateTime() const;
        /**
         * @brief Sets the date and time the download was completed.
         * @param dateTime The date and time the download was completed
         */
        void setDateTime(const boost::posix_time::ptime& dateTime);
        /**
         * @brief Compares two HistoricDownloads via ==.
         * @param other The other HistoricDownload to compare
         * @return True if this == other, false otherwise
         */
        bool operator==(const HistoricDownload& other) const;

    private:
        std::string m_url;
        std::string m_title;
        std::filesystem::path m_path;
        boost::posix_time::ptime m_dateTime;
    };
}

#endif //HISTORICDOWNLOAD_H