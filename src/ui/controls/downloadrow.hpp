#pragma once

#include <adwaita.h>
#include "../../models/download.hpp"

namespace NickvisionTubeConverter::UI::Controls
{
	class DownloadRow
	{
	public:
		DownloadRow(const NickvisionTubeConverter::Models::Download& download);
		GtkWidget* gobj();
		void start();

	private:
		NickvisionTubeConverter::Models::Download m_download;
		GtkWidget* m_gobj;
	};
}