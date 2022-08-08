#include "BrowsePage.h"
#include "../../Helpers/ThemeHelpers.h"
#include "../../Models/Configuration.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;

namespace NickvisionTubeConverter::UI::Views
{
	BrowsePage::BrowsePage(QWidget* parent) : QWidget{ parent }
	{
		//==UI==//
		//WebView Dark Mode
		if (Configuration::getInstance().getTheme() == Theme::Dark)
		{
			qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--force-dark-mode");
		}
		//Setup UI
		m_ui.setupUi(this);
		//==Theme==//
		refreshTheme();
	}

	void BrowsePage::refreshTheme()
	{
		m_ui.separator1->setStyleSheet(ThemeHelpers::getThemedSeparatorStyle());
	}

	void BrowsePage::on_btnBack_clicked()
	{
		m_ui.webView->back();
	}

	void BrowsePage::on_btnForward_clicked()
	{
		m_ui.webView->forward();
	}

	void BrowsePage::on_btnRefresh_clicked()
	{
		m_ui.webView->reload();
	}

	void BrowsePage::on_btnHome_clicked()
	{
		m_ui.webView->setUrl({ "https://www.youtube.com" });
	}

	void BrowsePage::on_btnDownload_clicked()
	{

	}

	void BrowsePage::on_webView_urlChanged(const QUrl& url)
	{
		m_ui.txtUrl->setText(url.toString());
	}
}
