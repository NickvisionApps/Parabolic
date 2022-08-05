#pragma once

#include <string>

namespace NickvisionTubeConverter::Update
{
	/// <summary>
	/// Represents an application version in the format: x.y.z
	/// </summary>
	class Version
	{
	public:
		/// <summary>
		/// Constructs a Version object
		/// </summary>
		/// <param name="version">The string representation of the version: "x.y.z"</param>
		Version(const std::string& version);
		/// <summary>
		/// Gets the string representation of the version
		/// </summary>
		/// <returns>The string representation of the version</returns>
		std::string toString() const;
		/// <summary>
		/// Compares two Version objects for equality
		/// </summary>
		/// <param name="toCompare">The Version object to compare</param>
		/// <returns>True if both versions are equal, else false</returns>
		bool operator==(const Version& toCompare) const;
		/// <summary>
		/// Compares two Version objects for not equality
		/// </summary>
		/// <param name="toCompare">The Version object to compare</param>
		/// <returns>True if both versions are not equal, else false</returns>
		bool operator!=(const Version& toCompare) const;
		/// <summary>
		/// Compares two Version objects for less than
		/// </summary>
		/// <param name="toCompare">The Version object to compare</param>
		/// <returns>True if this Version is less than toCompare, else false</returns>
		bool operator<(const Version& toCompare) const;
		/// <summary>
		/// Compares two Version objects for greater than
		/// </summary>
		/// <param name="toCompare">The Version object to compare</param>
		/// <returns>True if this Version is greater than toCompare, else false</returns>
		bool operator>(const Version& toCompare) const;

	private:
		int m_major;
		int m_minor;
		int m_build;
	};
}

