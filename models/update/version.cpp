#include "version.h"
#include <stdexcept>

namespace NickvisionTubeConverter::Models::Update
{
    Version::Version(const std::string& version)
    {
        int result = sscanf(version.c_str(), "%d.%d.%d", &m_major, &m_minor, &m_build);
        if (result < 3)
        {
            throw std::invalid_argument("Invalid Version String. Format: a.b.c");
        }
    }

    std::string Version::toString() const
    {
        return std::to_string(m_major) + "." + std::to_string(m_minor) + "." + std::to_string(m_build);
    }

    bool Version::operator<(const Version& toCompare) const
    {
        if (m_major < toCompare.m_major)
        {
            return true;
        }
        else
        {
            if (m_minor < toCompare.m_minor)
            {
                return true;
            }
            else
            {
                if (m_build < toCompare.m_build)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool Version::operator==(const Version& toCompare) const
    {
        return m_major == toCompare.m_major && m_minor == toCompare.m_minor && m_build == toCompare.m_build;
    }

    bool Version::operator!=(const Version& toCompare) const
    {
        return !(*this == toCompare);
    }

    bool Version::operator>(const Version& toCompare) const
    {
        return !(*this < toCompare) && !(*this == toCompare);
    }
}
