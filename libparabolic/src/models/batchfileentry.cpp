#include "models/batchfileentry.h"

namespace Nickvision::TubeConverter::Shared::Models
{
    BatchFileEntry::BatchFileEntry(const std::string& url)
        : m_url{ url }
    {

    }

    const std::string& BatchFileEntry::getUrl() const
    {
        return m_url;
    }

    const std::filesystem::path& BatchFileEntry::getSuggestedSaveFolder() const
    {
        return m_suggestedSaveFolder;
    }

    void BatchFileEntry::setSuggestedSaveFolder(const std::filesystem::path& suggestedSaveFolder)
    {
        m_suggestedSaveFolder = suggestedSaveFolder;
    }

    const std::string& BatchFileEntry::getSuggestedFilename() const
    {
        return m_suggestedFilename;
    }

    void BatchFileEntry::setSuggestedFilename(const std::string& suggestedFilename)
    {
        m_suggestedFilename = suggestedFilename;
    }
}