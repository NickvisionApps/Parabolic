#include "thumbnail.h"
#include <cpr/cpr.h>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace parabolic::thumbnail
{
    Image::Image(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Image::url() const
    {
        return m_url;
    }

    const std::vector<std::uint8_t>& Image::bytes() const
    {
        static std::mutex s_mutex;
        static std::unordered_map<std::string, std::vector<std::uint8_t>> s_cache{
            {
                    std::string{},
                    std::vector<std::uint8_t>{
#include "default_thumbnail.inc"
                    },
            },
        };
        std::unique_lock lock{ s_mutex };
        std::unordered_map<std::string, std::vector<std::uint8_t>>::iterator cached{ s_cache.find(m_url) };
        if (cached != s_cache.end())
        {
            return cached->second;
        }
        lock.unlock();
        std::vector<std::uint8_t> downloaded;
        cpr::Response response{
            cpr::Get(cpr::Url{ m_url }, cpr::UserAgent{ "NickvisionParabolic" }, cpr::ConnectTimeout{ 15000 },
                     cpr::Timeout{ 30000 },
                     cpr::WriteCallback{ [&downloaded](std::string_view data, std::intptr_t) -> bool
        {
            downloaded.insert(downloaded.end(), data.begin(), data.end());
            return true;
        } }),
        };
        if (response.error || response.status_code != 200)
        {
            downloaded.clear();
        }
        lock.lock();
        return s_cache[m_url] = downloaded.empty() ? s_cache[std::string{}] : std::move(downloaded);
    }
}
