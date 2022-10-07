#pragma once

#include "../models/configuration.hpp"

namespace NickvisionTubeConverter::Controllers
{
	/**
	 * A controller for PreferencesDialog
	 */
	class PreferencesDialogController
	{
	public:
		/**
		 * Constructs a PreferencesDialogController
		 *
		 * @param configuration The configuration fot the application (Stored as reference)
		 */
		PreferencesDialogController(NickvisionTubeConverter::Models::Configuration& configuration);
		/**
		 * Gets the theme from the configuration as an int
		 *
		 * @returns The theme from the configuration as an int
		 */
		int getThemeAsInt() const;
		/**
		 * Sets the theme in the configuration
		 *
		 * @param theme The new theme as an int
		 */
		void setTheme(int theme);
		/**
		 * Gets whether or not the application is first time open from configuration
		 *
		 * @returns True for first time open, else false
		 */
		bool getIsFirstTimeOpen() const;
		/**
		 * Sets whether or not the application is first time open in the configuration
		 *
		 * @param isFirstTimeOpen True for is first time open, else false
		 */
		void setIsFirstTimeOpen(bool isFirstTimeOpen);
		/**
		 * Saves the configuration file
		 */
		void saveConfiguration() const;

	private:
		NickvisionTubeConverter::Models::Configuration& m_configuration;
	};
}