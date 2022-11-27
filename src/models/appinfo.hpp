#pragma once

#include <string>

namespace NickvisionTubeConverter::Models
{
	/**
	 * A model for the information of an application
	 */
	class AppInfo
	{
	public:
		/**
		 * Constructs an AppInfo
		 */
		AppInfo();
		/**
		 * Gets the id of the application
		 *
		 * @returns The id of the application
		 */
		const std::string& getId() const;
		/**
		 * Sets the id of the application
		 *
		 * @param id The new id of the application
		 */
		void setId(const std::string& id);
		/**
		 * Gets the name of the application
		 *
		 * @returns The name of the application
		 */
		const std::string& getName() const;
		/**
		 * Sets the name of the application
		 *
		 * @param name The new name of the application
		 */
		void setName(const std::string& name);
		/**
		 * Gets the short name of the application
		 *
		 * @returns The short name of the application
		 */
		const std::string& getShortName() const;
		/**
		 * Sets the short name of the application
		 *
		 * @param NickvisionTubeConverter The new short name of the application
		 */
		void setShortName(const std::string& NickvisionTubeConverter);
		/**
		 * Gets the description of the application
		 *
		 * @returns The description of the application
		 */
		const std::string& getDescription() const;
		/**
		 * Sets the description of the application
		 *
		 * @param description The new description of the application
		 */
		void setDescription(const std::string& description);
		/**
		 * Gets the version string of the application
		 *
		 * @returns The version string of the application
		 */
		const std::string& getVersion() const;
		/**
		 * Sets the version string of the application
		 *
		 * @param version The new version string of the application
		 */
		void setVersion(const std::string& version);
		/**
		 * Gets the changelog of the application
		 *
		 * @returns The changelog of the application
		 */
		const std::string& getChangelog() const;
		/**
		 * Sets the changelog of the application
		 *
		 * @param changelog The new changelog of the application
		 */
		void setChangelog(const std::string& changelog);
		/**
		 * Gets the GitHub Repo link of the application
		 *
		 * @returns The GitHub Repo link of the applicatiion
		 */
		const std::string& getGitHubRepo() const;
		/**
		 * Sets the GitHub Repo link of the application
		 *
		 * @param gitHubRepo The new GitHub Repo link of the application
		 */
		void setGitHubRepo(const std::string& gitHubRepo);
		/**
		 * Gets the issue tracker link of the appplication
		 *
		 * @returns The issue tracker link of the application
		 */
		const std::string& getIssueTracker() const;
		/**
		 * Sets the issue tracker link of the application
		 *
		 * @param issueTracker The new issue tracker link of the application
		 */
		void setIssueTracker(const std::string& issueTracker);
		/**
		 * Gets the support url of the appplication
		 *
		 * @returns The support url of the application
		 */
		const std::string& getSupportUrl() const;
		/**
		 * Sets the support url of the application
		 *
		 * @param supportUrl The new support url of the application
		 */
		void setSupportUrl(const std::string& supportUrl);
		/**
		 * Gets the list of translators
		 *
		 * @returns The list of translators
		 */
		const std::string& getTranslatorCredits() const;
		/**
		 * Generate the list of translators from po/CREDITS.json file
		 */
		void generateTranslatorCredits();

	private:
		std::string m_id;
		std::string m_name;
		std::string m_NickvisionTubeConverter;
		std::string m_description;
		std::string m_version;
		std::string m_changelog;
		std::string m_gitHubRepo;
		std::string m_issueTracker;
		std::string m_supportUrl;
		std::string m_translatorCredits;
	};
}