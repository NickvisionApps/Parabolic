#include "audio.h"
#include <string>
#include <utility>

namespace parabolic::audio
{
    Language::Language(std::string language, bool audio_description)
        : m_language{ std::move(language) },
          m_audio_description{ audio_description }
    {
    }

    const std::string& Language::language() const
    {
        return m_language;
    }

    bool Language::has_audio_description() const
    {
        return m_audio_description;
    }
}
