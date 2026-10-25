#include "subtitle.h"
#include <compare>
#include <string>
#include <utility>
#include "translator.h"

using namespace parabolic::translator;

namespace parabolic::subtitle
{
    Language::Language(std::string language, bool is_auto_generated)
        : m_language{ std::move(language) },
          m_is_auto_generated{ is_auto_generated }
    {
    }

    const std::string& Language::language() const
    {
        return m_language;
    }

    bool Language::is_auto_generated() const
    {
        return m_is_auto_generated;
    }

    std::string Language::str(const Translator& translator) const
    {
        if (m_is_auto_generated)
        {
            return translator._("{} (Auto-generated)", m_language);
        }
        return m_language;
    }

    std::strong_ordering Language::operator<=>(const Language& rhs) const
    {
        if (m_is_auto_generated != rhs.m_is_auto_generated)
        {
            return static_cast<int>(m_is_auto_generated) <=> static_cast<int>(rhs.m_is_auto_generated);
        }
        return m_language <=> rhs.m_language;
    }

    bool Language::operator==(const Language& rhs) const
    {
        return m_language == rhs.m_language;
    }
}
