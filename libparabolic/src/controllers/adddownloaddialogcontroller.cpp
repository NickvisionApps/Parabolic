#include "controllers/adddownloaddialogcontroller.h"
#include <algorithm>
#include <format>
#include <thread>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "models/configuration.h"
#include "models/downloadoptions.h"
#include "models/urlinfo.h"

using namespace Nickvision::App;
using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    AddDownloadDialogController::AddDownloadDialogController(DownloadManager& downloadManager, DataFileManager& dataFileManager, Keyring::Keyring& keyring)
        : m_downloadManager{ downloadManager },
        m_previousOptions{ dataFileManager.get<PreviousDownloadOptions>("prev") },
        m_keyring{ keyring },
        m_urlInfo{ std::nullopt },
        m_credential{ std::nullopt },
        m_downloadImmediatelyAfterValidation{ dataFileManager.get<Configuration>("config").getDownloadImmediatelyAfterValidation() }
    {
        
    }

    AddDownloadDialogController::~AddDownloadDialogController()
    {
        m_previousOptions.save();   
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
        for(const Credential& credential : m_keyring.getCredentials())
        {
            names.push_back(credential.getName());
        }
        return names;
    }

    bool AddDownloadDialogController::getDownloadImmediatelyAfterValidation() const
    {
        return m_downloadImmediatelyAfterValidation;
    }

    bool AddDownloadDialogController::isUrlValid() const
    {
        return m_urlInfo && m_urlInfo->count() > 0;
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
        }
        return fileTypes;
    }

    std::vector<std::string> AddDownloadDialogController::getVideoFormatStrings(size_t* previousIndex) const
    {
        std::vector<std::string> formats;
        m_videoFormatMap.clear();
        if(previousIndex)
        {
            *previousIndex = 0;
        }
        if(!m_urlInfo)
        {
            return formats;
        }
        formats.push_back(_("Best"));
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            for(size_t i = 0; i < media.getFormats().size(); i++)
            {
                const Format& format{ media.getFormats()[i] };
                if(format.getType() == MediaType::Video)
                {
                    m_videoFormatMap[formats.size()] = i;
                    if(previousIndex && format.getId() == m_previousOptions.getVideoFormatId())
                    {
                        *previousIndex = formats.size();
                    }
                    formats.push_back(format.str());
                }
            }
        }
        return formats;
    }

    std::vector<std::string> AddDownloadDialogController::getAudioFormatStrings(size_t* previousIndex) const
    {
        std::vector<std::string> formats;
        m_audioFormatMap.clear();
        if(previousIndex)
        {
            *previousIndex = 0;
        }
        if(!m_urlInfo)
        {
            return formats;
        }
        formats.push_back(_("Best"));
        if(!m_urlInfo->isPlaylist())
        {
            const Media& media{ m_urlInfo->get(0) };
            for(size_t i = 0; i < media.getFormats().size(); i++)
            {
                const Format& format{ media.getFormats()[i] };
                if(format.getType() == MediaType::Audio)
                {
                    m_audioFormatMap[formats.size()] = i;
                    if(previousIndex && format.getId() == m_previousOptions.getAudioFormatId())
                    {
                        *previousIndex = formats.size();
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
            m_previousOptions.setNumberTitles(numbered);
            return numbered ? std::format("{} - {}", index + 1, title) : title;
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

    void AddDownloadDialogController::validateUrl(const std::string& url, const std::optional<Credential>& credential)
    {
        std::thread worker{ [this, url, credential]()
        {
            m_credential = credential;
            m_urlInfo = m_downloadManager.fetchUrlInfo(url, m_credential);
            m_urlValidated.invoke({ isUrlValid() });
        } };
        worker.detach();
    }

    void AddDownloadDialogController::validateUrl(const std::string& url, size_t credentialNameIndex)
    {
        if(credentialNameIndex < m_keyring.getCredentials().size())
        {
            validateUrl(url, m_keyring.getCredentials()[credentialNameIndex]);
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
            m_credential = credential;
            m_urlInfo = m_downloadManager.fetchUrlInfoFromBatchFile(batchFile, m_credential);
            m_urlValidated.invoke({ isUrlValid() });
        } };
        worker.detach();
    }

    void AddDownloadDialogController::validateBatchFile(const std::filesystem::path& batchFile, size_t credentialNameIndex)
    {
        if(credentialNameIndex < m_keyring.getCredentials().size())
        {
            validateBatchFile(batchFile, m_keyring.getCredentials()[credentialNameIndex]);
        }
        else
        {
            validateBatchFile(batchFile, std::nullopt);
        }
    }

    void AddDownloadDialogController::addSingleDownload(const std::filesystem::path& saveFolder, const std::string& filename, size_t fileTypeIndex, size_t videoFormatIndex, size_t audioFormatIndex, const std::vector<std::string>& subtitleLanguages, bool splitChapters, bool limitSpeed, bool exportDescription, const std::string& startTime, const std::string& endTime)
    {
        const Media& media{ m_urlInfo->get(0) };
        //Get Subtitle Languages
        std::vector<SubtitleLanguage> subtitles;
        for(const std::string& language : subtitleLanguages)
        {
            size_t autoGeneratedIndex{ language.find(" (") };
            subtitles.push_back({ language.substr(0, autoGeneratedIndex), autoGeneratedIndex != std::string::npos });
        }
        //Create Download Options
        DownloadOptions options{ media.getUrl() };
        options.setCredential(m_credential);
        if(media.getType() == MediaType::Audio)
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        options.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
        options.setAvailableFormats(m_urlInfo->get(0).getFormats());
        options.setSaveFolder(std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder());
        options.setSaveFilename(!filename.empty() ? StringHelpers::normalizeForFilename(filename, m_downloadManager.getDownloaderOptions().getLimitCharacters()) : media.getTitle());
        if(videoFormatIndex != 0)
        {
            options.setVideoFormat(media.getFormats()[m_videoFormatMap[videoFormatIndex]]);
        }
        if(audioFormatIndex != 0)
        {
            options.setAudioFormat(media.getFormats()[m_audioFormatMap[audioFormatIndex]]);
        }
        options.setSubtitleLanguages(subtitles);
        options.setSplitChapters(splitChapters);
        options.setLimitSpeed(limitSpeed);
        options.setExportDescription(exportDescription);
        std::optional<TimeFrame> timeFrame{ TimeFrame::parse(startTime, endTime, media.getTimeFrame().getDuration()) };
        if(timeFrame && media.getTimeFrame() != *timeFrame)
        {
            options.setTimeFrame(timeFrame);
        }
        //Save Previous Options
        m_previousOptions.setSaveFolder(options.getSaveFolder());
        m_previousOptions.setFileType(options.getFileType());
        m_previousOptions.setVideoFormatId(options.getVideoFormat() ? options.getVideoFormat()->getId() : "");
        m_previousOptions.setAudioFormatId(options.getAudioFormat() ? options.getAudioFormat()->getId() : "");
        m_previousOptions.setSplitChapters(options.getSplitChapters());
        m_previousOptions.setLimitSpeed(options.getLimitSpeed());
        m_previousOptions.setExportDescription(exportDescription);
        m_previousOptions.setSubtitleLanguages(options.getSubtitleLanguages());
        //Add Download
        m_downloadManager.addDownload(options);
    }

    void AddDownloadDialogController::addPlaylistDownload(const std::filesystem::path& saveFolder, const std::unordered_map<size_t, std::string>& filenames, size_t fileTypeIndex, bool splitChapters, bool limitSpeed, bool exportDescription)
    {
        //Save Previous Options
        m_previousOptions.setSaveFolder(saveFolder);
        m_previousOptions.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
        m_previousOptions.setSplitChapters(splitChapters);
        m_previousOptions.setLimitSpeed(limitSpeed);
        m_previousOptions.setExportDescription(exportDescription);
        std::filesystem::path playlistSaveFolder{ (std::filesystem::exists(saveFolder) ? saveFolder : m_previousOptions.getSaveFolder()) / StringHelpers::normalizeForFilename(m_urlInfo->getTitle(), m_downloadManager.getDownloaderOptions().getLimitCharacters()) };
        std::filesystem::create_directories(playlistSaveFolder);
        for(const std::pair<const size_t, std::string>& pair : filenames)
        {
            const Media& media{ m_urlInfo->get(pair.first) };
            //Create Download Options
            DownloadOptions options{ media.getUrl() };
            options.setCredential(m_credential);
            options.setFileType(static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex));
            options.setSaveFolder(playlistSaveFolder);
            options.setSaveFilename(!pair.second.empty() ? StringHelpers::normalizeForFilename(pair.second, m_downloadManager.getDownloaderOptions().getLimitCharacters()) : media.getTitle());
            options.setSplitChapters(splitChapters);
            options.setLimitSpeed(limitSpeed);
            options.setExportDescription(exportDescription);
            options.setPlaylistPosition(media.getPlaylistPosition());
            //Add Download
            m_downloadManager.addDownload(options);
        }
    }
}
