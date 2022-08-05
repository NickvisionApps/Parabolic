#include "AboutDialog.h"
#include <QDesktopServices>
#include "../../Helpers/ThemeHelpers.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;

namespace NickvisionTubeConverter::UI::Controls
{
	AboutDialog::AboutDialog(QWidget* parent) : QDialog{ parent }, m_appInfo{ AppInfo::getInstance() }
	{
		//==UI==//
		m_ui.setupUi(this);
		//App Info
		m_ui.lblAppName->setText(QString::fromStdString(m_appInfo.getName()));
		m_ui.lblDescription->setText(QString::fromStdString(m_appInfo.getDescription()));
		m_ui.lblVersion->setText(QString::fromStdString("Version " + m_appInfo.getVersion()));
		m_ui.lblChangelog->setText(QString::fromStdString(m_appInfo.getChangelog()));
		//==Theme==//
		m_ui.separator1->setStyleSheet(ThemeHelpers::getThemedSeparatorStyle());
		ThemeHelpers::applyWin32Theme(this);
	}

	void AboutDialog::on_btnGitHubRepo_clicked()
	{
		QDesktopServices::openUrl({ QString::fromStdString(m_appInfo.getGitHubRepo()) });
	}

	void AboutDialog::on_btnReportABug_clicked()
	{
		QDesktopServices::openUrl({ QString::fromStdString(m_appInfo.getIssueTracker()) });
	}

	void AboutDialog::on_btnClose_clicked()
	{
		close();
	}
}