#ifndef UNSCRAMBLEDQUERY_H
#define UNSCRAMBLEDQUERY_H

#include <string>

namespace NickvisionTubeConverter::libyt::Helpers
{
    class UnscrambledQuery
    {
    public:
        UnscrambledQuery(const std::string& uri, bool isEncrypted);
        const std::string& getUri() const;
        bool isEncrypted() const;

    private:
        std::string m_uri;
        bool m_isEncrypted;
    };
}

#endif // UNSCRAMBLEDQUERY_H
