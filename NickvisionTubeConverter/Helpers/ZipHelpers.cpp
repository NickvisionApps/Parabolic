#include "ZipHelpers.h"
#include <fstream>
#include <zip.h>

namespace NickvisionTubeConverter::Helpers
{
	void ZipHelpers::extractEntryFromZip(const std::string& pathToZip, const std::string& pathOfEntry, const std::string& pathToExtract)
	{
		//Open zip
		zip* zip{ zip_open(pathToZip.c_str(), 0, nullptr) };
		//Get stat of entry
		struct zip_stat stat;
		zip_stat_init(&stat);
		zip_stat(zip, pathOfEntry.c_str(), 0, &stat);
		//Read data from entry
		char* data{ new char[stat.size] };
		zip_file* entry{ zip_fopen(zip, pathOfEntry.c_str(), 0) };
		zip_fread(entry, data, stat.size);
		//Write data of entry to disk
		std::ofstream file{ pathToExtract, std::ios::binary };
		file.write(data, stat.size);
		//Cleanup
		delete[] data;
		zip_fclose(entry);
		zip_close(zip);
	}
}