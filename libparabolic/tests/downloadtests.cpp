#include <gtest/gtest.h>
#include <libnick/filesystem/userdirectories.h>
#include "helpers/pythonhelpers.h"
#include "models/download.h"
#include "models/downloadoptions.h"
#include "models/downloaderoptions.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Logging;
using namespace Nickvision::TubeConverter::Shared::Helpers;
using namespace Nickvision::TubeConverter::Shared::Models;

class DownloadTest : public ::testing::Test
{
public:
    static Logger m_logger;
    static DownloaderOptions m_downloaderOptions;

    static void SetUpTestSuite()
    {
        PythonHelpers::start(m_logger);
    }

    static void TearDownTestSuite()
    {
        PythonHelpers::shutdown(m_logger);
    }
};

Logger DownloadTest::m_logger;
DownloaderOptions DownloadTest::m_downloaderOptions;

TEST_F(DownloadTest, Download1)
{
    GTEST_SKIP();
    DownloadOptions options{ "https://www.youtube.com/watch?v=CvUxi35IdWM&pp=ygUTY29weXJpZ2h0IGZyZWUgc29uZw%3D%3D" };
    options.setFileType(MediaFileType::MP3);
    options.setSaveFolder(UserDirectories::get(UserDirectory::Downloads));
    options.setSaveFilename("Test1");
    Download download{ options };
    ASSERT_EQ(download.getStatus(), DownloadStatus::NotStarted);
    download.progressChanged() += [](const DownloadProgressChangedEventArgs& args)
    {
        std::cout << args << std::endl;
    };
    download.completed() += [](const ParamEventArgs<DownloadStatus>& args)
    {
        std::cout << "Done!" << std::endl;
    };
    download.start(m_downloaderOptions);
    while(download.getStatus() == DownloadStatus::Running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    ASSERT_EQ(download.getStatus(), DownloadStatus::Success);
}