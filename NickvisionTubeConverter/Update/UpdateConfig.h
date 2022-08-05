#pragma once

#include <string>
#include <optional>
#include "Version.h"

namespace NickvisionTubeConverter::Update
{
	/// <summary>
	/// A model for the configuration of an update
	/// </summary>
	class UpdateConfig
	{
	public:
		/// <summary>
		/// Downloads an update configuration from the internet
		/// </summary>
		/// <param name="url">The URL of the update config file</param>
		/// <returns>An UpdateConfig object based off the downloaded file. If error downloading, returns std::nullopt</returns>
		static std::optional<UpdateConfig> loadFromUrl(const std::string& url);
		/// <summary>
		/// Gets the version stored in the UpdateConfig
		/// </summary>
		/// <returns>The version</returns>
		const Version& getLatestVersion() const;
		/// <summary>
		/// Gets the changelog stored in the UpdateConfig
		/// </summary>
		/// <returns>The changelog</returns>
		const std::string& getChangelog() const;
		/// <summary>
		/// Gets the link to the setup exe stored in the updateConfig
		/// </summary>
		/// <returns>The link to the setup exe</returns>
		const std::string& getLinkToSetupExe() const;

	private:
		/// <summary>
		/// Constructs an UpdateConfig object
		/// </summary>
		UpdateConfig();
		Version m_latestVersion;
		std::string m_changelog;
		std::string m_linkToSetupExe;
	};
}

