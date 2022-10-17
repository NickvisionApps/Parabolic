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
		 * Gets whether or not to embed metadata in a download
		 *
		 * @returns True to embed, else false
		 */
		bool getEmbedMetadata() const;
		/**
		 * Sets whether or not to embed metadata in a download
		 *
		 * @param embedMetadata True to embed, else false
		 */
		void setEmbedMetadata(bool embedMetadata);
		/**
		 * Saves the configuration file
		 */
		void saveConfiguration() const;

	private:
		NickvisionTubeConverter::Models::Configuration& m_configuration;
	};
}