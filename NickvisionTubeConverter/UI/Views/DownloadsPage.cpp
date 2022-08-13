#include "DownloadsPage.h"
#include "../Messenger.h"
#include "../Controls/DownloadDialog.h"
#include "../Controls/DownloadListItemWidget.h"
#include "../../Helpers/ThemeHelpers.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;
using namespace NickvisionTubeConverter::UI;
using namespace NickvisionTubeConverter::UI::Controls;

namespace NickvisionTubeConverter::UI::Views
{
	DownloadsPage::DownloadsPage(QWidget* parent) : QWidget{ parent }
	{
		//==UI==//
		m_ui.setupUi(this);
		//Ribbon
		m_ui.ribbon->setCurrentIndex(0);
		//==Theme==//
		refreshTheme();
		//==Messages==//
		Messenger::getInstance().registerMessage("DownloadsPage.addDownload", [&](void* parameter)
		{
			Download* download{ static_cast<Download*>(parameter) };
			if (download)
			{
				addDownload(*download);
			}
		});
		Messenger::getInstance().registerMessage("DownloadsPage.addDownloadWithDialog", [&](void* parameter) { on_btnAddDownload_clicked(); });
	}

	void DownloadsPage::refreshTheme()
	{
		m_ui.ribbon->setStyleSheet(ThemeHelpers::getThemedRibbonStyle());
	}

	void DownloadsPage::on_btnAddDownload_clicked()
	{
		DownloadDialog downloadDialog{ this };
		int result{ downloadDialog.exec() };
		if (result == QDialog::Accepted)
		{
			Download download{ downloadDialog.getDownload() };
			addDownload(download);
		}
	}

	void DownloadsPage::on_btnClearCompletedDownloads_clicked()
	{
		for (QListWidgetItem* downloadItem : m_ui.listDownloads->findItems("*", Qt::MatchWildcard))
		{
			DownloadListItemWidget* downloadWiget{ static_cast<DownloadListItemWidget*>(m_ui.listDownloads->itemWidget(downloadItem)) };
			if (downloadWiget->getIsFinished())
			{
				m_ui.listDownloads->takeItem(m_ui.listDownloads->row(downloadItem));
			}
		}
	}

	void DownloadsPage::addDownload(const Download& download)
	{
		QListWidgetItem* downloadItem{ new QListWidgetItem() };
		DownloadListItemWidget* downloadWidget{ new DownloadListItemWidget(download, m_ui.listDownloads) };
		downloadItem->setSizeHint(downloadWidget->sizeHint());
		m_ui.listDownloads->addItem(downloadItem);
		m_ui.listDownloads->setItemWidget(downloadItem, downloadWidget);
	}
}