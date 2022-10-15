#pragma once

#include <memory>
#include <string>
#include "../models/configuration.hpp"
#include "../models/download.hpp"
#include "../models/mediafiletype.hpp"

namespace NickvisionTubeConverter::Controllers
{
	/**
	 * A controller for the AddDownloadDialog
	 */
	class AddDownloadDialogController
	{
	public:
		/**
		 * Constructs an AddDownloadDialogController
		 *
		 * @param configuration The Configuration for the application (Stored as a reference)
		 */
		AddDownloadDialogController(NickvisionTubeConverter::Models::Configuration& configuration);
		/**
		 * Gets the response of the dialog
		 *
		 * @returns The response of the dialog
		 */
		const std::string& getResponse() const;
		/**
		 * Sets the response of the dialog
		 *
		 * @param response The new response of the dialog
		 */
		void setResponse(const std::string& response);
		/**
		 * Gets the previously used saved folder from the configuration
		 *
		 * @returns The previously used saved folder
		 */
		std::string getPreviousSaveFolder() const;
		/**
		 * Gets the previously used file type (as an int) from the configuration
		 *
		 * @returns The previously used file type (as an int)
		 */
		int getPreviousFileTypeAsInt() const;
		/**
		 * Gets the download created by the dialog
		 *
		 * @returns The download created by the dialog
		 */
		const std::shared_ptr<NickvisionTubeConverter::Models::Download>& getDownload() const;
		/**
		 * Sets the download from the dialog and checks if it is valid
		 *
		 * @returns The DownloadCheckStatus
		 */
		NickvisionTubeConverter::Models::DownloadCheckStatus setDownload(const std::string& videoUrl, int mediaFileType, const std::string& saveFolder, const std::string& newFilename, int quality);

	private:
		NickvisionTubeConverter::Models::Configuration& m_configuration;
		std::string m_response;
		std::shared_ptr<NickvisionTubeConverter::Models::Download> m_download;
	};
}