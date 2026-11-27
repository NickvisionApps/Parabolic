#include "m3u.h"
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <utility>
#include "environment.h"
#include "string_manip.h"

namespace parabolic::m3u
{
    M3uFile::M3uFile(std::string title, bool use_relative_paths)
        : m_title{ std::move(title) },
          m_use_relative_paths{ use_relative_paths }
    {
        m_stream << "#EXTM3U\n";
        if (!m_title.empty())
        {
            m_stream << "#PLAYLIST:" << m_title << "\n";
        }
    }

    const std::string& M3uFile::title() const
    {
        return m_title;
    }

    std::expected<void, std::string> M3uFile::write(std::filesystem::path path)
    {
        if (string_manip::lower(path.extension().string()) != ".m3u")
        {
            path += ".m3u";
        }
        std::ofstream file{ path, std::ios::out | std::ios::trunc };
        if (!file)
        {
            return std::unexpected(environment::last_system_error());
        }
        file << m_stream.str();
        file.flush();
        if (!file)
        {
            return std::unexpected(environment::last_system_error());
        }
        return {};
    }
}
