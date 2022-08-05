#include "MainWindow.h"
#include <QMessageBox>
#include <QSettings>
#include "SettingsDialog.h"
#include "../Messenger.h"
#include "../Controls/AboutDialog.h"
#include "../Controls/ProgressDialog.h"
#include "../../Helpers/ThemeHelpers.h"
#include "../../Models/AppInfo.h"
#include "../../Models/Configuration.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Controls;

namespace NickvisionTubeConverter::UI::Views
{
	MainWindow::MainWindow(QWidget* parent) : QMainWindow{ parent }, m_opened{ false }, m_updater{ "https://raw.githubusercontent.com/nlogozzo/NickvisionTubeConverter/main/UpdateConfig.json", { AppInfo::getInstance().getVersion() }}
	{
		//==UI==//
		m_ui.setupUi(this);
		//==Window Settings==//
		setWindowTitle(QString::fromStdString(AppInfo::getInstance().getName()));
		//==Pages==//
		m_ui.viewStack->addWidget(&m_homePage);
		m_ui.viewStack->addWidget(new QFrame());
		m_ui.viewStack->addWidget(new QFrame());
		m_ui.viewStack->addWidget(new QFrame());
		changePage(Pages::Home);
		//==Theme==//
		refreshTheme();
		//==Messages==//
		Messenger::getInstance().registerMessage("MainWindow.changePage", [&](void* parameter)
		{
			Pages* page{ static_cast<Pages*>(parameter) };
			if (page)
			{
				changePage(*page);
			}
		});
		Messenger::getInstance().registerMessage("MainWindow.setTitle", [&](void* parameter)
		{
			std::string* title{ static_cast<std::string*>(parameter) };
			if (title)
			{
				setWindowTitle(QString::fromStdString(AppInfo::getInstance().getName() + " - " + *title));
			}
		});
		//==Load Config==//
		if (!Configuration::getInstance().getAlwaysStartOnHomePage())
		{
			changePage(Pages::Browse);
		}
	}

	void MainWindow::on_navHome_clicked()
	{
		changePage(Pages::Home);
	}

	void MainWindow::on_navBrowse_clicked()
	{
		changePage(Pages::Browse);
	}

	void MainWindow::on_navQueue_clicked()
	{
		changePage(Pages::Queue);
	}

	void MainWindow::on_navLogs_clicked()
	{
		changePage(Pages::Logs);
	}

	void MainWindow::on_navCheckForUpdates_clicked()
	{
		ProgressDialog checkingDialog{ this, "Checking for updates...", [&]() {  m_updater.checkForUpdates(); } };
		checkingDialog.exec();
		if (m_updater.getUpdateAvailable())
		{
			QMessageBox msgUpdate{ QMessageBox::Icon::Information, "Update Available", QString::fromStdString("===V" + m_updater.getLatestVersion().toString() + " Changelog===\n" + m_updater.getChangelog() + "\n\n" + AppInfo::getInstance().getName() + " will automatically download and install the update. Please save all work before continuing. Are you ready to update?"), QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, this };
			ThemeHelpers::applyWin32Theme(&msgUpdate);
			int result{ msgUpdate.exec() };
			if (result == QMessageBox::StandardButton::Yes)
			{
				ProgressDialog updateDialog{ this, "Downloading and installing the update...", [&]() { m_updater.windowsUpdate(this); } };
				updateDialog.exec();
				if (!m_updater.getUpdateSuccessful())
				{
					QMessageBox msgError{ QMessageBox::Icon::Critical, "Error", "There was an error downloading and installing the update. Please try again.\nIf the error continues, file a bug report.", QMessageBox::StandardButton::Ok, this };
					ThemeHelpers::applyWin32Theme(&msgError);
					msgError.exec();
				}
			}
		}
		else
		{
			QMessageBox msgNoUpdate{ QMessageBox::Icon::Critical, "Update", "There is no update available at this time. Please try again later.", QMessageBox::StandardButton::Ok, this };
			ThemeHelpers::applyWin32Theme(&msgNoUpdate);
			msgNoUpdate.exec();
		}
	}

	void MainWindow::on_navSettings_clicked()
	{
		SettingsDialog settingsDialog{ this };
		settingsDialog.exec();
		if (Configuration::getInstance().getTheme() != m_currentTheme)
		{
			refreshTheme();
		}
	}

	void MainWindow::on_navAbout_clicked()
	{
		AboutDialog aboutDialog{ this };
		aboutDialog.exec();
	}

	void MainWindow::refreshTheme()
	{
		QApplication::setPalette(ThemeHelpers::getThemedPalette());
		m_ui.separator1->setStyleSheet(ThemeHelpers::getThemedSeparatorStyle());
		setStyleSheet("QCommandLinkButton { font-weight: normal; }");
		m_homePage.refreshTheme();
		ThemeHelpers::applyWin32Theme(this);
		m_currentTheme = Configuration::getInstance().getTheme();
	}

	void MainWindow::changePage(Pages page)
	{
		m_ui.viewStack->setCurrentIndex(static_cast<int>(page));
		if (page == Pages::Home)
		{
			m_ui.navHome->setChecked(true);
			m_ui.navBrowse->setChecked(false);
			m_ui.navQueue->setChecked(false);
			m_ui.navLogs->setChecked(false);
		}
		else if (page == Pages::Browse)
		{
			m_ui.navHome->setChecked(false);
			m_ui.navBrowse->setChecked(true);
			m_ui.navQueue->setChecked(false);
			m_ui.navLogs->setChecked(false);
		}
		else if (page == Pages::Queue)
		{
			m_ui.navHome->setChecked(false);
			m_ui.navBrowse->setChecked(false);
			m_ui.navQueue->setChecked(true);
			m_ui.navLogs->setChecked(false);
		}
		else if (page == Pages::Logs)
		{
			m_ui.navHome->setChecked(false);
			m_ui.navBrowse->setChecked(false);
			m_ui.navQueue->setChecked(false);
			m_ui.navLogs->setChecked(true);
		}
	}
}