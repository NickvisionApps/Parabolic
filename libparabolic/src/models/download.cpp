#include "models/download.h"
#include <libnick/filesystem/userdirectories.h>
#include <fstream>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>
#include "helpers/pythonhelpers.h"

using namespace Nickvision::Events;
using namespace Nickvision::Filesystem;
using namespace Nickvision::Helpers;
using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Helpers;
using namespace pybind11::literals;
namespace py = pybind11;

namespace Nickvision::TubeConverter::Shared::Models
{
    Download::Download(const DownloadOptions& options)
        : m_options{ options },
        m_id{ StringHelpers::newGuid() },
        m_status{ DownloadStatus::NotStarted },
        m_filename{ m_options.getSaveFilename() + (m_options.getFileType().isGeneric() ? "" : m_options.getFileType().getDotExtension()) },
        m_tempDirPath{ UserDirectories::get(UserDirectory::Cache) / m_id },
        m_logFilePath{ m_tempDirPath / "log.log" }
    {
        std::filesystem::create_directories(m_tempDirPath);
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

    Event<ParamEventArgs<DownloadStatus>>& Download::completed()
    {
        return m_completed;
    }

    std::filesystem::path Download::getPath() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_options.getSaveFolder() / m_filename;
    }

    DownloadStatus Download::getStatus() const
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        return m_status;
    }

    void Download::start(const DownloaderOptions& downloaderOptions)
    {
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(!PythonHelpers::started() || m_status == DownloadStatus::Running)
        {
            return;
        }
        m_status = DownloadStatus::Running;
        lock.unlock();
        m_downloadThread = std::thread{ &Download::runDownload, this, downloaderOptions };
    }

    void Download::stop()
    {
        if(!PythonHelpers::started() || m_status != DownloadStatus::Running)
        {
            return;
        }
        //TODO
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

    void Download::progressHook(const pybind11::dict& data)
    {
        //Get downloaded bytes
        double downloadedBytes{ 0.0 };
        double totalBytes{ 1.0 };
        if(data.contains("downloaded_bytes") && !data["downloaded_bytes"].is_none())
        {
            downloadedBytes = data["downloaded_bytes"].cast<double>();
        }
        if(data.contains("total_bytes") && !data["total_bytes"].is_none())
        {
            totalBytes = data["total_bytes"].cast<double>();
        }
        else if(data.contains("total_bytes_estimate") && !data["total_bytes_estimate"].is_none())
        {
            totalBytes = data["total_bytes_estimate"].cast<double>();
        }
        //Get download status
        DownloadProgressStatus status{ DownloadProgressStatus::Other };
        if(data.contains("status") && !data["status"].is_none())
        {
            std::string statusStr{ data["status"].cast<std::string>() };
            if(statusStr == "started" || statusStr == "finished" || statusStr == "processing")
            {
                status = DownloadProgressStatus::Processing;
            }
            else if(statusStr == "downloading")
            {
                status = DownloadProgressStatus::Downloading;
            }
        }
        //Calculate download progress
        double progress{ status == DownloadProgressStatus::Processing ? 1.0 : downloadedBytes / totalBytes };
        //Calculate download speed
        double speed{ 0.0 };
        if(data.contains("speed") && !data["speed"].is_none())
        {
            speed = data["speed"].cast<double>();
        }
        //Get download log
        std::string log;
        if(std::filesystem::exists(m_logFilePath))
        {
            std::ifstream logFile{ m_logFilePath };
            log = { std::istreambuf_iterator<char>(logFile), std::istreambuf_iterator<char>() };
        }
        //Invoke progress changed event
        m_progressChanged.invoke({ status, progress, speed, log });
    }

    void Download::runDownload(const DownloaderOptions& downloaderOptions)
    {
        //Check if can overwrite
        if(std::filesystem::exists(m_options.getSaveFolder() / m_filename) && !downloaderOptions.getOverwriteExistingFiles())
        {
            std::unique_lock<std::mutex> lock{ m_mutex };
            m_status = DownloadStatus::Error;
            lock.unlock();
            m_progressChanged.invoke({ DownloadProgressStatus::Other, 0.0, 0.0, _("File already exists and overwriting is disabled.") });
            m_completed.invoke(m_status);
            return;
        }
        //Setup logs
        py::object logFileHandle{ PythonHelpers::setConsoleOutputFile(m_logFilePath) };
        //Setup download params
        py::list postProcessors;
        std::vector<std::function<void(const py::dict&)>> hooks;
        hooks.push_back([this](const py::dict& data) { progressHook(data); });
        py::dict outtmpl;
        outtmpl["default"] = m_id + ".%(ext)s";
        outtmpl["chapter"] = "%(section_number)03d - " + m_id + ".%(ext)s";
        py::dict paths;
        paths["home"] = m_options.getSaveFolder().string();
        paths["temp"] = m_tempDirPath.string();
        py::dict ytdlpOptions;
        ytdlpOptions["quiet"] = false;
        ytdlpOptions["ignoreerrors"] = "downloadonly";
        ytdlpOptions["progress_hooks"] = hooks;
        ytdlpOptions["postprocessor_hooks"] = hooks;
        ytdlpOptions["merge_output_format"] = py::none();
        ytdlpOptions["outtmpl"] = outtmpl;
        ytdlpOptions["ffmpeg_location"] = Environment::findDependency("ffmpeg").string();
        ytdlpOptions["windowsfilenames"] = downloaderOptions.getLimitCharacters();
        ytdlpOptions["encoding"] = "utf_8";
        ytdlpOptions["overwrites"] = downloaderOptions.getOverwriteExistingFiles();
        ytdlpOptions["noprogress"] = true;
        ytdlpOptions["paths"] = paths;
        if(m_options.getCredential())
        {
            ytdlpOptions["username"] = m_options.getCredential()->getUsername();
            ytdlpOptions["password"] = m_options.getCredential()->getPassword();
        }
        //Aria2
        if(downloaderOptions.getUseAria())
        {
            py::list ariaParams;
            ariaParams.append("--max-overall-download-limit=" + std::to_string(m_options.getLimitSpeed() ? downloaderOptions.getSpeedLimit() : 0) + "K");
            ariaParams.append("--allow-overwrite=true");
            ariaParams.append("--show-console-readout=false");
            ariaParams.append("--max-connection-per-server=" + std::to_string(downloaderOptions.getAriaMaxConnectionsPerServer()));
            ariaParams.append("--min-split-size=" + std::to_string(downloaderOptions.getAriaMinSplitSize()) + "M");
            ytdlpOptions["external_downloader"] = py::dict{ "default"_a=Environment::findDependency("aria2c") };
            ytdlpOptions["external_downloader_args"] = py::dict{ "default"_a=ariaParams };
        }
        //Speed limit
        else if(m_options.getLimitSpeed())
        {
            ytdlpOptions["ratelimit"] = downloaderOptions.getSpeedLimit() * 1024;
        }
        //Proxy
        if(!downloaderOptions.getProxyUrl().empty())
        {
            ytdlpOptions["proxy"] = downloaderOptions.getProxyUrl();
        }
        //Cookies File
        if(std::filesystem::exists(downloaderOptions.getCookiesPath()))
        {
            ytdlpOptions["cookiefile"] = downloaderOptions.getCookiesPath().string();
        }
        //File Format
        if(!m_options.getFileType().isGeneric())
        {
            ytdlpOptions["final_ext"] = StringHelpers::lower(m_options.getFileType().str());
        }
        if(m_options.getFileType().isAudio())
        {
            if(m_options.getFileType().isGeneric())
            {
                ytdlpOptions["format"] = std::get<Quality>(m_options.getResolution()) != Quality::Worst ? "ba/b" : "wa/w";
                postProcessors.append(py::dict{ "key"_a="FFmpegExtractAudio", "preferredquality"_a=(std::get<Quality>(m_options.getResolution()) != Quality::Worst ? 0 : 5) });
            }
            else
            {
                std::string ext{ "[ext=" + StringHelpers::lower(m_options.getFileType().str()) + "]" };
                ytdlpOptions["format"] = std::get<Quality>(m_options.getResolution()) != Quality::Worst ? "ba" + ext + "/ba/b" : "wa" + ext + "/wa/w";
                postProcessors.append(py::dict{ "key"_a="FFmpegExtractAudio", "preferredcodec"_a=StringHelpers::lower(m_options.getFileType().str()), "preferredquality"_a=(std::get<Quality>(m_options.getResolution()) != Quality::Worst ? 0 : 5) });
            }
        }
        else if(m_options.getFileType().isVideo())
        {
            std::string ext{ "[ext=" + StringHelpers::lower(m_options.getFileType().str()) + "]" };
            std::string proto{ m_options.getTimeFrame() ? "[protocol!*=m3u8]" : "" };
            std::string resolution{ std::get<VideoResolution>(m_options.getResolution()).isBest() ? "" : "[width<=" + std::to_string(std::get<VideoResolution>(m_options.getResolution()).getWidth()) + "]" + "[height<=" + std::to_string(std::get<VideoResolution>(m_options.getResolution()).getHeight()) + "]" };
            std::vector<std::string> formats;
            formats.push_back("bv*" + ext + resolution + proto + "+ba" + ext);
            formats.push_back("b" + ext + resolution);
            formats.push_back("bv*" + resolution + proto + "+ba");
            formats.push_back("b" + resolution);
            ytdlpOptions["format"] = StringHelpers::join(formats, "/", false);
            //Prefer AV1
            if(m_options.getPreferAV1())
            {
                ytdlpOptions["format_sort"] = py::list{ "vcodec:av1"_s };
            }
            if(!m_options.getFileType().isGeneric())
            {
                postProcessors.append(py::dict{ "key"_a="FFmpegVideoConvert", "preferedformat"_a=StringHelpers::lower(m_options.getFileType().str()) });
            }
            //Subtitles
            if(m_options.getDownloadSubtitles())
            {
                ytdlpOptions["writesubtitles"] = true;
                ytdlpOptions["writeautomaticsub"] = downloaderOptions.getIncludeAutoGeneratedSubtitles();
                postProcessors.append(py::dict{ "key"_a="TCSubtitlesConvertor", "format"_a="vtt" });
                if(downloaderOptions.getEmbedSubtitles())
                {
                    postProcessors.append(py::dict{ "key"_a="TCEmbedSubtitle" });
                }
            }
        }
        //SponsorBlock
        if(downloaderOptions.getYouTubeSponsorBlock())
        {
            py::list categories;
            categories.append("sponsor");
            categories.append("intro");
            categories.append("outro");
            categories.append("selfpromo");
            categories.append("preview");
            categories.append("filler");
            categories.append("interaction");
            categories.append("music_offtopic");
            postProcessors.append(py::dict{ "key"_a="SponsorBlock", "when"_a="after_filter", "categories"_a=categories });
            postProcessors.append(py::dict{ "key"_a="ModifyChapters", "remove_sponsor_segments"_a=categories });
        }
        //Split Chapters
        if(m_options.getSplitChapters())
        {
            postProcessors.append(py::dict{ "key"_a="FFmpegSplitChapters" });
        }
        //Metadata & Chapters
        if(downloaderOptions.getEmbedMetadata())
        {
            py::dict ppDict;
            if(downloaderOptions.getRemoveSourceData())
            {
                py::list formats;
                formats.append(":(?P<meta_comment>)");
                formats.append(":(?P<meta_description>)");
                formats.append(":(?P<meta_synopsis>)");
                formats.append(":(?P<meta_purl>)");
                formats.append(std::to_string(m_options.getPlaylistPosition().value_or(0)) + ":%(meta_track)s");
                py::list rsdParams;
                rsdParams.append("-metadata:s");
                rsdParams.append("handler_name=");
                postProcessors.append(py::dict{ "key"_a="MetadataFromField", "formats"_a=formats });
                ppDict["tcmetadata"] = rsdParams;
            }
            // TCMetadata should be added after MetadataFromField
            postProcessors.append(py::dict{ "key"_a="TCMetadata", "add_metadata"_a=true, "add_chapters"_a=downloaderOptions.getEmbedChapters() });
            if(m_options.getFileType().supportsThumbnails())
            {
                ytdlpOptions["writethumbnail"] = true;
                postProcessors.append(py::dict{ "key"_a="TCEmbedThumbnail" });
                if(downloaderOptions.getCropAudioThumbnails())
                {
                    py::list cropParams;
                    cropParams.append("-vf");
                    cropParams.append("crop=\'if(gt(ih,iw),iw,ih)\':\'if(gt(iw,ih),ih,iw)\'");
                    ppDict["thumbnailsconvertor"] = cropParams;
                }
            }
            ytdlpOptions["postprocessor_args"] = ppDict;
        }
        else if(downloaderOptions.getEmbedChapters())
        {
            postProcessors.append(py::dict{ "key"_a="TCMetadata", "add_chapters"_a=true });
        }
        //Post Processors
        if(!postProcessors.empty())
        {
            ytdlpOptions["postprocessors"] = postProcessors;
        }
        //Run Download
        py::module_ ytdlp{ py::module_::import("yt_dlp") };
        if(downloaderOptions.getUseAria())
        {
            m_progressChanged.invoke({ DownloadProgressStatus::DownloadingAria, 0.0, 0.0, _("Download using aria2 has started.") });
        }
        if(m_options.getTimeFrame())
        {
            m_progressChanged.invoke({ DownloadProgressStatus::DownloadingFFmpeg, 0.0, 0.0, _("Download using ffmpeg has started.") });
        }
        py::object successCode{ ytdlp.attr("YoutubeDL")(ytdlpOptions).attr("download")(m_options.getUrl()) };
        invokeLogUpdate();
        std::unique_lock<std::mutex> lock{ m_mutex };
        if(!successCode.is_none())
        {
            m_status = successCode.cast<int>() == 0 ? DownloadStatus::Success : DownloadStatus::Error;
        }
        else
        {
            m_status = DownloadStatus::Error;
        }
        lock.unlock();
        logFileHandle.attr("close")();
        if(m_status == DownloadStatus::Success)
        {
            bool genericExtensionFound{ false };
            for(const std::filesystem::directory_entry& e : std::filesystem::directory_iterator(m_options.getSaveFolder()))
            {
                if(e.path().filename().string().find(m_id) != std::string::npos)
                {
                    if(m_options.getFileType().isGeneric() && !genericExtensionFound)
                    {
                        std::string extension{ StringHelpers::lower(e.path().extension().string()) };
                        if(extension != ".srt" && extension != ".vtt")
                        {
                            lock.lock();
                            m_filename += extension;
                            lock.unlock();
                            genericExtensionFound = true;
                        }
                    }
                    std::filesystem::rename(e.path(), m_options.getSaveFolder() / m_filename);
                }
            }
        }
        m_completed.invoke(m_status);
    }
}