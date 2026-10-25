#include "thumbnail.h"
#include <algorithm>
#include <array>
#include <cpr/cpr.h>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <mutex>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace parabolic::thumbnail
{
    Thumbnail::Thumbnail(std::string url)
        : m_url{ std::move(url) }
    {
    }

    const std::string& Thumbnail::url() const
    {
        return m_url;
    }

    const std::vector<std::byte>& Thumbnail::bytes() const
    {
        static std::mutex s_mutex;
        static std::unordered_map<std::string, std::vector<std::byte>> s_cache{
            {
                    std::string{},
                    std::ranges::to<std::vector<std::byte>>(std::as_bytes(std::span<const unsigned char>{
                            std::to_array<unsigned char>({
#embed "default_thumbnail.jpg"
                            }),
                    })),
            },
        };
        std::unique_lock lock{ s_mutex };
        std::unordered_map<std::string, std::vector<std::byte>>::iterator cached{ s_cache.find(m_url) };
        if (cached != s_cache.end())
        {
            return cached->second;
        }
        lock.unlock();
        std::vector<std::byte> downloaded;
        cpr::Response response{
            cpr::Get(cpr::Url{ m_url }, cpr::UserAgent{ "NickvisionParabolic" }, cpr::ConnectTimeout{ 15000 },
                     cpr::Timeout{ 30000 },
                     cpr::WriteCallback{ [&downloaded](std::string_view data, std::intptr_t) -> bool
        {
            std::ranges::copy(std::as_bytes(std::span{ data }), std::back_inserter(downloaded));
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
