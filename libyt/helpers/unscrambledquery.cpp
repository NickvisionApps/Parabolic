#include "unscrambledquery.h"

namespace NickvisionTubeConverter::libyt::Helpers
{
    UnscrambledQuery::UnscrambledQuery(const std::string& uri, bool isEncrypted) : m_uri(uri), m_isEncrypted(isEncrypted)
    {

    }

    const std::string& UnscrambledQuery::getUri() const
    {
        return m_uri;
    }

    bool UnscrambledQuery::isEncrypted() const
    {
        return m_isEncrypted;
    }
}
