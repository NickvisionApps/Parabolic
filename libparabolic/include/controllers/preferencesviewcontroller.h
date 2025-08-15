#ifndef PREFERENCESVIEWCONTROLLER_H
#define PREFERENCESVIEWCONTROLLER_H

#include <filesystem>
#include <string>
#include "models/configuration.h"
#include "models/downloaderoptions.h"
#include "models/downloadhistory.h"
#include "models/postprocessorargumentcheckstatus.h"
#include "models/theme.h"

namespace Nickvision::TubeConverter::Shared::Controllers
{
    /**
     * @brief A controller for a PreferencesView.
     */
    class PreferencesViewController
    {
    public:
        /**
         * @brief Constructs a PreferencesViewController.
         * @param configuration The reference to the configuration to use
         * @param downloadHistory The reference to the download history to use
         */
        PreferencesViewController(Models::Configuration& configuration, Models::DownloadHistory& downloadHistory);
        /**
         * @brief Gets the maximum number of postprocessing threads allowed by the system.
         * @return The maximum number of postprocessing threads
         */
        int getMaxPostprocessingThreads() const;
        /**
         * @brief Gets the preferred theme for the application.
         * @return The preferred theme
         */
        Models::Theme getTheme() const;
        /**
         * @brief Sets the preferred theme for the application.
         * @param theme The new preferred theme
         */
        void setTheme(Models::Theme theme);
        /**
         * @brief Gets the available translation languages for the application.
         * @return The list of available translation languages
         */
        const std::vector<std::string>& getAvailableTranslationLanguages() const;
        /**
         * @brief Gets the preferred translation language for the application.
         * @return The preferred translation language
         */
        std::string getTranslationLanguage() const;
        /**
         * @brief Sets the preferred translation language for the application.
         * @param language The new preferred translation language
         */
        void setTranslationLanguage(const std::string& language);
        /**
         * @brief Sets the preferred translation language for the application.
         * @param index The index of the preferred translation language in the available languages list
         */
        void setTranslationLanguage(size_t index);
        /**
         * @brief Gets whether or not to prevent the system from suspending while Parabolic is running.
         * @return True to prevent the system from suspending, else false
         */
        bool getPreventSuspend() const;
        /**
         * @brief Sets whether or not to prevent the system from suspending while Parabolic is running.
         * @param prevent True to prevent the system from suspending, else false
         */
        void setPreventSuspend(bool prevent);
        /**
         * @brief Gets the downloader options.
         * @return The downloader options
         */
        const Models::DownloaderOptions& getDownloaderOptions() const;
        /**
         * @brief Sets the downloader options.
         * @param options The new downloader options
         */
        void setDownloaderOptions(const Models::DownloaderOptions& options);
        /**
         * @brief Gets the index of the selected download history length.
         * @return The download history length index
         */
        size_t getHistoryLengthIndex() const;
        /**
         * @brief Sets the index of the selected download history length.
         * @param length The new download history length index
         */
        void setHistoryLengthIndex(size_t length);
        /**
         * @brief Gets the list of post processor strings.
         * @return The list of post processor strings
         */
        const std::vector<std::string>& getPostProcessorStrings() const;
        /**
         * @brief Gets the list of executable strings.
         * @return The list of executable strings
         */
        const std::vector<std::string>& getExecutableStrings() const;
        /**
         * @brief Gets a postprocessing argument from the downloader options.
         * @param name The name of the argument to get
         * @return std::optional<PostProcessorArgument>
         */
        std::optional<Models::PostProcessorArgument> getPostprocessingArgument(const std::string& name) const;
        /**
         * @brief Adds a postprocessing argument to the downloader options.
         * @param name The name of the argument
         * @param postProcessor The post processor of the argument
         * @param executable The executable of the argument
         * @param args The args of the argument
         * @return PostProcessorArgumentCheckStatus
         */
        Models::PostProcessorArgumentCheckStatus addPostprocessingArgument(const std::string& name, Models::PostProcessor postProcessor, Models::Executable executable, const std::string& args);
        /**
         * @brief Updates a postprocessing argument in the downloader options.
         * @param name The name of the argument
         * @param postProcessor The post processor of the argument
         * @param executable The executable of the argument
         * @param args The args of the argument
         * @return PostProcessorArgumentCheckStatus
         */
        Models::PostProcessorArgumentCheckStatus updatePostprocessingArgument(const std::string& name, Models::PostProcessor postProcessor, Models::Executable executable, const std::string& args);
        /**
         * @brief Deletes a postprocessing argument from the downloader options.
         * @param name The name of the argument to delete
         * @return True if deleted
         * @return False if not deleted
         */
        bool deletePostprocessingArgument(const std::string& name);
        /**
         * @brief Saves the current configuration to disk.
         */
        void saveConfiguration();

    private:
        Models::Configuration& m_configuration;
        std::vector<std::string> m_availableTranslationLanguages;
        Models::DownloadHistory& m_downloadHistory;
        Models::DownloaderOptions m_options;
    };
}

#endif //PREFERENCESVIEWCONTROLLER_H
