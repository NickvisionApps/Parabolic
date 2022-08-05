#pragma once

#include <string>

namespace NickvisionTubeConverter::Models
{
	/// <summary>
	/// A model for an application's information
	/// </summary>
	class AppInfo
	{
	public:
		AppInfo(const AppInfo&) = delete;
		void operator=(const AppInfo&) = delete;
		/// <summary>
		/// Gets the AppInfo singleton object
		/// </summary>
		/// <returns>A reference to the AppInfo object</returns>
		static AppInfo& getInstance();
		/// <summary>
		/// Gets the name of the application
		/// </summary>
		/// <returns>The name of the application</returns>
		const std::string& getName() const;
		/// <summary>
		/// Sets the name of the application
		/// </summary>
		/// <param name="name">The name of the application</param>
		void setName(const std::string& name);
		/// <summary>
		/// Gets the description of the application
		/// </summary>
		/// <returns>The description of the application</returns>
		const std::string& getDescription() const;
		/// <summary>
		/// Sets the description of the application
		/// </summary>
		/// <param name="description">The description of the application</param>
		void setDescription(const std::string& description);
		/// <summary>
		/// Gets the version of the application
		/// </summary>
		/// <returns>The version of the application</returns>
		const std::string& getVersion() const;
		/// <summary>
		/// Sets the version of the application
		/// </summary>
		/// <param name="version">The version of the application</param>
		void setVersion(const std::string& version);
		/// <summary>
		/// Gets the changelog of the application
		/// </summary>
		/// <returns>The changelog of the application</returns>
		const std::string& getChangelog() const;
		/// <summary>
		/// Sets the changelog of the application
		/// </summary>
		/// <param name="changelog">The changelog of the application</param>
		void setChangelog(const std::string& changelog);
		/// <summary>
		/// Gets the GitHub repo of the application
		/// </summary>
		/// <returns>The GitHub repo of the application</returns>
		const std::string& getGitHubRepo() const;
		/// <summary>
		/// Sets the GitHub repo of the application
		/// </summary>
		/// <param name="gitHubRepo">The GtHub repo of the application</param>
		void setGitHubRepo(const std::string& gitHubRepo);
		/// <summary>
		/// Gets the issue tracker of the application
		/// </summary>
		/// <returns>The issue tracker of the application</returns>
		const std::string& getIssueTracker() const;
		/// <summary>
		/// Sets the issue tracker of the application
		/// </summary>
		/// <param name="issueTracker">The issue tracker of the application</param>
		void setIssueTracker(const std::string& issueTracker);

	private:
		/// <summary>
		/// Constructs an AppInfo object
		/// </summary>
		AppInfo();
		std::string m_name;
		std::string m_description;
		std::string m_version;
		std::string m_changelog;
		std::string m_gitHubRepo;
		std::string m_issueTracker;
	};
}

