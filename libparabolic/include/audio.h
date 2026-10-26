#ifndef AUDIO_H
#define AUDIO_H

#include <cstdint>
#include <string>

namespace parabolic::audio
{
    enum class Codec : std::uint8_t
    {
        Any,
        Flac,
        Wav,
        Opus,
        Aac,
        Mp4a,
        Mp3,
    };

    class Language
    {
    public:
        Language(std::string language, bool audio_description);
        [[nodiscard]] const std::string& language() const;
        [[nodiscard]] bool has_audio_description() const;

    private:
        std::string m_language;
        bool m_audio_description;
    };
}

#endif
