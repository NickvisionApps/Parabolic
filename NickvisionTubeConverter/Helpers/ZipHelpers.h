#pragma once

#include <string>

/// <summary>
/// Functions for working with zip files
/// </summary>
namespace NickvisionTubeConverter::Helpers::ZipHelpers
{
	/// <summary>
	/// Extracts an entry from a zip file to a file on disk
	/// </summary>
	/// <param name="pathToZip">The path to the zip file on disk</param>
	/// <param name="pathOfEntry">The path of the entry relative to the zip file</param>
	/// <param name="pathToExtract">The path of the file to extract the entry on disk</param>
	/// <returns></returns>
	void extractEntryFromZip(const std::string& pathToZip, const std::string& pathOfEntry, const std::string& pathToExtract);
}

