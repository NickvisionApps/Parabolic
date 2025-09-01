#include "controllers/preferencesviewcontroller.h"
#include <algorithm>
#include <thread>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Localization;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Shared::Controllers
{
    PreferencesViewController::PreferencesViewController(Configuration& configuration, Models::DownloadHistory& downloadHistory)
        : m_configuration{ configuration },
        m_availableTranslationLanguages{ Gettext::getAvailableLanguages() },
        m_downloadHistory{ downloadHistory },
        m_options{ m_configuration.getDownloaderOptions() }
    {
        std::sort(m_availableTranslationLanguages.begin(), m_availableTranslationLanguages.end());
        m_availableTranslationLanguages.insert(m_availableTranslationLanguages.begin(), "en_US");
        m_availableTranslationLanguages.insert(m_availableTranslationLanguages.begin(), _("System"));
    }

    int PreferencesViewController::getMaxPostprocessingThreads() const
    {
        return static_cast<int>(std::thread::hardware_concurrency());
    }

    Theme PreferencesViewController::getTheme() const
    {
        return m_configuration.getTheme();
    }

    void PreferencesViewController::setTheme(Theme theme)
    {
        m_configuration.setTheme(theme);
    }

    const std::vector<std::string>& PreferencesViewController::getAvailableTranslationLanguages() const
    {
        return m_availableTranslationLanguages;
    }

    std::string PreferencesViewController::getTranslationLanguage() const
    {
        std::string language{ m_configuration.getTranslationLanguage() };
        if(language.empty())
        {
            return _("System");
        }
        else if(language == "C")
        {
            return "en_US";
        }
        return language;
    }

    void PreferencesViewController::setTranslationLanguage(const std::string& language)
    {
        if(language == _("System"))
        {
            m_configuration.setTranslationLanguage("");
        }
        else if(language == "en_US")
        {
            m_configuration.setTranslationLanguage("C");
        }
        else
        {
            m_configuration.setTranslationLanguage(language);
        }
    }

    void PreferencesViewController::setTranslationLanguage(size_t index)
    {
        if(index == 0 || index >= m_availableTranslationLanguages.size())
        {
            m_configuration.setTranslationLanguage("");
        }
        else if(index == 1)
        {
            m_configuration.setTranslationLanguage("C");
        }
        else
        {
            m_configuration.setTranslationLanguage(m_availableTranslationLanguages[index]);
        }
    }

    bool PreferencesViewController::getPreventSuspend() const
    {
        return m_configuration.getPreventSuspend();
    }

    void PreferencesViewController::setPreventSuspend(bool prevent)
    {
        m_configuration.setPreventSuspend(prevent);
    }

    const DownloaderOptions& PreferencesViewController::getDownloaderOptions() const
    {
        return m_options;
    }

    void PreferencesViewController::setDownloaderOptions(const DownloaderOptions& options)
    {
        m_options = options;
    }

    size_t PreferencesViewController::getHistoryLengthIndex() const
    {
        switch(m_downloadHistory.getLength())
        {
        case HistoryLength::Never:
            return 0;
        case HistoryLength::OneDay:
            return 1;
        case HistoryLength::OneWeek:
            return 2;
        case HistoryLength::OneMonth:
            return 3;
        case HistoryLength::ThreeMonths:
            return 4;
        case HistoryLength::Forever:
            return 5;
        default:
            return 2;
        }
    }

    void PreferencesViewController::setHistoryLengthIndex(size_t length)
    {
        switch(length)
        {
        case 0:
            m_downloadHistory.setLength(HistoryLength::Never);
            break;
        case 1:
            m_downloadHistory.setLength(HistoryLength::OneDay);
            break;
        case 2:
            m_downloadHistory.setLength(HistoryLength::OneWeek);
            break;
        case 3:
            m_downloadHistory.setLength(HistoryLength::OneMonth);
            break;
        case 4:
            m_downloadHistory.setLength(HistoryLength::ThreeMonths);
            break;
        case 5:
            m_downloadHistory.setLength(HistoryLength::Forever);
            break;
        default:
            m_downloadHistory.setLength(HistoryLength::OneWeek);
            break;
        }
    }

    const std::vector<std::string>& PreferencesViewController::getPostProcessorStrings() const
    {
        static std::vector<std::string> postProcessors;
        if(postProcessors.empty())
        {
            postProcessors.push_back(_("None"));
            postProcessors.push_back("Merger");
            postProcessors.push_back("ModifyChapters");
            postProcessors.push_back("SplitChapters");
            postProcessors.push_back("ExtractAudio");
            postProcessors.push_back("VideoRemuxer");
            postProcessors.push_back("VideoConverter");
            postProcessors.push_back("Metadata");
            postProcessors.push_back("EmbedSubtitle");
            postProcessors.push_back("EmbedThumbnail");
            postProcessors.push_back("SubtitlesConverter");
            postProcessors.push_back("ThumbnailsConverter");
            postProcessors.push_back("FixupStretched");
            postProcessors.push_back("FixupM4a");
            postProcessors.push_back("FixupM3u8");
            postProcessors.push_back("FixupTimestamp");
            postProcessors.push_back("FixupDuration");
        }
        return postProcessors;
    }

    const std::vector<std::string>& PreferencesViewController::getExecutableStrings() const
    {
        static std::vector<std::string> executables;
        if(executables.empty())
        {
            executables.push_back(_("None"));
            executables.push_back("AtomicParsley");
            executables.push_back("FFmpeg");
            executables.push_back("FFprobe");
        }
        return executables;
    }

    std::optional<PostProcessorArgument> PreferencesViewController::getPostprocessingArgument(const std::string& name) const
    {
        for(const PostProcessorArgument& argument : m_options.getPostprocessingArguments())
        {
            if(argument.getName() == name)
            {
                return argument;
            }
        }
        return std::nullopt;
    }

    PostProcessorArgumentCheckStatus PreferencesViewController::addPostprocessingArgument(const std::string& name, PostProcessor postProcessor, Executable executable, const std::string& args)
    {
        if(name.empty())
        {
            return PostProcessorArgumentCheckStatus::EmptyName;
        }
        else if(args.empty())
        {
            return PostProcessorArgumentCheckStatus::EmptyArgs;
        }
        std::vector<PostProcessorArgument> arguments{ m_options.getPostprocessingArguments() };
        for(const PostProcessorArgument& argument : arguments)
        {
            if(argument.getName() == name)
            {
                return PostProcessorArgumentCheckStatus::ExistingName;
            }
        }
        arguments.push_back({ name, postProcessor, executable, args });
        m_options.setPostprocessingArguments(arguments);
        return PostProcessorArgumentCheckStatus::Valid;
    }

    PostProcessorArgumentCheckStatus PreferencesViewController::updatePostprocessingArgument(const std::string& name, PostProcessor postProcessor, Executable executable, const std::string& args)
    {
        if(args.empty())
        {
            return PostProcessorArgumentCheckStatus::EmptyArgs;
        }
        std::vector<PostProcessorArgument> arguments{ m_options.getPostprocessingArguments() };
        for(std::vector<PostProcessorArgument>::iterator it = arguments.begin(); it != arguments.end(); ++it)
        {
            if(it->getName() == name)
            {
                it->setPostProcessor(postProcessor);
                it->setExecutable(executable);
                it->setArgs(args);
                m_options.setPostprocessingArguments(arguments);
                return PostProcessorArgumentCheckStatus::Valid;
            }
        }
        return PostProcessorArgumentCheckStatus::EmptyName;
    }

    bool PreferencesViewController::deletePostprocessingArgument(const std::string& name)
    {
        std::vector<PostProcessorArgument> arguments{ m_options.getPostprocessingArguments() };
        for(std::vector<PostProcessorArgument>::iterator it = arguments.begin(); it != arguments.end(); ++it)
        {
            if(it->getName() == name)
            {
                arguments.erase(it);
                m_options.setPostprocessingArguments(arguments);
                return true;
            }
        }
        return false;
    }

    void PreferencesViewController::saveConfiguration()
    {
        m_configuration.setDownloaderOptions(m_options);
        m_configuration.save();
    }
}
