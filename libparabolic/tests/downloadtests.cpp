#include <gtest/gtest.h>
#include <libnick/filesystem/userdirectories.h>
#include <thread>
#include "models/download.h"
#include "models/downloadoptions.h"
#include "models/downloaderoptions.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::TubeConverter::Shared::Models;

class DownloadTest : public testing::Test
{
public:
    static DownloaderOptions m_downloaderOptions;
};

DownloaderOptions DownloadTest::m_downloaderOptions;

TEST_F(DownloadTest, YouTubeAudio1)
{
    bool downloadFinished{ false };
    DownloadOptions options{ "https://www.youtube.com/watch?v=CvUxi35IdWM&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D" };
    options.setFileType(MediaFileType::MP3);
    options.setSaveFolder(UserDirectories::get(UserDirectory::Downloads));
    options.setSaveFilename("Test1");
    Download download{ options };
    download.progressChanged() += [](const DownloadProgressChangedEventArgs& e) { std::cout << e << std::endl; };
    download.completed() += [&downloadFinished](const ParamEventArgs<DownloadStatus>&) { downloadFinished = true; };
    ASSERT_NO_THROW(download.start(m_downloaderOptions));
    ASSERT_EQ(download.getStatus(), DownloadStatus::Running);
    while(!downloadFinished)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_EQ(download.getStatus(), DownloadStatus::Success);
    ASSERT_TRUE(std::filesystem::exists(download.getPath()));
    std::filesystem::remove(download.getPath());
}

TEST_F(DownloadTest, YouTubeVideo1)
{
    bool downloadFinished{ false };
    DownloadOptions options{ "https://www.youtube.com/watch?v=K4DyBUG242c&pp=ygUUY29weXJpZ2h0IG11c2ljIHNvbmc%3D" };
    options.setFileType(MediaFileType::MP4);
    options.setSaveFolder(UserDirectories::get(UserDirectory::Downloads));
    options.setSaveFilename("Test2");
    Download download{ options };
    download.progressChanged() += [](const DownloadProgressChangedEventArgs& e) { std::cout << e << std::endl; };
    download.completed() += [&downloadFinished](const ParamEventArgs<DownloadStatus>&) { downloadFinished = true; };
    ASSERT_NO_THROW(download.start(m_downloaderOptions));
    ASSERT_EQ(download.getStatus(), DownloadStatus::Running);
    while(!downloadFinished)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_EQ(download.getStatus(), DownloadStatus::Success);
    ASSERT_TRUE(std::filesystem::exists(download.getPath()));
    std::filesystem::remove(download.getPath());
}