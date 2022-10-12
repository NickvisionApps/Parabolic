#pragma once

#include <functional>
#include <string>
#include "adddownloaddialogcontroller.hpp"
#include "preferencesdialogcontroller.hpp"
#include "../models/appinfo.hpp"
#include "../models/configuration.hpp"

namespace NickvisionTubeConverter::Controllers
{
	/**
	 * A controller for the MainWindow
	 */
	class MainWindowController
	{
	public:
		/**
		 * Constructs a MainWindowController
		 *
		 * @param appInfo The AppInfo for the application (Stored as a reference)
		 * @param configuration The Configuration for the application (Stored as a reference)
		 */
		MainWindowController(NickvisionTubeConverter::Models::AppInfo& appInfo, NickvisionTubeConverter::Models::Configuration& configuration);
		/**
		 * Gets the AppInfo object representing the application's information
		 *
		 * @returns The AppInfo object for the application
		 */
		const NickvisionTubeConverter::Models::AppInfo& getAppInfo() const;
		/**
		 * Gets whether or not the application version is a development version or not
		 *
		 * @returns True for development version, else false
		 */
		bool getIsDevVersion() const;
		/**
		 * Creates a AddDownloadDialogController
		 *
		 * @returns A new AddDownloadDialogController
		 */
		AddDownloadDialogController createAddDownloadDialogController() const;
		/**
		 * Creates a PreferencesDialogController
		 *
		 * @returns A new PreferencesDialogController
		 */
		PreferencesDialogController createPreferencesDialogController() const;
		/**
		 * Registers a callback for sending a toast notification on the MainWindow
		 *
		 * @param callback A void(const std::string&) function
		 */
		void registerSendToastCallback(const std::function<void(const std::string& message)>& callback);
		/**
		 * Runs startup functions
		 */
		void startup();

	private:
		NickvisionTubeConverter::Models::AppInfo& m_appInfo;
		NickvisionTubeConverter::Models::Configuration& m_configuration;
		bool m_isOpened;
		bool m_isDevVersion;
		std::function<void(const std::string& message)> m_sendToastCallback;
	};
}