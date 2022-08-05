#pragma once

#include <string>
#include <optional>
#include <QMainWindow>
#include "Version.h"
#include "UpdateConfig.h"

namespace NickvisionTubeConverter::Update
{
	/// <summary>
	/// An object for updating an application
	/// </summary>
	class Updater
	{
    public:
        /// <summary>
        /// Constructs an Updater object
        /// </summary>
        /// <param name="linkToConfig">The url to an UpdateConfig file</param>
        /// <param name="currentVersion">The current Version for the running application</param>
        Updater(const std::string& linkToConfig, const Version& currentVersion);
        /// <summary>
        /// Gets whether or not an update is available (Must call checkForUpdates() first to check)
        /// </summary>
        /// <returns>True if an update is available, else false</returns>
        bool getUpdateAvailable() const;
        /// <summary>
        /// Gets the latest Version provided by the UpdateConfig file.
        /// </summary>
        /// <returns>The latest Version available</returns>
        Version getLatestVersion() const;
        /// <summary>
        /// Gets the changelog provided by the UpdateConfig file.
        /// </summary>
        /// <returns>The changelog of the latest version available</returns>
        std::string getChangelog() const;
        /// <summary>
        /// Gets if an update operation was successful or not.
        /// </summary>
        /// <returns>Ture if successful, else false</returns>
        bool getUpdateSuccessful() const;
        /// <summary>
        /// Checks if an update is available
        /// </summary>
        /// <returns>True if an update is available, else false</returns>
        bool checkForUpdates();
        /// <summary>
        /// Preforms an application update on a windows system.
        /// </summary>
        /// <param name="window">The wxWindow object to close when updating</param>
        /// <returns>True if the update was successful, else false. (Also returns false if system is not running Windows or if no update is available)</returns>
        bool windowsUpdate(QMainWindow* window);

    private:
        std::string m_linkToConfig;
        Version m_currentVersion;
        std::optional<UpdateConfig> m_updateConfig;
        bool m_updateAvailable;
        bool m_updateSuccessful;
	};
}