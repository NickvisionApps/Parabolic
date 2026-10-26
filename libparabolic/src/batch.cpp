#include "batch.h"
#include <filesystem>
#include <string>
#include <vector>
#include <utility>

#define DELIMITER '|'

namespace parabolic::batch
{
    Entry::Entry(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Entry::url() const
    {
        return m_url;
    }

    const std::filesystem::path& Entry::suggested_save_folder() const
    {
        return m_suggested_save_folder;
    }

    void Entry::set_suggested_save_folder(const std::filesystem::path& folder)
    {
        m_suggested_save_folder = folder;
    }

    const std::string& Entry::suggested_filename() const
    {
        return m_suggested_filename;
    }

    void Entry::set_suggested_filename(const std::string& filename)
    {
        m_suggested_filename = filename;
    }

    std::vector<Entry> read_file(const std::filesystem::path& path)
    {
        return {};
    }
}
