#include "subtitle.h"
#include <format>

namespace parabolic
{
    SubtitleLanguage::SubtitleLanguage(std::string language, bool is_auto_generated)
        : m_language{ std::move(language) },
        m_is_auto_generated{ is_auto_generated }
    {
    }

    const std::string& SubtitleLanguage::language() const
    {
        return m_language;
    }

    bool SubtitleLanguage::is_auto_generated() const
    {
        return m_is_auto_generated;
    }

    std::string SubtitleLanguage::str() const
    {
        if (m_is_auto_generated)
        {
            return std::format("{} (Auto-generated)", m_language);
        }
        return m_language;
    }

    std::string SubtitleLanguage::str(const Translator& translator) const
    {
        if (m_is_auto_generated)
        {
            return translator._("{} (Auto-generated)", m_language);
        }
        return m_language;
    }

    std::strong_ordering SubtitleLanguage::operator<=>(const SubtitleLanguage& rhs) const
    {
        if (m_is_auto_generated != rhs.m_is_auto_generated)
        {
            return m_is_auto_generated <=> rhs.m_is_auto_generated;
        }
        return m_language <=> rhs.m_language;
    }

    bool SubtitleLanguage::operator==(const SubtitleLanguage& rhs) const
    {
        return m_language == rhs.m_language;
    }
}