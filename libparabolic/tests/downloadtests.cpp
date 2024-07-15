#include <gtest/gtest.h>
#include <filesystem>
#include <memory>
#include <libnick/filesystem/userdirectories.h>
#include <libnick/logging/logger.h>
#include "models/download.h"
#include "models/downloadhistory.h"
#include "models/downloadmanager.h"
#include "models/downloadoptions.h"
#include "models/downloaderoptions.h"

using namespace Nickvision::Filesystem;
using namespace Nickvision::Logging;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;

class DownloadTest : public testing::Test
{
public:
    static std::shared_ptr<DownloadManager> m_downloadManager;
    static std::filesystem::path m_saveFolder;

    static void SetUpTestSuite()
    {
        m_downloadManager = std::make_shared<DownloadManager>(m_downloaderOptions, m_downloadHistory, m_logger);
    }

    static void TearDownTestSuite()
    {
        m_downloadManager.reset();
        std::filesystem::remove_all(m_saveFolder);
    }

private:
    static DownloaderOptions m_downloaderOptions;
    static DownloadHistory m_downloadHistory;
    static Logger m_logger;
};

DownloaderOptions DownloadTest::m_downloaderOptions;
DownloadHistory DownloadTest::m_downloadHistory{ "history", "libparabolic" };
Logger DownloadTest::m_logger;
std::shared_ptr<DownloadManager> DownloadTest::m_downloadManager{ nullptr };
std::filesystem::path DownloadTest::m_saveFolder{ UserDirectories::get(UserDirectory::Downloads) / "libparabolic" };

TEST_F(DownloadTest, AddDownload1)
{
    DownloadOptions options{ "https://www.youtube.com/watch?v=CvUxi35IdWM&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D" };
    options.setFileType(MediaFileType::MP3);
    options.setSaveFolder(m_saveFolder);
    options.setSaveFilename("Test1");
    ASSERT_NO_THROW(m_downloadManager->addDownload(options));
    ASSERT_EQ(m_downloadManager->getDownloadingCount(), 1);
    ASSERT_EQ(m_downloadManager->getQueuedCount(), 0);
    ASSERT_EQ(m_downloadManager->getCompletedCount(), 0);
}

TEST_F(DownloadTest, AddDownload2)
{
    DownloadOptions options{ "https://www.youtube.com/watch?v=K4DyBUG242c&pp=ygUUY29weXJpZ2h0IG11c2ljIHNvbmc%3D" };
    options.setFileType(MediaFileType::MP4);
    options.setSaveFolder(m_saveFolder);
    options.setSaveFilename("Test2");
    ASSERT_NO_THROW(m_downloadManager->addDownload(options));
    ASSERT_EQ(m_downloadManager->getDownloadingCount(), 2);
    ASSERT_EQ(m_downloadManager->getQueuedCount(), 0);
    ASSERT_EQ(m_downloadManager->getCompletedCount(), 0);
}

TEST_F(DownloadTest, AddDownload3)
{
    DownloadOptions options{ "https://www.youtube.com/watch?v=83RUhxsfLWs&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D" };
    options.setFileType(MediaFileType::FLAC);
    options.setSaveFolder(m_saveFolder);
    options.setSaveFilename("Test3");
    ASSERT_NO_THROW(m_downloadManager->addDownload(options));
    ASSERT_EQ(m_downloadManager->getDownloadingCount(), 3);
    ASSERT_EQ(m_downloadManager->getQueuedCount(), 0);
    ASSERT_EQ(m_downloadManager->getCompletedCount(), 0);
}

TEST_F(DownloadTest, EnsureDownloadsComplete)
{
    while (m_downloadManager->getRemainingDownloadsCount() > 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    ASSERT_EQ(m_downloadManager->getDownloadingCount(), 0);
    ASSERT_EQ(m_downloadManager->getQueuedCount(), 0);
    ASSERT_EQ(m_downloadManager->getCompletedCount(), 3);
    ASSERT_TRUE(std::filesystem::exists(m_saveFolder / "Test1.mp3"));
    ASSERT_TRUE(std::filesystem::exists(m_saveFolder / "Test2.mp4"));
    ASSERT_TRUE(std::filesystem::exists(m_saveFolder / "Test3.flac"));
}