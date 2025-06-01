#include "models/m3u.h"
#include <fstream>
#include "libnick/helpers/stringhelpers.h"

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    M3U::M3U(const std::string& title)
    {
        m_builder << "#EXTM3U" << std::endl;
        if(!title.empty())
        {
            m_builder << "#PLAYLIST:" << title << std::endl;
        }
    }

    bool M3U::add(const DownloadOptions& options)
    {
        if(options.getFileType().isGeneric())
        {
            return false;
        }
        std::filesystem::path path{ options.getSaveFolder() / (options.getSaveFilename() + options.getFileType().getDotExtension()) };
        m_builder << path.make_preferred().string() << std::endl;
        return true;
    }

    bool M3U::write(std::filesystem::path path) const
    {
        if(StringHelpers::lower(path.extension().string()) != ".m3u")
        {
            path.replace_extension(".m3u");
        }
        std::ofstream file{ path, std::ios::trunc };
        if(!file.is_open())
        {
            return false;
        }
        file << m_builder.str() << std::endl;
        return true;
    }
}
