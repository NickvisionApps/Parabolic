#include "models/batchfile.h"
#include <fstream>
#include <string>
#include <libnick/helpers/stringhelpers.h>

#define BATCH_FILE_ENTRY_DELIMITER '|'

using namespace Nickvision::Helpers;

namespace Nickvision::TubeConverter::Shared::Models
{
    static std::string& trimQuotes(std::string& str)
    {
        str = StringHelpers::trim(str);
        str = StringHelpers::trim(str, '"');
        str = StringHelpers::trim(str);
        return str;
    }

    BatchFile::BatchFile(const std::filesystem::path& path)
        : m_path{ path }
    {
        if(!std::filesystem::exists(m_path) || m_path.extension() != ".txt")
        {
            return;
        }
        std::ifstream file{ m_path };
        if(!file.is_open())
        {
            return;
        }
        std::string line;
        while(std::getline(file, line))
        {
            if(line.find(BATCH_FILE_ENTRY_DELIMITER) != std::string::npos)
            {
                std::vector<std::string> fields{ StringHelpers::split(line, BATCH_FILE_ENTRY_DELIMITER, false) };
                if(fields.size() < 1 || fields.size() > 3)
                {
                    continue;
                }
                trimQuotes(fields[0]);
                if(!StringHelpers::isValidUrl(fields[0]))
                {
                    continue;
                }
                BatchFileEntry entry{ fields[0] };
                if(fields.size() >= 2)
                {
                    trimQuotes(fields[1]);
                    if(!fields[1].empty())
                    {
                        std::filesystem::path suggestedSaveFolder{ fields[1] };
                        if(suggestedSaveFolder.is_absolute())
                        {
                            entry.setSuggestedSaveFolder(suggestedSaveFolder);
                        }
                    }
                }
                if(fields.size() == 3)
                {
                    trimQuotes(fields[2]);
                    if(!fields[2].empty())
                    {
                        entry.setSuggestedFilename(fields[2]);
                    }
                }
                m_entries.push_back(entry);
            }
            else
            {
                trimQuotes(line);
                if(!StringHelpers::isValidUrl(line))
                {
                    continue;
                }
                m_entries.push_back({ line });
            }
        }
    }

    const std::filesystem::path& BatchFile::getPath() const
    {
        return m_path;
    }

    const std::vector<BatchFileEntry>& BatchFile::getEntries() const
    {
        return m_entries;
    }
}