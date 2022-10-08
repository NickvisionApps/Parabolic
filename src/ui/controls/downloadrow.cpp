#include "downloadrow.hpp"

using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI::Controls;

DownloadRow::DownloadRow(const Download& download) : m_download{ download }, m_gobj{ adw_action_row_new() }
{
    //Row Settings
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(m_gobj), m_download.getSavePath().c_str());
    adw_action_row_set_subtitle(ADW_ACTION_ROW(m_gobj), m_download.getVideoUrl().c_str());
}

GtkWidget* DownloadRow::gobj()
{
    return m_gobj;
}

void DownloadRow::start()
{

}
