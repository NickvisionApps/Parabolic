#include <gtest/gtest.h>
#include <libnick/filesystem/userdirectories.h>
#include <thread>
#include "models/download.h"
#include "models/downloadoptions.h"
#include "models/downloaderoptions.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::TubeConverter::Shared::Models;

TEST(DownloadTests, YouTubeAudio1)
{
    bool downloadFinished{ false };
    DownloaderOptions defaultDownloaderOptions;
    DownloadOptions options{ "https://www.youtube.com/watch?v=CvUxi35IdWM&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D" };
    options.setFileType(MediaFileType::MP3);
    options.setSaveFolder(UserDirectories::get(UserDirectory::Downloads));
    options.setSaveFilename("Test1");
    Download download{ options };
    download.completed() += [&downloadFinished](const ParamEventArgs<DownloadStatus>&) { downloadFinished = true; };
    ASSERT_NO_THROW(download.start(defaultDownloaderOptions));
    ASSERT_EQ(download.getStatus(), DownloadStatus::Running);
    while(!downloadFinished)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_EQ(download.getStatus(), DownloadStatus::Success);
    ASSERT_TRUE(std::filesystem::exists(download.getPath()));
    std::filesystem::remove(download.getPath());
}