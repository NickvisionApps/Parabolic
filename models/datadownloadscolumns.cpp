#include "datadownloadscolumns.h"

namespace NickvisionTubeConverter::Models
{
    DataDownloadsColumns::DataDownloadsColumns()
    {
        add(m_colID);
        add(m_colPath);
        add(m_colFileType);
        add(m_colUrl);
    }

    const Gtk::TreeModelColumn<unsigned int>& DataDownloadsColumns::getColID() const
    {
        return m_colID;
    }

    const Gtk::TreeModelColumn<std::string>& DataDownloadsColumns::getColPath() const
    {
        return m_colPath;
    }

    const Gtk::TreeModelColumn<std::string>& DataDownloadsColumns::getColFileType() const
    {
        return m_colFileType;
    }

    const Gtk::TreeModelColumn<std::string>& DataDownloadsColumns::getColUrl() const
    {
        return m_colUrl;
    }
}
