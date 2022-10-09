#pragma once

#include <memory>
#include <adwaita.h>
#include "../../models/download.hpp"

namespace NickvisionTubeConverter::UI::Controls
{
	class DownloadRow
	{
	public:
		DownloadRow(GtkWindow* parent, const std::shared_ptr<NickvisionTubeConverter::Models::Download>& download);
		GtkWidget* gobj();
		void start();

	private:
		std::shared_ptr<NickvisionTubeConverter::Models::Download> m_download;
		GtkWidget* m_gobj;
		GtkWindow* m_parent;
		GtkWidget* m_boxSuffix;
		GtkWidget* m_viewStackProgress;
		GtkWidget* m_progBar;
		GtkWidget* m_levelBar;
		GtkWidget* m_btnViewLogs;
		void onViewLogs();
	};
}