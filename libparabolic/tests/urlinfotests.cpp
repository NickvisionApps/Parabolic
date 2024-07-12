#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <libnick/system/environment.h>
#include "models/downloaderoptions.h"
#include "models/urlinfo.h"

using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Models;

class UrlInfoTest : public testing::Test
{
public:
    static DownloaderOptions m_options;

    static void SetUpTestCase()
    {
        m_options.setLimitCharacters(Environment::getOperatingSystem() == OperatingSystem::Windows);
    }
};

DownloaderOptions UrlInfoTest::m_options;

TEST_F(UrlInfoTest, YouTube1)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://www.youtube.com/watch?v=kdksdfhkjsdfhkjsdfhkj", m_options) };
    ASSERT_FALSE(info.has_value());
}

TEST_F(UrlInfoTest, YouTube2)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://www.youtube.com/watch?v=CvUxi35IdWM&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_FALSE(info->isPlaylist());
    ASSERT_EQ(info->count(), 1);
    const Media& media{ info->get(0) };
    ASSERT_EQ(media.getTitle(), "NEFFEX - Take Off [Copyright-Free] No.225");
    ASSERT_EQ(media.getType(), MediaType::Video);
    ASSERT_FALSE(media.hasSubtitles());
    std::cout << *info << std::endl;
}

TEST_F(UrlInfoTest, YouTube3)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://www.youtube.com/watch?v=9gBTKiVqprE&list=PLfP6i5T0-DkIvYYFH2GLRSBv_XYyWRN62", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_TRUE(info->isPlaylist());
    ASSERT_EQ(info->count(), 16);
    std::cout << *info << std::endl;
}

TEST_F(UrlInfoTest, YouTube4)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://www.youtube.com/watch?v=wTuOasmssE0", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_FALSE(info->isPlaylist());
    ASSERT_EQ(info->count(), 1);
    const Media& media{ info->get(0) };
    ASSERT_EQ(media.getTitle(), "My Wifi is Better than Yours");
    ASSERT_EQ(media.getType(), MediaType::Video);
    ASSERT_FALSE(media.hasSubtitles());
    std::cout << *info << std::endl;
}

TEST_F(UrlInfoTest, YouTube5)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://www.youtube.com/watch?v=o2EJnLOSOOc&ab_channel=DawidDoesTechStuff", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_FALSE(info->isPlaylist());
    ASSERT_EQ(info->count(), 1);
    const Media& media{ info->get(0) };
    ASSERT_EQ(media.getTitle(), "I Tried Gaming On The New Snapdragon X Windows ARM Laptop...");
    ASSERT_EQ(media.getType(), MediaType::Video);
    ASSERT_TRUE(media.hasSubtitles());
    std::cout << *info << std::endl;
}

TEST_F(UrlInfoTest, YouTubeMusic1)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://music.youtube.com/watch?v=kdksdfhkjsdfhkjsdfhkj", m_options) };
    ASSERT_FALSE(info.has_value());
}

TEST_F(UrlInfoTest, YouTubeMusic2)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://music.youtube.com/watch?v=JOQazNC9lCU", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_FALSE(info->isPlaylist());
    ASSERT_EQ(info->count(), 1);
    const Media& media{ info->get(0) };
    ASSERT_EQ(media.getTitle(), "Epic Emotional");
    ASSERT_EQ(media.getType(), MediaType::Video);
    ASSERT_FALSE(media.hasSubtitles());
    std::cout << *info << std::endl;
}

TEST_F(UrlInfoTest, SoundCloud1)
{
    std::optional<UrlInfo> info{ UrlInfo::fetch("https://soundcloud.com/neffexmusic/take-control-copyright-free", m_options) };
    ASSERT_TRUE(info.has_value());
    ASSERT_FALSE(info->isPlaylist());
    ASSERT_EQ(info->count(), 1);
    const Media& media{ info->get(0) };
    ASSERT_EQ(media.getTitle(), "Take Control [Copyright-Free]");
    ASSERT_EQ(media.getType(), MediaType::Audio);
    ASSERT_FALSE(media.hasSubtitles());
    std::cout << *info << std::endl;
}