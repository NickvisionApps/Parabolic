#ifndef DATADOWNLOADSCOLUMNS_H
#define DATADOWNLOADSCOLUMNS_H

#include <string>
#include <gtkmm.h>

namespace NickvisionTubeConverter::Models
{
    class DataDownloadsColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        DataDownloadsColumns();
        const Gtk::TreeModelColumn<unsigned int>& getColID() const;
        const Gtk::TreeModelColumn<std::string>& getColPath() const;
        const Gtk::TreeModelColumn<std::string>& getColFileType() const;
        const Gtk::TreeModelColumn<std::string>& getColUrl() const;

    private:
        Gtk::TreeModelColumn<unsigned int> m_colID;
        Gtk::TreeModelColumn<std::string> m_colPath;
        Gtk::TreeModelColumn<std::string> m_colFileType;
        Gtk::TreeModelColumn<std::string> m_colUrl;
    };
}

#endif // DATADOWNLOADSCOLUMNS_H
