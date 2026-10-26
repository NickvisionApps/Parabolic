#ifndef BATCH_H
#define BATCH_H

#include <filesystem>
#include <string>
#include <vector>

namespace parabolic::batch
{
    class Entry
    {
    public:
        Entry(std::string url);
        [[nodiscard]] const std::string& url() const;
        [[nodiscard]] const std::filesystem::path& suggested_save_folder() const;
        void set_suggested_save_folder(const std::filesystem::path& folder);
        [[nodiscard]] const std::string& suggested_filename() const;
        void set_suggested_filename(const std::string& filename);

    private:
        std::string m_url;
        std::filesystem::path m_suggested_save_folder;
        std::string m_suggested_filename;
    };

    std::vector<Entry> read_file(const std::filesystem::path& path);
}

#endif
