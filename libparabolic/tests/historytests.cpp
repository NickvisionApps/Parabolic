#include <gtest/gtest.h>
#include <filesystem>
#include <libnick/app/datafilemanager.h>
#include <libnick/filesystem/userdirectories.h>
#include "models/downloadhistory.h"

using namespace Nickvision::App;
using namespace Nickvision::Filesystem;
using namespace Nickvision::TubeConverter::Shared::Models;

class HistoryTest : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        std::filesystem::remove_all(UserDirectories::get(ApplicationUserDirectory::Config, "libparabolic_test"));
        m_dataFileManager.get<DownloadHistory>("history");
    }

    static void TearDownTestCase()
    {
        std::filesystem::remove_all(UserDirectories::get(ApplicationUserDirectory::Config, "libparabolic_test"));
    }

    static DownloadHistory& get()
    {
        return m_dataFileManager.get<DownloadHistory>("history");
    }

private:
    static DataFileManager m_dataFileManager;
};

DataFileManager HistoryTest::m_dataFileManager{ "libparabolic_test" };

TEST_F(HistoryTest, EnsureDefaults)
{
    ASSERT_EQ(get().getHistory().size(), 0);
    ASSERT_EQ(get().getLength(), HistoryLength::OneWeek);
}

TEST_F(HistoryTest, AddDownload1)
{
    HistoricDownload download{ "https://www.youtube.com/watch?v=kdksdfhkjsdfhkjsdf" };
    download.setTitle("Test1");
    ASSERT_TRUE(get().addDownload(download));
    ASSERT_EQ(get().getHistory().size(), 1);
}

TEST_F(HistoryTest, AddDownload2)
{
    HistoricDownload download{ "https://www.youtube.com/watch?v=fjksdfhkjsdfhkjsdf" };
    download.setTitle("Test2");
    download.setDateTime(boost::posix_time::from_iso_string("20210101T000000"));
    ASSERT_FALSE(get().addDownload(download));
    ASSERT_EQ(get().getHistory().size(), 1);
}

TEST_F(HistoryTest, ChangeLengthForever)
{
    ASSERT_NO_THROW(get().setLength(HistoryLength::Forever));
    ASSERT_EQ(get().getLength(), HistoryLength::Forever);
    ASSERT_EQ(get().getHistory().size(), 1);
}

TEST_F(HistoryTest, AddDownload3)
{
    HistoricDownload download{ "https://www.youtube.com/watch?v=fjksdfhkjsdfhkjsdf" };
    download.setTitle("Test3");
    download.setDateTime(boost::posix_time::from_iso_string("20210101T000000"));
    ASSERT_TRUE(get().addDownload(download));
    ASSERT_EQ(get().getHistory().size(), 2);
}

TEST_F(HistoryTest, RemoveDownload)
{
    ASSERT_TRUE(get().removeDownload(get().getHistory()[1]));
}

TEST_F(HistoryTest, ChangeLengthNever)
{
    ASSERT_NO_THROW(get().setLength(HistoryLength::Never));
    ASSERT_EQ(get().getLength(), HistoryLength::Never);
    ASSERT_EQ(get().getHistory().size(), 0);
}