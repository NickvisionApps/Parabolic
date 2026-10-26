#ifndef M3U_H
#define M3U_H

#include <filesystem>
#include <sstream>
#include <string>

namespace parabolic::m3u
{
    class M3uFile
    {
    public:
        M3uFile(std::string title, bool use_relative_paths);
        [[nodiscard]] const std::string& title() const;
        bool write(std::filesystem::path path);

    private:
        std::string m_title;
        bool m_use_relative_paths;
        std::stringstream m_stream;
    };
}

#endif
