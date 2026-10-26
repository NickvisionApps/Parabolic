#include "batch.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include "string_manip.h"
#include "url.h"
#ifdef _WIN32
#include "environment.h"
#else
#include <sago/platform_folders.h>
#endif

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
        if (!std::filesystem::exists(path) || string_manip::lower(path.extension().string()) != ".txt")
        {
            return {};
        }
        std::ifstream file{ path };
        std::string line;
        std::vector<Entry> entries;
        while (std::getline(file, line))
        {
            std::vector<std::string> fields{ string_manip::split(line, '|') };
            if (fields.size() > 3)
            {
                continue;
            }
            std::string url{ string_manip::trim(fields[0]) };
            if (!url::is_valid(url))
            {
                continue;
            }
            Entry entry{ url };
            if (fields.size() > 1)
            {
                std::string save_folder{ string_manip::trim(fields[1]) };
                if (save_folder.starts_with('~'))
                {
#ifdef _WIN32
                    save_folder = environment::variable("USERPROFILE") + save_folder.substr(1);
#else
                    save_folder = sago::internal::getHome() + save_folder.substr(1);
#endif
                }
                if (std::filesystem::path{ save_folder }.has_root_path())
                {
                    entry.set_suggested_save_folder(save_folder);
                }
            }
            if (fields.size() > 2)
            {
                std::string filename{ string_manip::trim(fields[2]) };
                if (!filename.empty())
                {
                    entry.set_suggested_filename(filename);
                }
            }
            entries.push_back(std::move(entry));
        }
        return entries;
    }
}
