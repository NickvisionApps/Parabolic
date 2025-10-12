#include "controllers/adddownloaddialogcontroller.h"
#include <format>
#include <thread>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <libnick/notifications/appnotification.h>
#include "models/downloadoptions.h"
#include "models/m3u.h"
#include "models/urlinfo.h"

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::Notifications;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(const std::filesystem::path& dataDirPath, DownloadManager& downloadManager, Keyring::Keyring& keyring)
        : m_downloadManager{ downloadManager },
        m_previousOptions{ dataDirPath / "prev.json" },
        m_keyring{ keyring },
        m_urlInfo{ std::nullopt },
        m_credential{ std::nullopt }
    {

    }

    AddDownloadDialogController::~AddDownloadDialogController()
    {
        m_previousOptions.save();
        m_validationCancellationToken.cancel();
        if(m_validationWorkerThread.joinable())
        {
            m_validationWorkerThread.join();
        }
    }

    Event<ParamEventArgs<bool>>& AddDownloadDialogController::urlValidated()
    {
        return m_urlValidated;
    }

    const PreviousDownloadOptions& AddDownloadDialogController::getPreviousDownloadOptions() const
    {
        return m_previousOptions;
    }

    std::vector<std::string> AddDownloadDialogController::getKeyringCredentialNames() const
    {
        std::vector<std::string> names;
        for(const Credential& credential : m_keyring.getAll())
        {
            names.push_back(credential.getName());
        }
        return names;
    }

    bool AddDownloadDialogController::isUrlPlaylist() const
    {
        return m_urlInfo && m_urlInfo->isPlaylist();
    }

    size_t AddDownloadDialogController::getMediaCount() const
    {
        return !m_urlInfo ? 0 : m_urlInfo->count();
    }

    std::vector<std::string> AddDownloadDialogController::getFileTypeStrings() const
    {
        std::vector<std::string> fileTypes;
        if(!m_urlInfo)
        {
            return fileTypes;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            if(media.getType() == MediaType::Video)
            {
                fileTypes.push_back(_("Video (Generic)"));
                fileTypes.push_back(_("MP4 (Video)"));
                fileTypes.push_back(_("WEBM (Video)"));
                fileTypes.push_back(_("MKV (Video)"));
                fileTypes.push_back(_("MOV (Video)"));
                fileTypes.push_back(_("AVI (Video)"));
            }
            fileTypes.push_back(_("Audio (Generic)"));
            fileTypes.push_back(_("MP3 (Audio)"));
            fileTypes.push_back(_("M4A (Audio)"));
            fileTypes.push_back(_("OPUS (Audio)"));
            fileTypes.push_back(_("FLAC (Audio)"));
            fileTypes.push_back(_("WAV (Audio)"));
            fileTypes.push_back(_("OGG (Audio)"));
        }
        else
        {
            fileTypes.push_back(_("Video (Generic)"));
            fileTypes.push_back(_("MP4 (Video)"));
            fileTypes.push_back(_("WEBM (Video)"));
            fileTypes.push_back(_("MKV (Video)"));
            fileTypes.push_back(_("MOV (Video)"));
            fileTypes.push_back(_("AVI (Video)"));
            fileTypes.push_back(_("Audio (Generic)"));
            fileTypes.push_back(_("MP3 (Audio)"));
            fileTypes.push_back(_("M4A (Audio)"));
            fileTypes.push_back(_("OPUS (Audio)"));
            fileTypes.push_back(_("FLAC (Audio)"));
            fileTypes.push_back(_("WAV (Audio)"));
            fileTypes.push_back(_("OGG (Audio)"));
        }
        return fileTypes;
    }

    std::vector<std::string> AddDownloadDialogController::getVideoFormatStrings(const MediaFileType& type, size_t& previousIndex) const
    {
        std::vector<std::string> formats;
        m_videoFormatMap.clear();
        previousIndex = 0;
        if(!m_urlInfo)
        {
            return formats;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            std::string previousId{ m_previousOptions.getVideoFormatId(type) };
            for(size_t i = 0; i < media.getFormats().size(); i++)
            {
                const Format& format{ media.getFormats()[i] };
                if(format.getType() == MediaType::Video)
                {
                    m_videoFormatMap[formats.size()] = i;
                    if(format.getId() == previousId)
                    {
                        previousIndex = formats.size();
                    }
                    formats.push_back(format.str());
                }
            }
        }
        return formats;
    }

    std::vector<std::string> AddDownloadDialogController::getAudioFormatStrings(const MediaFileType& type, size_t& previousIndex) const
    {
        std::vector<std::string> formats;
        m_audioFormatMap.clear();
        previousIndex = 0;
        if(!m_urlInfo)
        {
            return formats;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            std::string previousId{ m_previousOptions.getAudioFormatId(type) };
            for(size_t i = 0; i < media.getFormats().size(); i++)
            {
                const Format& format{ media.getFormats()[i] };
                if(format.getType() == MediaType::Audio)
                {
                    m_audioFormatMap[formats.size()] = i;
                    if(format.getId() == previousId)
                    {
                        previousIndex = formats.size();
                    }
                    formats.push_back(format.str());
                }
            }
        }
        return formats;
    }

    std::vector<std::string> AddDownloadDialogController::getSubtitleLanguageStrings() const
    {
        std::vector<std::string> languages;
        if(!m_urlInfo)
        {
            return languages;
        }
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            for(const SubtitleLanguage& language : media.getSubtitles())
            {
                languages.push_back(language.str());
            }
        }
        return languages;
    }

    std::vector<std::string> AddDownloadDialogController::getPostprocessingArgumentNames() const
    {
        std::vector<std::string> arguments;
        arguments.reserve(m_downloadManager.getDownloaderOptions().getPostprocessingArguments().size() + 1);
        arguments.push_back(_("None"));
        for(const PostProcessorArgument& argument : m_downloadManager.getDownloaderOptions().getPostprocessingArguments())
        {
            arguments.push_back(argument.getName());
        }
        return arguments;
    }

    const std::string& AddDownloadDialogController::getMediaUrl(size_t index) const
    {
        static std::string empty;
        if(m_urlInfo && index < m_urlInfo->count())
        {
            return m_urlInfo->get(index).getUrl();
        }
        return empty;
    }

    std::string AddDownloadDialogController::getMediaTitle(size_t index, bool numbered) const
    {
        static std::string empty;
        if(m_urlInfo && index < m_urlInfo->count())
        {
            std::string title{ m_urlInfo->get(index).getTitle() };
            return numbered ? std::format("{:02} - {}", index + 1, title) : title;
        }
        return empty;
    }

    const TimeFrame& AddDownloadDialogController::getMediaTimeFrame(size_t index) const
    {
        static TimeFrame empty{ std::chrono::seconds(0), std::chrono::seconds(0) };
        if(m_urlInfo && index < m_urlInfo->count())
        {
            return m_urlInfo->get(index).getTimeFrame();
        }
        return empty;
    }

    void AddDownloadDialogController::setPreviousNumberTitles(bool number)
    {
        m_previousOptions.setNumberTitles(number);
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, const std::optional<Credential>& credential)
    {
        m_validationCancellationToken.cancel();
        if(m_validationWorkerThread.joinable())
        {
            m_validationWorkerThread.join();
        }
        m_validationCancellationToken.reset();
        m_validationWorkerThread = std::thread{ [this, url, credential]()
        {
            m_urlInfo = std::nullopt;
            try
            {
                m_credential = credential;
                m_urlInfo = m_downloadManager.fetchUrlInfo(m_validationCancellationToken, url, m_credential);
                if(m_urlInfo)
                {
                    m_urlValidated.invoke({ true });
                }

            }
            catch(const std::runtime_error& e)
            {
                m_urlValidated.invoke({ false });
#ifdef _WIN32
                AppNotification::send({ _f("Error during yt-dlp validation:\n{}", e.what()), NotificationSeverity::Error, "error" });
#else
                AppNotification::send({ _f("Error during yt-dlp validation:\n<span font_family='monospace'>{}</span>", e.what()), NotificationSeverity::Error, "error" });
#endif
            }
            catch(const std::exception& e)
            {
                m_urlValidated.invoke({ false });
                AppNotification::send({ _f("Error during download validation: {}", e.what()), NotificationSeverity::Error, "error" });
            }
        } };
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, size_t credentialNameIndex)
    {
        if(credentialNameIndex < m_keyring.getAll().size())
        {
            validateUrl(url, m_keyring.getAll()[credentialNameIndex]);
        }
        else
        {
            validateUrl(url, std::nullopt);
        }
    }

    void AddDownloadDialogController::validateBatchFile(const std::filesystem::path& batchFile, const std::optional<Credential>& credential)
    {
        std::thread worker{ [this, batchFile, credential]()
        {
            m_urlInfo = std::nullopt;
            try
            {
                m_credential = credential;
                m_urlInfo = m_downloadManager.fetchUrlInfo(m_validationCancellationToken, batchFile, m_credential);
                if(m_urlInfo)
                {
                    m_urlValidated.invoke({ true });
                }
            }
            catch(const std::runtime_error& e)
            {
                m_urlValidated.invoke({ false });
#ifdef _WIN32
                AppNotification::send({ _f("Error during yt-dlp validation:\n{}", e.what()), NotificationSeverity::Error, "error" });
#else
                AppNotification::send({ _f("Error during yt-dlp validation:\n<span font_family='monospace'>{}</span>", e.what()), NotificationSeverity::Error, "error" });
#endif
            }
            catch(const std::exception& e)
            {
                m_urlValidated.invoke({ false });
                AppNotification::send({ _f("Error during download validation: {}", e.what()), NotificationSeverity::Error, "error" });
            }
        } };
        worker.detach();
    }

    void AddDownloadDialogController::validateBatchFile(const std::filesystem::path& batchFile, size_t credentialNameIndex)
    {
        if(credentialNameIndex < m_keyring.getAll().size())
        {
            validateBatchFile(batchFile, m_keyring.getAll()[credentialNameIndex]);
        }
        else
        {
            validateBatchFile(batchFile, std::nullopt);
        }
    }

    void AddDownloadDialogController::addSingleDownload(const std::filesystem::path& saveFolder, const std::string& filename, size_t fileTypeIndex, size_t videoFormatIndex, size_t audioFormatIndex, const std::vector<std::string>& subtitleLanguages, bool splitChapters, bool exportDescription, bool excludeFromHistory, size_t postProcessorArgumentIndex, const std::string& startTime, const std::string& endTime)
    {
        const Media& media{ m_urlInfo->get(0) };
        //Get Subtitle Languages
        std::vector<SubtitleLanguage> subtitles;
        for(const std::string& language : subtitleLanguages)
        {
            size_t autoGeneratedIndex{ language.find(" (") };
            subtitles.push_back({ language.substr(0, autoGeneratedIndex), autoGeneratedIndex != std::string::npos });
        }
        //Get Post Processor Argument
        std::optional<PostProcessorArgument> postProcessorArgument;
        if(postProcessorArgumentIndex > 0)
        {
            postProcessorArgument = m_downloadManager.getDownloaderOptions().getPostprocessingArguments()[postProcessorArgumentIndex  - 1];
        }
        //Create Download Options
        DownloadOptions options{ media.getUrl() };
        options.setCredential(m_credential);
        if(media.getType() == MediaType::Audio)
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex) };
        options.setFileType(type);
        options.setAvailableFormats(m_urlInfo->get(0).getFormats());
        options.setSaveFolder(std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder());
        options.setSaveFilename(!filename.empty() ? StringHelpers::normalizeForFilename(filename, m_downloadManager.getDownloaderOptions().getLimitCharacters()) : media.getTitle());
        options.setVideoFormat(media.getFormats()[m_videoFormatMap[videoFormatIndex]]);
        options.setAudioFormat(media.getFormats()[m_audioFormatMap[audioFormatIndex]]);
        options.setSubtitleLanguages(subtitles);
        options.setSplitChapters(splitChapters);
        options.setExportDescription(exportDescription);
        options.setPostProcessorArgument(postProcessorArgument);
        std::optional<TimeFrame> timeFrame{ TimeFrame::parse(startTime, endTime, media.getTimeFrame().getDuration()) };
        if(timeFrame && media.getTimeFrame() != *timeFrame)
        {
            options.setTimeFrame(timeFrame);
        }
        //Save Previous Options
        m_previousOptions.setSaveFolder(options.getSaveFolder());
        m_previousOptions.setFileType(type);
        if(media.hasVideoFormats())
        {
            m_previousOptions.setVideoFormatId(type, options.getVideoFormat() ? options.getVideoFormat()->getId() : "");
        }
        if(media.hasAudioFormats())
        {
            m_previousOptions.setAudioFormatId(type, options.getAudioFormat() ? options.getAudioFormat()->getId() : "");
        }
        m_previousOptions.setSplitChapters(options.getSplitChapters());
        m_previousOptions.setExportDescription(exportDescription);
        m_previousOptions.setPostProcessorArgument(options.getPostProcessorArgument() ? options.getPostProcessorArgument()->getName() : _("None"));
        m_previousOptions.setSubtitleLanguages(options.getSubtitleLanguages());
        //Add Download
        try
        {
            m_downloadManager.addDownload(options, excludeFromHistory);
        }
        catch(const std::exception& e)
        {
            AppNotification::send({ _f("Error attempting to add download: {}", e.what()), NotificationSeverity::Error, "error" });
        }
    }

    void AddDownloadDialogController::addPlaylistDownload(const std::filesystem::path& saveFolder, const std::map<size_t, std::string>& filenames, size_t fileTypeIndex, bool splitChapters, bool exportDescription, bool writePlaylistFile, bool excludeFromHistory, size_t postProcessorArgumentIndex)
    {
        M3U m3u{ m_urlInfo->getTitle(), m_urlInfo->hasSuggestedSaveFolder() ? PathType::Absolute : PathType::Relative };
        //Get Post Processor Argument
        std::optional<PostProcessorArgument> postProcessorArgument;
        if(postProcessorArgumentIndex > 0)
        {
            postProcessorArgument = m_downloadManager.getDownloaderOptions().getPostprocessingArguments()[postProcessorArgumentIndex  - 1];
        }
        //Save Previous Options
        m_previousOptions.setSaveFolder(saveFolder);
        m_previousOptions.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
        m_previousOptions.setSplitChapters(splitChapters);
        m_previousOptions.setExportDescription(exportDescription);
        m_previousOptions.setPostProcessorArgument(postProcessorArgument ? postProcessorArgument->getName() : _("None"));
        m_previousOptions.setWritePlaylistFile(writePlaylistFile);
        std::filesystem::path playlistSaveFolder{ (std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder()) / StringHelpers::normalizeForFilename(m_urlInfo->getTitle(), m_downloadManager.getDownloaderOptions().getLimitCharacters()) };
        std::filesystem::create_directories(playlistSaveFolder);
        for(const std::pair<const size_t, std::string>& pair : filenames)
        {
            const Media& media{ m_urlInfo->get(pair.first) };
            //Create Download Options
            DownloadOptions options{ media.getUrl() };
            options.setCredential(m_credential);
            options.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
            options.setAvailableFormats(media.getFormats());
            options.setSaveFolder(media.getSuggestedSaveFolder().empty() ? playlistSaveFolder : media.getSuggestedSaveFolder());
            options.setSaveFilename(!pair.second.empty() ? StringHelpers::normalizeForFilename(pair.second, m_downloadManager.getDownloaderOptions().getLimitCharacters()) : media.getTitle());
            options.setSplitChapters(splitChapters);
            options.setExportDescription(exportDescription);
            options.setPostProcessorArgument(postProcessorArgument);
            options.setPlaylistPosition(media.getPlaylistPosition());
            //Add Download
            try
            {
                m_downloadManager.addDownload(options, excludeFromHistory);
                m3u.add(options);
            }
            catch(const std::exception& e)
            {
                AppNotification::send({ _f("Error attempting to add download: {}", e.what()), NotificationSeverity::Error, "error" });
            }
        }
        //Write playlist file
        if(writePlaylistFile)
        {
            m3u.write(playlistSaveFolder / (playlistSaveFolder.filename().string() + ".m3u"));
        }
    }
}
