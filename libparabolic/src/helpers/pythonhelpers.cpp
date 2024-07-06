#include "helpers/pythonhelpers.h"
#include <sstream>

using namespace Nickvision::Logging;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Helpers
{
    static bool pythonStarted{ false };

    bool PythonHelpers::start(const Logger& logger)
    {
        if(pythonStarted)
        {
            return true;
        }
        try
        {
            logger.log(LogLevel::Debug, "Starting python interpreter...");
            py::initialize_interpreter();
            logger.log(LogLevel::Info, "Python interpreter started.");
            logger.log(LogLevel::Debug, "Loading yt-dlp python module...");
            py::module_ ytdlp{ py::module_::import("yt_dlp") };
            logger.log(LogLevel::Debug, "yt-dlp python module loaded.");
            pythonStarted = true;
        }
        catch(const std::exception& e)
        {
            logger.log(LogLevel::Error, "Unable to start python: " + std::string(e.what()));
        }
        return pythonStarted;
    }

    bool PythonHelpers::shutdown(const Logger& logger)
    {
        if(!pythonStarted)
        {
            return true;
        }
        try
        {
            logger.log(LogLevel::Debug, "Shutting down python interpreter...");
            py::finalize_interpreter();
            logger.log(LogLevel::Debug, "Python interpreter shut down.");
            pythonStarted = false;
        }
        catch(const std::exception& e)
        {
            logger.log(LogLevel::Error, "Unable to shut down python: " + std::string(e.what()));
        }
        return !pythonStarted;
    }

    bool PythonHelpers::started()
    {
        return pythonStarted;
    }

    py::object PythonHelpers::setConsoleOutputFile(const std::filesystem::path& path)
    {
        py::module_ sys{ py::module_::import("sys") };
        py::module_ io{ py::module_::import("io") };
        py::object file{ io.attr("open")(path.string(), "w") };
        sys.attr("stdout") = file;
        sys.attr("stderr") = file;
        return file;
    }

    std::string PythonHelpers::getDebugInformation()
    {
        if(!pythonStarted)
        {
            return "Python not started.";
        }
        std::stringstream builder;
        py::module_ sys{ py::module_::import("sys") };
        py::module_ ytdlp{ py::module_::import("yt_dlp") };
        builder << "Python: " << sys.attr("version").cast<std::string>() << std::endl;
        builder << "yt-dlp: " << ytdlp.attr("version").attr("__version__").cast<std::string>();
        return builder.str();
    }
}