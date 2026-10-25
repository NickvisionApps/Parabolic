#include "ytdlp.h"
#include <string>
#include <unordered_set>

namespace parabolic::ytdlp
{
    bool is_youtube_supported_language(const std::string& language)
    {
        static std::unordered_set<std::string> supported{
            "af",    "am", "ar",    "as",    "az",    "be", "bg",     "bn",    "bs",      "ca",    "cs", "da",
            "de",    "el", "en",    "en-GB", "en-IN", "es", "es-419", "es-US", "et",      "eu",    "fa", "fi",
            "fil",   "fr", "fr-CA", "gl",    "gu",    "hi", "hr",     "hu",    "hy",      "id",    "is", "it",
            "iw",    "ja", "ka",    "kk",    "km",    "kn", "ko",     "ky",    "lo",      "lt",    "lv", "mk",
            "ml",    "mn", "mr",    "ms",    "my",    "ne", "nl",     "no",    "or",      "pa",    "pl", "pt",
            "pt-PT", "ro", "ru",    "si",    "sk",    "sl", "sq",     "sr",    "sr-Latn", "sv",    "sw", "ta",
            "te",    "th", "tr",    "uk",    "ur",    "uz", "vi",     "zh-CN", "zh-HK",   "zh-TW", "zu",
        };
        return supported.contains(language);
    }
}
