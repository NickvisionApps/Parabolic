#include "models/download.h"
#include <libnick/filesystem/userdirectories.h>
#include <fstream>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "helpers/pythonhelpers.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::TubeConverter::Shared::Helpers;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Models
{
    Download::Download(const DownloadOptions& options)
        : m_options{ options },
        m_id{ StringHelpers::newGuid() },
        m_tempDirPath{ UserDirectories::get(UserDirectory::Cache) / m_id },
        m_logFilePath{ m_tempDirPath / "log.log" },
        m_isRunning{ false },
        m_isDone{ false },
        m_isSuccess{ false },
        m_wasStopped{ false },
        m_filename{ m_options.getSaveFilename() + (m_options.getFileType().isGeneric() ? "" : m_options.getFileType().getDotExtension()) }
    {

    }

    Download::~Download()
    {
        stop();
        std::filesystem::remove_all(m_tempDirPath);
    }

    Event<DownloadProgressChangedEventArgs>& Download::progressChanged()
    {
        return m_progressChanged;
    }

    Event<ParamEventArgs<bool>>& Download::completed()
    {
        return m_completed;
    }

    bool Download::start(const DownloaderOptions& downloaderOptions)
    {
        if(!PythonHelpers::started())
        {
            return false;
        }
        if(m_isRunning)
        {
            return true;
        }
        m_isRunning = true;
        m_isDone = false;
        m_isSuccess = false;
        m_wasStopped = false;
        //Check if can overwrite
        if(std::filesystem::exists(m_options.getSaveFolder() / m_filename) && !downloaderOptions.getOverwriteExistingFiles())
        {
            m_progressChanged.invoke({ DownloadProgressStatus::Other, 0.0, 0.0, _("File already exists and overwriting is disabled.") });
            m_isRunning = false;
            m_isDone = true;
            m_completed.invoke(m_isSuccess);
            return false;
        }
        //Setup logs
        std::filesystem::create_directories(m_tempDirPath);
        m_logFileHandle = PythonHelpers::setConsoleOutputFile(m_logFilePath);
        //Setup download params
        return m_isRunning;
    }

    bool Download::stop()
    {
        if(!PythonHelpers::started())
        {
            return false;
        }
        if(!m_isRunning)
        {
            return true;
        }
        return !m_isRunning;
    }

    void Download::invokeLogUpdate()
    {
        std::string log;
        if(std::filesystem::exists(m_logFilePath))
        {
            std::ifstream logFile{ m_logFilePath };
            log = { std::istreambuf_iterator<char>(logFile), std::istreambuf_iterator<char>() };
        }
        m_progressChanged.invoke({ DownloadProgressStatus::Other, 0.0, 0.0, log });
    }

    void Download::progressHook()
    {

    }
}