#include "helpers/pythonhelpers.h"
#include <fstream>
#include <sstream>
#include <libnick/filesystem/userdirectories.h>

using namespace Nickvision::Filesystem;
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
        //Write yt-dlp plugin
        std::filesystem::path pluginPath{ UserDirectories::get(UserDirectory::Config) / "yt-dlp" / "plugins" / "tubeconverter" / "yt_dlp_plugins" / "postprocessor" / "tubeconverter.py" };
        std::filesystem::create_directories(pluginPath.parent_path());
        std::ofstream pluginFile{pluginPath, std::ios::trunc };
        pluginFile << R"(#!/usr/bin/env python3
from yt_dlp.postprocessor.ffmpeg import FFmpegMetadataPP, FFmpegSubtitlesConvertorPP, FFmpegEmbedSubtitlePP
from yt_dlp.postprocessor.embedthumbnail import EmbedThumbnailPP
import os, re


class TCMetadataPP(FFmpegMetadataPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
            self.to_screen(e)
            self.to_screen('WARNING: Failed to embed metadata')
            return [], info


class TCSubtitlesConvertorPP(FFmpegSubtitlesConvertorPP):
    def run(self, info):
        # Remove styling from VTT files before processing
        # https://trac.ffmpeg.org/ticket/8684
        subs = info.get('requested_subtitles')
        if subs is not None:
            for _, sub in subs.items():
                if os.path.exists(sub.get('filepath', '')):
                    with open(sub.get('filepath', ''), 'r') as f:
                        data = f.read()
                    data = re.sub(r'^STYLE\n(::cue.*\{\n*[^}]*\}\n+)+', '', data, flags=re.MULTILINE)
                    with open(sub.get('filepath', ''), 'w') as f:
                        f.write(data)
        return super().run(info)


class TCEmbedSubtitlePP(FFmpegEmbedSubtitlePP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
          self.to_screen(e)
          self.to_screen('WARNING: Failed to embed subtitles')
          return [], info


class TCEmbedThumbnailPP(EmbedThumbnailPP):
    def run(self, info):
        try:
            success = super().run(info)
            return success
        except Exception as e:
            self.to_screen(e)
            self.to_screen('WARNING: Failed to embed thumbnail')
            idx = next((-i for i, t in enumerate(info['thumbnails'][::-1], 1) if t.get('filepath')), None)
            thumbnail_filename = info['thumbnails'][idx]['filepath']
            self._delete_downloaded_files(thumbnail_filename, info=info)
            return [], info
)";
        pluginFile << std::endl;
        //Start python
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