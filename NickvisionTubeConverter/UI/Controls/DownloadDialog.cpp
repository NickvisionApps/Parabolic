#include "DownloadDialog.h"
#include <filesystem>
#include <QFileDialog>
#include "../../Helpers/ThemeHelpers.h"
#include "../../Models/Configuration.h"

using namespace NickvisionTubeConverter::Helpers;
using namespace NickvisionTubeConverter::Models;

namespace NickvisionTubeConverter::UI::Controls
{
	DownloadDialog::DownloadDialog(QWidget* parent, const QString& videoUrl) : QDialog{ parent }
	{
		//==UI==//
		m_ui.setupUi(this);
		//Video Url
		if (!videoUrl.isEmpty())
		{
			m_ui.txtVideoUrl->setText(videoUrl);
			m_ui.txtVideoUrl->setReadOnly(true);
		}
		//==Theme==//
		ThemeHelpers::applyWin32Theme(this);
		//==Load Config==//
		Configuration& configuration{ Configuration::getInstance() };
		if (std::filesystem::exists(configuration.getPreviousSaveFolder()))
		{
			m_ui.txtSaveFolder->setText(QString::fromStdString(configuration.getPreviousSaveFolder()));
		}
		m_ui.cmbFileFormat->setCurrentIndex(static_cast<int>(configuration.getPreviousFileFormat()));
	}

	Download DownloadDialog::getDownload() const
	{
		return { m_ui.txtVideoUrl->text().toStdString(), static_cast<MediaFileType::Value>(m_ui.cmbFileFormat->currentIndex()), m_ui.txtSaveFolder->text().toStdString(), m_ui.txtNewFilename->text().toStdString(), static_cast<Quality>(m_ui.cmbQuality->currentIndex()) };
	}

	void DownloadDialog::on_btnSelectSaveFolder_clicked()
	{
		QString folderPath{ QFileDialog::getExistingDirectory(this, "Select Save Folder") };
		if (!folderPath.isEmpty())
		{
			m_ui.txtSaveFolder->setText(folderPath);
		}
	}

	void DownloadDialog::on_btnDownload_clicked()
	{
		//==Empty Check==//
		if (m_ui.txtVideoUrl->text().isEmpty())
		{
			m_ui.lblError->setText("[Error] Video Url can not be empty.");
		}
		else if (!m_ui.txtVideoUrl->text().startsWith("https://www.youtube.com/watch?v=") && !m_ui.txtVideoUrl->text().startsWith("http://www.youtube.com/watch?v="))
		{
			m_ui.lblError->setText("[Error] Video Url must be a valid YouTube url.");
		}
		else if (m_ui.txtSaveFolder->text().isEmpty())
		{
			m_ui.lblError->setText("[Error] Save Folder can not be empty.");
		}
		else if (m_ui.txtNewFilename->text().isEmpty())
		{
			m_ui.lblError->setText("[Error] New Filename can not be empty.");
		}
		else
		{
			//==Save Config==//
			Configuration& configuration{ Configuration::getInstance() };
			configuration.setPreviousSaveFolder(m_ui.txtSaveFolder->text().toStdString());
			configuration.setPreviousFileForamt(static_cast<MediaFileType::Value>(m_ui.cmbFileFormat->currentIndex()));
			configuration.save();
			//==Close==//
			accept();
		}
	}

	void DownloadDialog::on_btnCancel_clicked()
	{
		reject();
	}
}
