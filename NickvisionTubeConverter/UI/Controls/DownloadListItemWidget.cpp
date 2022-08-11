#include "DownloadListItemWidget.h"
#include <QMessageBox>
#include "../../Helpers/ThemeHelpers.h"

using namespace NickvisionTubeConverter::Helpers;

namespace NickvisionTubeConverter::UI::Controls
{
	DownloadListItemWidget::DownloadListItemWidget(const NickvisionTubeConverter::Models::Download& download, QWidget* parent) : QWidget{ parent }, m_download{ download }, m_isFinished{ false }, m_isSuccess{ false }
	{
		//==UI==//
		m_ui.setupUi(this);
		//Path
		m_ui.lblPath->setText(QString::fromStdString(m_download.getSavePath()));
		//Url
		m_ui.lblUrl->setText(QString::fromStdString(m_download.getVideoUrl()));
		//Logs
		m_ui.btnLog->setVisible(false);
		//==Thread==//
		m_thread = std::jthread{ [&]()
		{
			bool isSuccess = m_download.download();
			std::lock_guard<std::mutex> lock{ m_mutex };
			m_isFinished = true;
			m_isSuccess = isSuccess;
		}};
		//==Timer==//
		m_timer = new QTimer(this);
		connect(m_timer, &QTimer::timeout, this, &DownloadListItemWidget::timeout);
		m_timer->start(50);
	}

	bool DownloadListItemWidget::getIsFinished() const
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		return m_isFinished;
	}

	void DownloadListItemWidget::timeout()
	{
		std::lock_guard<std::mutex> lock{ m_mutex };
		if (m_isFinished)
		{
			m_timer->stop();
			m_ui.progressBar->setMaximum(100);
			m_ui.progressBar->setValue(100);
			QPalette palette{ m_ui.progressBar->palette() };
			palette.setColor(QPalette::Highlight, m_isSuccess ? Qt::darkGreen : Qt::red);
			m_ui.progressBar->setPalette(palette);
			m_ui.btnLog->setVisible(true);
		}
	}

	void DownloadListItemWidget::on_btnLog_clicked()
	{
		QMessageBox msgLogs{ QMessageBox::Icon::Information, "Logs", QString::fromStdString(m_download.getLog()), QMessageBox::StandardButton::Ok, this};
		ThemeHelpers::applyWin32Theme(&msgLogs);
		msgLogs.exec();
	}
}
