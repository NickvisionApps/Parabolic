#pragma once

#include <string>
#include "../models/configuration.hpp"
#include "../models/download.hpp"
#include "../models/mediafiletype.hpp"

namespace NickvisionTubeConverter::Controllers
{
	enum class DownloadCheckStatus
	{
		Valid = 0,
		EmptyVideoUrl,
		InvalidVideoUrl,
		EmptySaveFolder,
		InvalidSaveFolder,
		EmptyNewFilename
	};

	class AddDownloadDialogController
	{
	public:
		AddDownloadDialogController(NickvisionTubeConverter::Models::Configuration& configuration);
		const std::string& getResponse() const;
		void setResponse(const std::string& response);
		std::string getPreviousSaveFolder() const;
		int getPreviousFileTypeAsInt() const;
		const NickvisionTubeConverter::Models::Download& getDownload() const;
		DownloadCheckStatus checkIfDownloadValid() const;
		DownloadCheckStatus setDownload(const std::string& videoUrl, int mediaFileType, const std::string& saveFolder, const std::string& newFilename, int quality);

	private:
		NickvisionTubeConverter::Models::Configuration& m_configuration;
		std::string m_response;
		NickvisionTubeConverter::Models::Download m_download;
	};
}