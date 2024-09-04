#include "views/adddownloaddialog.h"
#include "ui_adddownloaddialog.h"
#include <functional>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <libnick/helpers/codehelpers.h>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include "helpers/qthelpers.h"

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::QT::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;

namespace Nickvision::TubeConverter::QT::Views
{
    AddDownloadDialog::AddDownloadDialog(const std::shared_ptr<AddDownloadDialogController>& controller, const std::string& url, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::AddDownloadDialog() },
        m_controller{ controller }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("Add Download"));
        //Localize Strings
        m_ui->lblMediaUrl->setText(_("Media URL"));
        m_ui->txtMediaUrl->setPlaceholderText(_("Enter media url here"));
        m_ui->lblAuthenticate->setText(_("Authenticate"));
        m_ui->lblUsername->setText(_("Username"));
        m_ui->txtUsername->setPlaceholderText(_("Enter username here"));
        m_ui->lblPassword->setText(_("Password"));
        m_ui->txtPassword->setPlaceholderText(_("Enter password here"));
        m_ui->btnValidate->setText(_("Validate"));
        m_ui->lblFileTypeSingle->setText(_("File Type"));
        m_ui->lblQualitySingle->setText(_("Quality"));
        m_ui->lblAudioLanguageSingle->setText(_("Audio Language"));
        m_ui->lblDownloadSubtitlesSingle->setText(_("Download Subtitles"));
        m_ui->lblPreferAV1Single->setText(_("Prefer AV1 Codec"));
        m_ui->lblSplitChaptersSingle->setText(_("Split Video by Chapters"));
        m_ui->lblSaveFolderSingle->setText(_("Save Folder"));
        m_ui->txtSaveFolderSingle->setPlaceholderText(_("Select save folder"));
        m_ui->btnSelectSaveFolderSingle->setText(_("Select"));
        m_ui->btnSelectSaveFolderSingle->setToolTip(_("Select Save Folder"));
        m_ui->lblFilenameSingle->setText(_("File Name"));
        m_ui->txtFilenameSingle->setPlaceholderText(_("Enter file name here"));
        m_ui->btnRevertFilenameSingle->setText(_("Revert"));
        m_ui->btnRevertFilenameSingle->setToolTip(_("Revert to Title"));
        m_ui->lblTimeFrameStartSingle->setText(_("Start Time"));
        m_ui->lblTimeFrameEndSingle->setText(_("End Time"));
        m_ui->lblLimitSpeedSingle->setText(_("Limit Download Speed"));
        m_ui->btnDownloadSingle->setText(_("Download"));
        m_ui->lblFileTypePlaylist->setText(_("File Type"));
        m_ui->lblDownloadSubtitlesPlaylist->setText(_("Download Subtitles"));
        m_ui->lblPreferAV1Playlist->setText(_("Prefer AV1 Codec"));
        m_ui->lblSplitChaptersPlaylist->setText(_("Split Video by Chapters"));
        m_ui->lblLimitSpeedPlaylist->setText(_("Limit Download Speed"));
        m_ui->lblNumberTitlesPlaylist->setText(_("Number Titles"));
        m_ui->lblSaveFolderPlaylist->setText(_("Save Folder"));
        m_ui->txtSaveFolderPlaylist->setPlaceholderText(_("Select save folder"));
        m_ui->btnSelectSaveFolderPlaylist->setText(_("Select"));
        m_ui->btnSelectSaveFolderPlaylist->setToolTip(_("Select Save Folder"));
        m_ui->tblItemsPlaylist->setHorizontalHeaderLabels({ _("Download"), _("File Name"), "" });
        m_ui->btnDownloadPlaylist->setText(_("Download"));
        //Load Validate Page
        m_ui->viewStack->setCurrentIndex(0);
        m_ui->lblUsername->hide();
        m_ui->txtUsername->hide();
        m_ui->lblPassword->hide();
        m_ui->txtPassword->hide();
        if(StringHelpers::isValidUrl(url))
        {
            m_ui->txtMediaUrl->setText(QString::fromStdString(url));
            m_ui->btnValidate->setEnabled(true);
        }
        else if(StringHelpers::isValidUrl(QApplication::clipboard()->text().toStdString()))
        {
            m_ui->txtMediaUrl->setText(QApplication::clipboard()->text());
            m_ui->btnValidate->setEnabled(true);
        }
        std::vector<std::string> credentialNames{ m_controller->getKeyringCredentialNames() };
        credentialNames.insert(credentialNames.begin(), _("Use manual credential"));
        credentialNames.insert(credentialNames.begin(), _("None"));
        QTHelpers::setComboBoxItems(m_ui->cmbAuthenticate, credentialNames);
        //Signals
        connect(m_ui->txtMediaUrl, &QLineEdit::textChanged, this, &AddDownloadDialog::onTxtUrlChanged);
        connect(m_ui->cmbAuthenticate, &QComboBox::currentIndexChanged, this, &AddDownloadDialog::onCmbAuthenticateChanged);
        connect(m_ui->btnValidate, &QPushButton::clicked, this, &AddDownloadDialog::validateUrl);
        connect(m_ui->cmbFileTypeSingle, &QComboBox::currentIndexChanged, this, &AddDownloadDialog::onCmbFileTypeSingleChanged);
        connect(m_ui->btnSelectSaveFolderSingle, &QPushButton::clicked, this, &AddDownloadDialog::selectSaveFolderSingle);
        connect(m_ui->btnRevertFilenameSingle, &QPushButton::clicked, this, &AddDownloadDialog::revertFilenameSingle);
        connect(m_ui->btnDownloadSingle, &QPushButton::clicked, this, &AddDownloadDialog::downloadSingle);
        connect(m_ui->btnSelectSaveFolderPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::selectSaveFolderPlaylist);
        connect(m_ui->chkNumberTitlesPlaylist, &QCheckBox::stateChanged, this, &AddDownloadDialog::onNumberTitlesPlaylistChanged);
        connect(m_ui->btnDownloadPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::downloadPlaylist);
        m_controller->urlValidated() += [this](const ParamEventArgs<bool>& args){ QTHelpers::dispatchToMainThread([this]() { onUrlValidated(); }); };
    }

    AddDownloadDialog::~AddDownloadDialog()
    {
        delete m_ui;
    }

    void AddDownloadDialog::onTxtUrlChanged(const QString& text)
    {
        m_ui->btnValidate->setEnabled(StringHelpers::isValidUrl(text.toStdString()));
    }

    void AddDownloadDialog::onCmbAuthenticateChanged(int index)
    {
        if(index == 1)
        {
            m_ui->lblUsername->show();
            m_ui->txtUsername->show();
            m_ui->lblPassword->show();
            m_ui->txtPassword->show();
        }
        else
        {
            m_ui->lblUsername->hide();
            m_ui->txtUsername->hide();
            m_ui->lblPassword->hide();
            m_ui->txtPassword->hide();
        }
    }

    void AddDownloadDialog::validateUrl()
    {
        m_ui->viewStack->setCurrentIndex(1);
        std::optional<Credential> credential{ std::nullopt };
        if(m_ui->cmbAuthenticate->currentIndex() == 1)
        {
            credential = Credential{ "", "", m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString() };
        }
        if(m_ui->cmbAuthenticate->currentIndex() < 2)
        {
            m_controller->validateUrl(m_ui->txtMediaUrl->text().toStdString(), credential);
        }
        else
        {
            m_controller->validateUrl(m_ui->txtMediaUrl->text().toStdString(), m_ui->cmbAuthenticate->currentIndex() - 2);
        }
    }

    void AddDownloadDialog::onUrlValidated()
    {
        if(!m_controller->isUrlValid())
        {
            QMessageBox::critical(this, _("Error"), _("The url provided is invalid or unable to be reached. Check the url, the authentication used, and the selected browser for cookies in settings."), QMessageBox::StandardButton::Ok);
            m_ui->viewStack->setCurrentIndex(0);
            return;
        }
        if(!m_controller->isUrlPlaylist())
        {
            m_ui->viewStack->setCurrentIndex(2);
            QTHelpers::setComboBoxItems(m_ui->cmbFileTypeSingle, m_controller->getFileTypeStrings());
            m_ui->cmbFileTypeSingle->setCurrentIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            QTHelpers::setComboBoxItems(m_ui->cmbQualitySingle, m_controller->getQualityStrings(m_ui->cmbFileTypeSingle->currentIndex()));
            QTHelpers::setComboBoxItems(m_ui->cmbAudioLanguageSingle, m_controller->getAudioLanguageStrings());
            m_ui->chkDownloadSubtitlesSingle->setChecked(m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            m_ui->chkPreferAV1Single->setChecked(m_controller->getPreviousDownloadOptions().getPreferAV1());
            m_ui->chkSplitChaptersSingle->setChecked(m_controller->getPreviousDownloadOptions().getSplitChapters());
            m_ui->txtSaveFolderSingle->setText(QString::fromStdString(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            m_ui->txtFilenameSingle->setText(QString::fromStdString(m_controller->getMediaTitle(0)));
            m_ui->txtTimeFrameStartSingle->setText(QString::fromStdString(m_controller->getMediaTimeFrame(0).startStr()));
            m_ui->txtTimeFrameStartSingle->setPlaceholderText(QString::fromStdString(m_controller->getMediaTimeFrame(0).startStr()));
            m_ui->txtTimeFrameEndSingle->setText(QString::fromStdString(m_controller->getMediaTimeFrame(0).endStr()));
            m_ui->txtTimeFrameEndSingle->setPlaceholderText(QString::fromStdString(m_controller->getMediaTimeFrame(0).endStr()));
            m_ui->chkLimitSpeedSingle->setChecked(m_controller->getPreviousDownloadOptions().getLimitSpeed());
        }
        else
        {
            m_ui->viewStack->setCurrentIndex(3);
            m_ui->lblItemsPlaylist->setText(QString::fromStdString(std::vformat(_("Playlist Items ({})"), std::make_format_args(CodeHelpers::unmove(m_controller->getMediaCount())))));
            QTHelpers::setComboBoxItems(m_ui->cmbFileTypePlaylist, m_controller->getFileTypeStrings());
            m_ui->cmbFileTypePlaylist->setCurrentIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            m_ui->chkDownloadSubtitlesPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getDownloadSubtitles());
            m_ui->chkPreferAV1Playlist->setChecked(m_controller->getPreviousDownloadOptions().getPreferAV1());
            m_ui->chkSplitChaptersPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getSplitChapters());
            m_ui->chkLimitSpeedPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getLimitSpeed());
            m_ui->chkNumberTitlesPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getNumberTitles());
            m_ui->txtSaveFolderPlaylist->setText(QString::fromStdString(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            for(size_t i = 0; i < m_controller->getMediaCount(); i++)
            {
                QCheckBox* chk{ new QCheckBox(m_ui->tblItemsPlaylist) };
                chk->setChecked(true);
                QPushButton* btn{ new QPushButton(m_ui->tblItemsPlaylist) };
                btn->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo));
                btn->setText(_("Revert"));
                btn->setToolTip(_("Revert to Title"));
                connect(btn, &QPushButton::clicked, [this, i](){ m_ui->tblItemsPlaylist->item(static_cast<int>(i), 1)->setText(QString::fromStdString(m_controller->getMediaTitle(i))); });
                m_ui->tblItemsPlaylist->insertRow(static_cast<int>(i));
                m_ui->tblItemsPlaylist->setCellWidget(static_cast<int>(i), 0, chk);
                m_ui->tblItemsPlaylist->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::fromStdString(m_controller->getMediaTitle(i))));
                m_ui->tblItemsPlaylist->setCellWidget(static_cast<int>(i), 2, btn);
            }
            m_ui->tblItemsPlaylist->resizeColumnToContents(0);
            m_ui->tblItemsPlaylist->resizeColumnToContents(2);
            m_ui->tblItemsPlaylist->setColumnWidth(1, m_ui->tblItemsPlaylist->width() - m_ui->tblItemsPlaylist->columnWidth(0) - m_ui->tblItemsPlaylist->columnWidth(2) - 40);
        }
        adjustSize();
    }

    void AddDownloadDialog::onCmbFileTypeSingleChanged(int index)
    {
        QTHelpers::setComboBoxItems(m_ui->cmbQualitySingle, m_controller->getQualityStrings(index));
    }

    void AddDownloadDialog::selectSaveFolderSingle()
    {
        QString path{ QFileDialog::getExistingDirectory(this, _("Select Save Folder")) };
        if(!path.isEmpty())
        {
            m_ui->txtSaveFolderSingle->setText(path);
        }
    }

    void AddDownloadDialog::revertFilenameSingle()
    {
        m_ui->txtFilenameSingle->setText(QString::fromStdString(m_controller->getMediaTitle(0)));
    }
    
    void AddDownloadDialog::downloadSingle()
    {
        m_controller->addSingleDownload(m_ui->txtSaveFolderSingle->text().toStdString(), m_ui->txtFilenameSingle->text().toStdString(), m_ui->cmbFileTypeSingle->currentIndex(), m_ui->cmbQualitySingle->currentIndex(), m_ui->cmbAudioLanguageSingle->currentIndex(), m_ui->chkDownloadSubtitlesSingle->isChecked(), m_ui->chkPreferAV1Single->isChecked(), m_ui->chkSplitChaptersSingle->isChecked(), m_ui->chkLimitSpeedSingle->isChecked(), m_ui->txtTimeFrameStartSingle->text().toStdString(), m_ui->txtTimeFrameEndSingle->text().toStdString());
        accept();
    }

    void AddDownloadDialog::selectSaveFolderPlaylist()
    {
        QString path{ QFileDialog::getExistingDirectory(this, _("Select Save Folder")) };
        if(!path.isEmpty())
        {
            m_ui->txtSaveFolderPlaylist->setText(path);
        }
    }

    void AddDownloadDialog::onNumberTitlesPlaylistChanged(int state)
    {
        for(int i = 0; i < m_ui->tblItemsPlaylist->rowCount(); i++)
        {
            m_ui->tblItemsPlaylist->item(i, 1)->setText(QString::fromStdString(m_controller->getMediaTitle(i, state == Qt::Checked)));
        }
    }

    void AddDownloadDialog::downloadPlaylist()
    {
        std::unordered_map<size_t, std::string> filenames;
        for(int i = 0; i < m_ui->tblItemsPlaylist->rowCount(); i++)
        {
            QCheckBox* chk{ static_cast<QCheckBox*>(m_ui->tblItemsPlaylist->cellWidget(i, 0)) };
            if(chk->isChecked())
            {
                filenames.emplace(static_cast<size_t>(i), m_ui->tblItemsPlaylist->item(i, 1)->text().toStdString());
            }
        }
        m_controller->addPlaylistDownload(m_ui->txtSaveFolderPlaylist->text().toStdString(), filenames, m_ui->cmbFileTypePlaylist->currentIndex(), m_ui->chkDownloadSubtitlesPlaylist->isChecked(), m_ui->chkPreferAV1Playlist->isChecked(), m_ui->chkSplitChaptersPlaylist->isChecked(), m_ui->chkLimitSpeedPlaylist->isChecked());
        accept();
    }
}