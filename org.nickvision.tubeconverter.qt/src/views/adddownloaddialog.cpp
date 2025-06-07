#include "views/adddownloaddialog.h"
#include <functional>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <libnick/helpers/stringhelpers.h>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/LoadingSpinner.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"

using namespace Nickvision::Events;
using namespace Nickvision::Helpers;
using namespace Nickvision::Keyring;
using namespace Nickvision::TubeConverter::Qt::Controls;
using namespace Nickvision::TubeConverter::Qt::Helpers;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace oclero::qlementine;

enum AddDownloadDialogPage
{
    Validate = 0,
    Loading,
    Single,
    Playlist
};

enum AddDownloadDialogSubtitlesSinglePage
{
    None = 0,
    Some
};

namespace Ui
{
    class AddDownloadDialog
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::AddDownloadDialog* parent)
        {
            viewStack = new QStackedWidget(parent);
            //Validation Page
            QLabel* lblMediaUrl{ new QLabel(parent) };
            lblMediaUrl->setText(_("Media URL"));
            txtMediaUrl = new LineEdit(parent);
            txtMediaUrl->setPlaceholderText(_("Enter media url here"));
            btnUseBatchFile = new QPushButton(parent);
            btnUseBatchFile->setAutoDefault(false);
            btnUseBatchFile->setDefault(false);
            btnUseBatchFile->setIcon(QLEMENTINE_ICON(Document_Open));
            btnUseBatchFile->setText(_("Use Batch File"));
            QLabel* lblDownloadImmediately{ new QLabel(parent) };
            lblDownloadImmediately->setText(_("Download Immediately"));
            lblDownloadImmediately->setToolTip(_("Use your previous download options for this download"));
            chkDownloadImmediately = new Switch(parent);
            chkDownloadImmediately->setToolTip(_("Use your previous download options for this download"));
            QFormLayout* layoutMedia{ new QFormLayout() };
            layoutMedia->addRow(lblMediaUrl, txtMediaUrl);
            layoutMedia->addRow(nullptr, btnUseBatchFile);
            layoutMedia->addRow(lblDownloadImmediately, chkDownloadImmediately);
            QWidget* mediaPage{ new QWidget(parent) };
            mediaPage->setLayout(layoutMedia);
            QLabel* lblCredential{ new QLabel(parent) };
            lblCredential->setText(_("Credential"));
            cmbCredential = new QComboBox(parent);
            QLabel* lblUsername{ new QLabel(parent) };
            lblUsername->setText(_("Username"));
            txtUsername = new LineEdit(parent);
            txtUsername->setPlaceholderText(_("Enter username here"));
            QLabel* lblPassword{ new QLabel(parent) };
            lblPassword->setText(_("Password"));
            txtPassword = new LineEdit(parent);
            txtPassword->setPlaceholderText(_("Enter password here"));
            QFormLayout* layoutAuthenticate{ new QFormLayout() };
            layoutAuthenticate->addRow(lblCredential, cmbCredential);
            layoutAuthenticate->addRow(lblUsername, txtUsername);
            layoutAuthenticate->addRow(lblPassword, txtPassword);
            QWidget* authenticatePage{ new QWidget(parent) };
            authenticatePage->setLayout(layoutAuthenticate);
            QTabWidget* tabsValidation{ new QTabWidget(parent) };
            tabsValidation->addTab(mediaPage, _("Media"));
            tabsValidation->addTab(authenticatePage, _("Authentication"));
            tabsValidation->setCurrentIndex(0);
            btnValidate = new QPushButton(parent);
            btnValidate->setAutoDefault(true);
            btnValidate->setDefault(true);
            btnValidate->setEnabled(false);
            btnValidate->setIcon(QLEMENTINE_ICON(Navigation_Search));
            btnValidate->setText(_("Validate"));
            QVBoxLayout* layoutValidation{ new QVBoxLayout() };
            layoutValidation->addWidget(tabsValidation);
            layoutValidation->addWidget(btnValidate);
            QWidget* validationPage{ new QWidget(parent) };
            validationPage->setLayout(layoutValidation);
            viewStack->addWidget(validationPage);
            //Loading Page
            LoadingSpinner* spinner{ new LoadingSpinner(parent) };
            spinner->setMinimumSize(32, 32);
            spinner->setMaximumSize(32, 32);
            spinner->setSpinning(true);
            QLabel* lblLoading{ new QLabel(parent) };
            lblLoading->setText(_("This may take some time..."));
            lblLoading->setAlignment(Qt::AlignCenter);
            QHBoxLayout* layoutSpinner{ new QHBoxLayout() };
            layoutSpinner->addStretch();
            layoutSpinner->addWidget(spinner);
            layoutSpinner->addStretch();
            QVBoxLayout* layoutLoading{ new QVBoxLayout() };
            layoutLoading->addStretch();
            layoutLoading->addLayout(layoutSpinner);
            layoutLoading->addWidget(lblLoading);
            layoutLoading->addStretch();
            QWidget* loadingPage{ new QWidget(parent) };
            loadingPage->setLayout(layoutLoading);
            viewStack->addWidget(loadingPage);
            //Single Download Page
            QLabel* lblFileTypeSingle{ new QLabel(parent) };
            lblFileTypeSingle->setText(_("File Type"));
            cmbFileTypeSingle = new QComboBox(parent);
            cmbFileTypeSingle->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
            btnGenericDisclaimerSingle = new QPushButton(parent);
            btnGenericDisclaimerSingle->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            btnGenericDisclaimerSingle->setAutoDefault(false);
            btnGenericDisclaimerSingle->setDefault(false);
            btnGenericDisclaimerSingle->setIcon(QLEMENTINE_ICON(Misc_Warning));
            QLabel* lblVideoFormatSingle{ new QLabel(parent) };
            lblVideoFormatSingle->setText(_("Video Format"));
            cmbVideoFormatSingle = new QComboBox(parent);
            QLabel* lblAudioFormatSingle{ new QLabel(parent) };
            lblAudioFormatSingle->setText(_("Audio Format"));
            cmbAudioFormatSingle = new QComboBox(parent);
            QLabel* lblSaveFolderSingle{ new QLabel(parent) };
            lblSaveFolderSingle->setText(_("Save Folder"));
            txtSaveFolderSingle = new LineEdit(parent);
            txtSaveFolderSingle->setReadOnly(false);
            txtSaveFolderSingle->setPlaceholderText(_("No save folder selected"));
            btnSelectSaveFolderSingle = new QPushButton(parent);
            btnSelectSaveFolderSingle->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            btnSelectSaveFolderSingle->setAutoDefault(false);
            btnSelectSaveFolderSingle->setDefault(false);
            btnSelectSaveFolderSingle->setIcon(QLEMENTINE_ICON(File_FolderOpen));
            btnSelectSaveFolderSingle->setToolTip(_("Select Save Folder"));
            QLabel* lblFilenameSingle{ new QLabel(parent) };
            lblFilenameSingle->setText(_("File Name"));
            txtFilenameSingle = new LineEdit(parent);
            txtFilenameSingle->setPlaceholderText(_("Enter file name here"));
            btnRevertFilenameSingle = new QPushButton(parent);
            btnRevertFilenameSingle->setAutoDefault(false);
            btnRevertFilenameSingle->setDefault(false);
            btnRevertFilenameSingle->setIcon(QLEMENTINE_ICON(Action_Undo));
            btnRevertFilenameSingle->setToolTip(_("Revert to Title"));
            QHBoxLayout* layoutFileTypeSingle{ new QHBoxLayout() };
            layoutFileTypeSingle->addWidget(cmbFileTypeSingle);
            layoutFileTypeSingle->addWidget(btnGenericDisclaimerSingle);
            QHBoxLayout* layoutSaveFolderSingle{ new QHBoxLayout() };
            layoutSaveFolderSingle->addWidget(txtSaveFolderSingle);
            layoutSaveFolderSingle->addWidget(btnSelectSaveFolderSingle);
            QHBoxLayout* layoutFilenameSingle{ new QHBoxLayout() };
            layoutFilenameSingle->addWidget(txtFilenameSingle);
            layoutFilenameSingle->addWidget(btnRevertFilenameSingle);
            QFormLayout* layoutGeneralSingle{ new QFormLayout() };
            layoutGeneralSingle->addRow(lblFileTypeSingle, layoutFileTypeSingle);
            layoutGeneralSingle->addRow(lblVideoFormatSingle, cmbVideoFormatSingle);
            layoutGeneralSingle->addRow(lblAudioFormatSingle, cmbAudioFormatSingle);
            layoutGeneralSingle->addRow(lblSaveFolderSingle, layoutSaveFolderSingle);
            layoutGeneralSingle->addRow(lblFilenameSingle, layoutFilenameSingle);
            QWidget* generalSinglePage{ new QWidget(parent) };
            generalSinglePage->setLayout(layoutGeneralSingle);
            QLabel* lblSplitChaptersSingle{ new QLabel(parent) };
            lblSplitChaptersSingle->setText(_("Split Video by Chapters"));
            chkSplitChaptersSingle = new Switch(parent);
            QLabel* lblExportDescriptionSingle{ new QLabel(parent) };
            lblExportDescriptionSingle->setText(_("Export Description"));
            chkExportDescriptionSingle = new Switch(parent);
            QLabel* lblExcludeHistorySingle{ new QLabel(parent) };
            lblExcludeHistorySingle->setText(_("Exclude from History"));
            chkExcludeHistorySingle = new Switch(parent);
            QLabel* lblTimeFrameStartSingle{ new QLabel(parent) };
            lblTimeFrameStartSingle->setText(_("Start Time"));
            txtTimeFrameStartSingle = new LineEdit(parent);
            txtTimeFrameStartSingle->setPlaceholderText(_("Enter start time here"));
            QLabel* lblTimeFrameEndSingle{ new QLabel(parent) };
            lblTimeFrameEndSingle->setText(_("End Time"));
            txtTimeFrameEndSingle = new LineEdit(parent);
            txtTimeFrameEndSingle->setPlaceholderText(_("Enter end time here"));
            QFormLayout* layoutAdvancedSingle{ new QFormLayout() };
            layoutAdvancedSingle->addRow(lblSplitChaptersSingle, chkSplitChaptersSingle);
            layoutAdvancedSingle->addRow(lblExportDescriptionSingle, chkExportDescriptionSingle);
            layoutAdvancedSingle->addRow(lblExcludeHistorySingle, chkExcludeHistorySingle);
            layoutAdvancedSingle->addRow(lblTimeFrameStartSingle, txtTimeFrameStartSingle);
            layoutAdvancedSingle->addRow(lblTimeFrameEndSingle, txtTimeFrameEndSingle);
            QWidget* advancedSinglePage{ new QWidget(parent) };
            advancedSinglePage->setLayout(layoutAdvancedSingle);
            Nickvision::TubeConverter::Qt::Controls::StatusPage* statusSubtitlesSingle{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            statusSubtitlesSingle->setIcon(QLEMENTINE_ICON(Misc_Unavailable));
            statusSubtitlesSingle->setTitle(_("No Subtitles Available"));
            btnSelectAllSubtitlesSingle = new QPushButton(parent);
            btnSelectAllSubtitlesSingle->setAutoDefault(false);
            btnSelectAllSubtitlesSingle->setDefault(false);
            btnSelectAllSubtitlesSingle->setIcon(QLEMENTINE_ICON(Action_SelectAll));
            btnSelectAllSubtitlesSingle->setText(_("Select All"));
            btnDeselectAllSubtitlesSingle = new QPushButton(parent);
            btnDeselectAllSubtitlesSingle->setAutoDefault(false);
            btnDeselectAllSubtitlesSingle->setDefault(false);
            btnDeselectAllSubtitlesSingle->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            btnDeselectAllSubtitlesSingle->setText(_("Deselect All"));
            QHBoxLayout* layoutButtonsSubtitlesSingle{ new QHBoxLayout() };
            layoutButtonsSubtitlesSingle->addWidget(btnSelectAllSubtitlesSingle);
            layoutButtonsSubtitlesSingle->addWidget(btnDeselectAllSubtitlesSingle);
            listSubtitlesSingle = new QListWidget(parent);
            listSubtitlesSingle->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
            QVBoxLayout* layoutSomeSubtitlesSingle{ new QVBoxLayout() };
            layoutSomeSubtitlesSingle->setContentsMargins(0, 0, 0, 0);
            layoutSomeSubtitlesSingle->addLayout(layoutButtonsSubtitlesSingle);
            layoutSomeSubtitlesSingle->addWidget(listSubtitlesSingle);
            QWidget* someSubtitlesPageSingle{ new QWidget(parent) };
            someSubtitlesPageSingle->setLayout(layoutSomeSubtitlesSingle);
            viewStackSubtitlesSingle = new QStackedWidget(parent);
            viewStackSubtitlesSingle->addWidget(statusSubtitlesSingle);
            viewStackSubtitlesSingle->addWidget(someSubtitlesPageSingle);
            QVBoxLayout* layoutSubtitlesSingle{ new QVBoxLayout() };
            layoutSubtitlesSingle->addWidget(viewStackSubtitlesSingle);
            QWidget* subtitlesSinglePage{ new QWidget(parent) };
            subtitlesSinglePage->setLayout(layoutSubtitlesSingle);
            QTabWidget* tabsSingle{ new QTabWidget(parent) };
            tabsSingle->addTab(generalSinglePage, _("General"));
            tabsSingle->addTab(advancedSinglePage, _("Advanced"));
            tabsSingle->addTab(subtitlesSinglePage, _("Subtitles"));
            tabsSingle->setCurrentIndex(0);
            lblUrlSingle = new QLabel(parent);
            lblUrlSingle->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse);
            btnDownloadSingle = new QPushButton(parent);
            btnDownloadSingle->setAutoDefault(true);
            btnDownloadSingle->setIcon(QLEMENTINE_ICON(Action_Download));
            btnDownloadSingle->setText(_("Download"));
            QVBoxLayout* layoutSingle{ new QVBoxLayout() };
            layoutSingle->addWidget(tabsSingle);
            layoutSingle->addWidget(lblUrlSingle);
            layoutSingle->addWidget(btnDownloadSingle);
            QWidget* singlePage{ new QWidget(parent) };
            singlePage->setLayout(layoutSingle);
            viewStack->addWidget(singlePage);
            //Playlist Download Page
            QLabel* lblFileTypePlaylist{ new QLabel(parent) };
            lblFileTypePlaylist->setText(_("File Type"));
            cmbFileTypePlaylist = new QComboBox(parent);
            btnGenericDisclaimerPlaylist = new QPushButton(parent);
            btnGenericDisclaimerPlaylist->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            btnGenericDisclaimerPlaylist->setAutoDefault(false);
            btnGenericDisclaimerPlaylist->setDefault(false);
            btnGenericDisclaimerPlaylist->setIcon(QLEMENTINE_ICON(Misc_Warning));
            QLabel* lblSplitChaptersPlaylist{ new QLabel(parent) };
            lblSplitChaptersPlaylist->setText(_("Split Video by Chapters"));
            chkSplitChaptersPlaylist = new Switch(parent);
            QLabel* lblExportDescriptionPlaylist{ new QLabel(parent) };
            lblExportDescriptionPlaylist->setText(_("Export Description"));
            chkExportDescriptionPlaylist = new Switch(parent);
            QLabel* lblWriteFilePlaylist{ new QLabel(parent) };
            lblWriteFilePlaylist->setText(_("Write M3U Playlist File"));
            chkWriteFilePlaylist = new Switch(parent);
            QLabel* lblExcludeHistoryPlaylist{ new QLabel(parent) };
            lblExcludeHistoryPlaylist->setText(_("Exclude from History"));
            chkExcludeHistoryPlaylist = new Switch(parent);
            QLabel* lblSaveFolderPlaylist{ new QLabel(parent) };
            lblSaveFolderPlaylist->setText(_("Save Folder"));
            txtSaveFolderPlaylist = new LineEdit(parent);
            txtSaveFolderPlaylist->setReadOnly(false);
            txtSaveFolderPlaylist->setPlaceholderText(_("No save folder selected"));
            btnSelectSaveFolderPlaylist = new QPushButton(parent);
            btnSelectSaveFolderPlaylist->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            btnSelectSaveFolderPlaylist->setAutoDefault(false);
            btnSelectSaveFolderPlaylist->setDefault(false);
            btnSelectSaveFolderPlaylist->setIcon(QLEMENTINE_ICON(File_FolderOpen));
            btnSelectSaveFolderPlaylist->setToolTip(_("Select Save Folder"));
            btnSaveFolderDisclaimerPlaylist = new QPushButton(parent);
            btnSaveFolderDisclaimerPlaylist->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
            btnSaveFolderDisclaimerPlaylist->setAutoDefault(false);
            btnSaveFolderDisclaimerPlaylist->setDefault(false);
            btnSaveFolderDisclaimerPlaylist->setIcon(QLEMENTINE_ICON(Misc_Warning));
            QHBoxLayout* layoutFileTypePlaylist{ new QHBoxLayout() };
            layoutFileTypePlaylist->addWidget(cmbFileTypePlaylist);
            layoutFileTypePlaylist->addWidget(btnGenericDisclaimerPlaylist);
            QHBoxLayout* layoutSaveFolderPlaylist{ new QHBoxLayout() };
            layoutSaveFolderPlaylist->addWidget(txtSaveFolderPlaylist);
            layoutSaveFolderPlaylist->addWidget(btnSelectSaveFolderPlaylist);
            layoutSaveFolderPlaylist->addWidget(btnSaveFolderDisclaimerPlaylist);
            QFormLayout* layoutGeneralPlaylist{ new QFormLayout() };
            layoutGeneralPlaylist->addRow(lblFileTypePlaylist, layoutFileTypePlaylist);
            layoutGeneralPlaylist->addRow(lblSplitChaptersPlaylist, chkSplitChaptersPlaylist);
            layoutGeneralPlaylist->addRow(lblExportDescriptionPlaylist, chkExportDescriptionPlaylist);
            layoutGeneralPlaylist->addRow(lblWriteFilePlaylist, chkWriteFilePlaylist);
            layoutGeneralPlaylist->addRow(lblExcludeHistoryPlaylist, chkExcludeHistoryPlaylist);
            layoutGeneralPlaylist->addRow(lblSaveFolderPlaylist, layoutSaveFolderPlaylist);
            QWidget* generalPagePlaylist{ new QWidget(parent) };
            generalPagePlaylist->setLayout(layoutGeneralPlaylist);
            QLabel* lblNumberTitlesPlaylist{ new QLabel(parent) };
            lblNumberTitlesPlaylist->setText(_("Number Titles"));
            chkNumberTitlesPlaylist = new Switch(parent);
            btnSelectAllPlaylist = new QPushButton(parent);
            btnSelectAllPlaylist->setAutoDefault(false);
            btnSelectAllPlaylist->setDefault(false);
            btnSelectAllPlaylist->setIcon(QLEMENTINE_ICON(Action_SelectAll));
            btnSelectAllPlaylist->setText(_("Select All"));
            btnDeselectAllPlaylist = new QPushButton(parent);
            btnDeselectAllPlaylist->setAutoDefault(false);
            btnDeselectAllPlaylist->setDefault(false);
            btnDeselectAllPlaylist->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            btnDeselectAllPlaylist->setText(_("Deselect All"));
            QHBoxLayout* layoutTitlesPlaylist{ new QHBoxLayout() };
            layoutTitlesPlaylist->addWidget(lblNumberTitlesPlaylist);
            layoutTitlesPlaylist->addWidget(chkNumberTitlesPlaylist);
            QHBoxLayout* layoutButtonsPlaylist{ new QHBoxLayout() };
            layoutButtonsPlaylist->addWidget(btnSelectAllPlaylist);
            layoutButtonsPlaylist->addWidget(btnDeselectAllPlaylist);
            btnRevertToTitlePlaylist = new QPushButton(parent);
            btnRevertToTitlePlaylist->setEnabled(false);
            btnRevertToTitlePlaylist->setIcon(QLEMENTINE_ICON(Action_Undo));
            btnRevertToTitlePlaylist->setText(_("Revert to Title"));
            listItemsPlaylist = new QListWidget(parent);
            listItemsPlaylist->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
            QObject::connect(listItemsPlaylist, &QListWidget::itemSelectionChanged, [this]()
            {
                btnRevertToTitlePlaylist->setEnabled(listItemsPlaylist->selectedItems().size() > 0);
            });
            QVBoxLayout* layoutItemsPlaylist{ new QVBoxLayout() };
            layoutItemsPlaylist->addLayout(layoutTitlesPlaylist);
            layoutItemsPlaylist->addLayout(layoutButtonsPlaylist);
            layoutItemsPlaylist->addWidget(btnRevertToTitlePlaylist);
            layoutItemsPlaylist->addWidget(listItemsPlaylist);
            QWidget* itemsPagePlaylist{ new QWidget(parent) };
            itemsPagePlaylist->setLayout(layoutItemsPlaylist);
            QTabWidget* tabsPlaylist{ new QTabWidget(parent) };
            tabsPlaylist->addTab(generalPagePlaylist, _("General"));
            tabsPlaylist->addTab(itemsPagePlaylist, _("Items"));
            tabsPlaylist->setCurrentIndex(0);
            btnDownloadPlaylist = new QPushButton(parent);
            btnDownloadPlaylist->setAutoDefault(true);
            btnDownloadPlaylist->setIcon(QLEMENTINE_ICON(Action_Download));
            btnDownloadPlaylist->setText(_("Download"));
            QVBoxLayout* layoutPlaylist{ new QVBoxLayout() };
            layoutPlaylist->addWidget(tabsPlaylist);
            layoutPlaylist->addWidget(btnDownloadPlaylist);
            QWidget* playlistPage{ new QWidget(parent) };
            playlistPage->setLayout(layoutPlaylist);
            viewStack->addWidget(playlistPage);
            //Main Layout
            QVBoxLayout* layout{ new QVBoxLayout() };
            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(viewStack);
            parent->setLayout(layout);
        }

        QStackedWidget* viewStack;
        LineEdit* txtMediaUrl;
        QPushButton* btnUseBatchFile;
        Switch* chkDownloadImmediately;
        QComboBox* cmbCredential;
        LineEdit* txtUsername;
        LineEdit* txtPassword;
        QPushButton* btnValidate;
        QComboBox* cmbFileTypeSingle;
        QPushButton* btnGenericDisclaimerSingle;
        QComboBox* cmbVideoFormatSingle;
        QComboBox* cmbAudioFormatSingle;
        LineEdit* txtSaveFolderSingle;
        QPushButton* btnSelectSaveFolderSingle;
        LineEdit* txtFilenameSingle;
        QPushButton* btnRevertFilenameSingle;
        Switch* chkSplitChaptersSingle;
        Switch* chkExportDescriptionSingle;
        Switch* chkExcludeHistorySingle;
        LineEdit* txtTimeFrameStartSingle;
        LineEdit* txtTimeFrameEndSingle;
        QStackedWidget* viewStackSubtitlesSingle;
        QPushButton* btnSelectAllSubtitlesSingle;
        QPushButton* btnDeselectAllSubtitlesSingle;
        QListWidget* listSubtitlesSingle;
        QLabel* lblUrlSingle;
        QPushButton* btnDownloadSingle;
        QComboBox* cmbFileTypePlaylist;
        QPushButton* btnGenericDisclaimerPlaylist;
        Switch* chkSplitChaptersPlaylist;
        Switch* chkExportDescriptionPlaylist;
        Switch* chkWriteFilePlaylist;
        Switch* chkExcludeHistoryPlaylist;
        LineEdit* txtSaveFolderPlaylist;
        QPushButton* btnSelectSaveFolderPlaylist;
        QPushButton* btnSaveFolderDisclaimerPlaylist;
        Switch* chkNumberTitlesPlaylist;
        QPushButton* btnSelectAllPlaylist;
        QPushButton* btnDeselectAllPlaylist;
        QPushButton* btnRevertToTitlePlaylist;
        QListWidget* listItemsPlaylist;
        QPushButton* btnDownloadPlaylist;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    AddDownloadDialog::AddDownloadDialog(const std::shared_ptr<AddDownloadDialogController>& controller, const std::string& url, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::AddDownloadDialog() },
        m_controller{ controller }
    {
        //Dialog Settings
        setWindowTitle(_("Add Download"));
        setMinimumSize(400, 400);
        setModal(true);
        //Load Ui
        m_ui->setupUi(this);
        //Load Validate Page
        m_ui->viewStack->setCurrentIndex(AddDownloadDialogPage::Validate);
        m_ui->txtUsername->setEnabled(false);
        m_ui->txtPassword->setEnabled(false);
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
        QtHelpers::setComboBoxItems(m_ui->cmbCredential, credentialNames);
        //Signals
        connect(m_ui->txtMediaUrl, &QLineEdit::textChanged, this, &AddDownloadDialog::onTxtUrlChanged);
        connect(m_ui->btnUseBatchFile, &QPushButton::clicked, this, &AddDownloadDialog::useBatchFile);
        connect(m_ui->cmbCredential, &QComboBox::currentIndexChanged, this, &AddDownloadDialog::onCmbCredentialChanged);
        connect(m_ui->btnValidate, &QPushButton::clicked, this, &AddDownloadDialog::validateUrl);
        connect(m_ui->cmbFileTypeSingle, &QComboBox::currentIndexChanged, this, &AddDownloadDialog::onCmbFileTypeChanged);
        connect(m_ui->btnGenericDisclaimerSingle, &QPushButton::clicked, this, &AddDownloadDialog::genericFileTypeDisclaimerSingle);
        connect(m_ui->btnSelectSaveFolderSingle, &QPushButton::clicked, this, &AddDownloadDialog::selectSaveFolderSingle);
        connect(m_ui->btnRevertFilenameSingle, &QPushButton::clicked, this, &AddDownloadDialog::revertFilenameSingle);
        connect(m_ui->btnSelectAllSubtitlesSingle, &QPushButton::clicked, this, &AddDownloadDialog::selectAllSubtitlesSingle);
        connect(m_ui->btnDeselectAllSubtitlesSingle, &QPushButton::clicked, this, &AddDownloadDialog::deselectAllSubtitlesSingle);
        connect(m_ui->btnDownloadSingle, &QPushButton::clicked, this, &AddDownloadDialog::downloadSingle);
        connect(m_ui->cmbFileTypePlaylist, &QComboBox::currentIndexChanged, this, &AddDownloadDialog::onCmbFileTypeChanged);
        connect(m_ui->btnGenericDisclaimerPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::genericFileTypeDisclaimerPlaylist);
        connect(m_ui->btnSelectSaveFolderPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::selectSaveFolderPlaylist);
        connect(m_ui->btnSaveFolderDisclaimerPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::saveFolderDisclaimerPlaylist);
        connect(m_ui->chkNumberTitlesPlaylist, &Switch::clicked, this, &AddDownloadDialog::onNumberTitlesPlaylistChanged);
        connect(m_ui->btnSelectAllPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::selectAllPlaylist);
        connect(m_ui->btnDeselectAllPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::deselectAllPlaylist);
        connect(m_ui->btnRevertToTitlePlaylist, &QPushButton::clicked, this, &AddDownloadDialog::revertToTitlePlaylist);
        connect(m_ui->btnDownloadPlaylist, &QPushButton::clicked, this, &AddDownloadDialog::downloadPlaylist);
        m_controller->urlValidated() += [this](const ParamEventArgs<bool>& args){ QtHelpers::dispatchToMainThread([this, args]() { onUrlValidated(args.getParam()); }); };
    }

    AddDownloadDialog::~AddDownloadDialog()
    {
        delete m_ui;
    }

    void AddDownloadDialog::closeEvent(QCloseEvent* event)
    {
        if(m_ui->viewStack->currentIndex() == AddDownloadDialogPage::Loading)
        {
            event->ignore();
            return;
        }
        event->accept();
    }

    void AddDownloadDialog::onTxtUrlChanged(const QString& text)
    {
        m_ui->btnValidate->setEnabled(StringHelpers::isValidUrl(text.toStdString()));
    }

    void AddDownloadDialog::useBatchFile()
    {
        QString file{ QFileDialog::getOpenFileName(this, _("Select Batch File"), {}, _("TXT Files (*.txt)")) };
        if(!file.isEmpty())
        {
            m_ui->viewStack->setCurrentIndex(AddDownloadDialogPage::Loading);
            std::optional<Credential> credential{ std::nullopt };
            if(m_ui->cmbCredential->currentIndex() == 1)
            {
                credential = Credential{ "", "", m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString() };
            }
            if(m_ui->cmbCredential->currentIndex() < 2)
            {
                m_controller->validateBatchFile(file.toStdString(), credential);
            }
            else
            {
                m_controller->validateBatchFile(file.toStdString(), m_ui->cmbCredential->currentIndex() - 2);
            }
        }
    }

    void AddDownloadDialog::onCmbCredentialChanged(int index)
    {
        bool enabled{ index == 1 };
        m_ui->txtUsername->setEnabled(enabled);
        m_ui->txtUsername->clear();
        m_ui->txtPassword->setEnabled(enabled);
        m_ui->txtPassword->clear();
    }

    void AddDownloadDialog::validateUrl()
    {
        m_ui->viewStack->setCurrentIndex(AddDownloadDialogPage::Loading);
        std::optional<Credential> credential{ std::nullopt };
        if(m_ui->cmbCredential->currentIndex() == 1)
        {
            credential = Credential{ "", "", m_ui->txtUsername->text().toStdString(), m_ui->txtPassword->text().toStdString() };
        }
        if(m_ui->cmbCredential->currentIndex() < 2)
        {
            m_controller->validateUrl(m_ui->txtMediaUrl->text().toStdString(), credential);
        }
        else
        {
            m_controller->validateUrl(m_ui->txtMediaUrl->text().toStdString(), m_ui->cmbCredential->currentIndex() - 2);
        }
    }

    void AddDownloadDialog::onUrlValidated(bool valid)
    {
        if(!valid)
        {
            QMessageBox::critical(this, _("Error"), _("The url provided is invalid or unable to be reached. Check the url, the authentication used, the cookies settings, and the preferred codecs selected. Note that the service may have blocked your IP or the video may be geo-restricted."), QMessageBox::StandardButton::Ok);
            m_ui->viewStack->setCurrentIndex(0);
            return;
        }
        m_ui->btnValidate->setDefault(false);
        if(!m_controller->isUrlPlaylist()) //Single Download
        {
            m_ui->viewStack->setCurrentIndex(AddDownloadDialogPage::Single);
            m_ui->lblUrlSingle->setText(QString::fromStdString(m_controller->getMediaUrl(0)));
            m_ui->btnDownloadSingle->setDefault(true);
            //Load Options
            size_t previous{ static_cast<size_t>(m_controller->getPreviousDownloadOptions().getFileType()) };
            QtHelpers::setComboBoxItems(m_ui->cmbFileTypeSingle, m_controller->getFileTypeStrings());
            m_ui->cmbFileTypeSingle->setCurrentIndex(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount() ? previous - MediaFileType::getVideoFileTypeCount() : previous);
            QtHelpers::setComboBoxItems(m_ui->cmbVideoFormatSingle, m_controller->getVideoFormatStrings(&previous));
            m_ui->cmbVideoFormatSingle->setCurrentIndex(previous);
            QtHelpers::setComboBoxItems(m_ui->cmbAudioFormatSingle, m_controller->getAudioFormatStrings(&previous));
            m_ui->cmbAudioFormatSingle->setCurrentIndex(previous);
            m_ui->chkSplitChaptersSingle->setChecked(m_controller->getPreviousDownloadOptions().getSplitChapters());
            m_ui->chkExportDescriptionSingle->setChecked(m_controller->getPreviousDownloadOptions().getExportDescription());
            m_ui->txtSaveFolderSingle->setText(QString::fromStdString(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            m_ui->txtFilenameSingle->setText(QString::fromStdString(m_controller->getMediaTitle(0)));
            m_ui->txtTimeFrameStartSingle->setText(QString::fromStdString(m_controller->getMediaTimeFrame(0).startStr()));
            m_ui->txtTimeFrameStartSingle->setPlaceholderText(QString::fromStdString(m_controller->getMediaTimeFrame(0).startStr()));
            m_ui->txtTimeFrameEndSingle->setText(QString::fromStdString(m_controller->getMediaTimeFrame(0).endStr()));
            m_ui->txtTimeFrameEndSingle->setPlaceholderText(QString::fromStdString(m_controller->getMediaTimeFrame(0).endStr()));
            //Load Subtitles
            std::vector<SubtitleLanguage> previousSubtitles{ m_controller->getPreviousDownloadOptions().getSubtitleLanguages() };
            m_ui->viewStackSubtitlesSingle->setCurrentIndex(AddDownloadDialogSubtitlesSinglePage::None);
            for(const std::string& subtitle : m_controller->getSubtitleLanguageStrings())
            {
                bool wasPreviouslySelected{ false };
                for(const SubtitleLanguage& language : previousSubtitles)
                {
                    if(subtitle == language.str())
                    {
                        wasPreviouslySelected = true;
                        break;
                    }
                }
                QListWidgetItem* item{ new QListWidgetItem(QString::fromStdString(subtitle)) };
                item->setFlags(item->flags() | ::Qt::ItemIsUserCheckable);
                item->setCheckState(wasPreviouslySelected ? ::Qt::CheckState::Checked : ::Qt::CheckState::Unchecked);
                m_ui->listSubtitlesSingle->addItem(item);
                m_ui->viewStackSubtitlesSingle->setCurrentIndex(AddDownloadDialogSubtitlesSinglePage::Some);
            }
        }
        else //Playlist Download
        {
            m_ui->viewStack->setCurrentIndex(AddDownloadDialogPage::Playlist);
            QtHelpers::setComboBoxItems(m_ui->cmbFileTypePlaylist, m_controller->getFileTypeStrings());
            m_ui->cmbFileTypePlaylist->setCurrentIndex(static_cast<int>(m_controller->getPreviousDownloadOptions().getFileType()));
            m_ui->chkSplitChaptersPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getSplitChapters());
            m_ui->chkExportDescriptionPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getExportDescription());
            m_ui->chkWriteFilePlaylist->setChecked(m_controller->getPreviousDownloadOptions().getWritePlaylistFile());
            m_ui->txtSaveFolderPlaylist->setText(QString::fromStdString(m_controller->getPreviousDownloadOptions().getSaveFolder().string()));
            m_ui->chkNumberTitlesPlaylist->setChecked(m_controller->getPreviousDownloadOptions().getNumberTitles());
            for(size_t i = 0; i < m_controller->getMediaCount(); i++)
            {
                QListWidgetItem* item{ new QListWidgetItem(QString::fromStdString(m_controller->getMediaTitle(i, m_controller->getPreviousDownloadOptions().getNumberTitles()))) };
                item->setFlags(item->flags() | ::Qt::ItemIsUserCheckable | ::Qt::ItemIsEditable);
                item->setCheckState(::Qt::CheckState::Checked);
                m_ui->listItemsPlaylist->addItem(item);
            }
        }
        if(m_ui->chkDownloadImmediately->isChecked())
        {
            if(m_controller->isUrlPlaylist())
            {
                downloadPlaylist();
            }
            else
            {
                downloadSingle();
            }
        }
    }

    void AddDownloadDialog::onCmbFileTypeChanged(int index)
    {
        int fileTypeIndex{ index };
        if(m_controller->getFileTypeStrings().size() == MediaFileType::getAudioFileTypeCount())
        {
            fileTypeIndex += MediaFileType::getVideoFileTypeCount();
        }
        MediaFileType type{ static_cast<MediaFileType::MediaFileTypeValue>(fileTypeIndex) };
        m_ui->btnGenericDisclaimerSingle->setVisible(type.isGeneric());
        m_ui->btnGenericDisclaimerPlaylist->setVisible(type.isGeneric());
    }

    void AddDownloadDialog::genericFileTypeDisclaimerSingle()
    {
        QMessageBox::warning(this, _("Warning"), _("Generic file types do not fully support embedding thumbnails and subtitles. Please select a specific file type that is known to support embedding to prevent separate files from being written."));
    }

    void AddDownloadDialog::selectSaveFolderSingle()
    {
        std::filesystem::path path{ QFileDialog::getExistingDirectory(this, _("Select Save Folder")).toStdString() };
        if(std::filesystem::exists(path))
        {
            m_ui->txtSaveFolderSingle->setText(QString::fromStdString(path.make_preferred().string()));
        }
    }

    void AddDownloadDialog::revertFilenameSingle()
    {
        m_ui->txtFilenameSingle->setText(QString::fromStdString(m_controller->getMediaTitle(0)));
    }

    void AddDownloadDialog::selectAllSubtitlesSingle()
    {
        for(int i = 0; i < m_ui->listSubtitlesSingle->count(); i++)
        {
            m_ui->listSubtitlesSingle->item(i)->setCheckState(::Qt::CheckState::Checked);
        }
    }

    void AddDownloadDialog::deselectAllSubtitlesSingle()
    {
        for(int i = 0; i < m_ui->listSubtitlesSingle->count(); i++)
        {
            m_ui->listSubtitlesSingle->item(i)->setCheckState(::Qt::CheckState::Unchecked);
        }
    }
    
    void AddDownloadDialog::downloadSingle()
    {
        std::vector<std::string> subtitles;
        for(int i = 0; i < m_ui->listSubtitlesSingle->count(); i++)
        {
            QListWidgetItem* item{ m_ui->listSubtitlesSingle->item(i) };
            if(item->checkState() == ::Qt::CheckState::Checked)
            {
                subtitles.push_back(item->text().toStdString());
            }
        }
        m_controller->addSingleDownload(m_ui->txtSaveFolderSingle->text().toStdString(), m_ui->txtFilenameSingle->text().toStdString(), m_ui->cmbFileTypeSingle->currentIndex(), m_ui->cmbVideoFormatSingle->currentIndex(), m_ui->cmbAudioFormatSingle->currentIndex(), subtitles, m_ui->chkSplitChaptersSingle->isChecked(), m_ui->chkExportDescriptionSingle->isChecked(), m_ui->chkExcludeHistorySingle->isChecked(), m_ui->txtTimeFrameStartSingle->text().toStdString(), m_ui->txtTimeFrameEndSingle->text().toStdString());
        accept();
    }

    void AddDownloadDialog::genericFileTypeDisclaimerPlaylist()
    {
        QMessageBox::warning(this, _("Warning"), _("Generic file types do not fully support embedding thumbnails and writing playlist files. Please select a specific file type that is known to support embedding to prevent separate files from being written."));
    }

    void AddDownloadDialog::selectSaveFolderPlaylist()
    {
        std::filesystem::path path{ QFileDialog::getExistingDirectory(this, _("Select Save Folder")).toStdString() };
        if(std::filesystem::exists(path))
        {
            m_ui->txtSaveFolderPlaylist->setText(QString::fromStdString(path.make_preferred().string()));
        }
    }

    void AddDownloadDialog::saveFolderDisclaimerPlaylist()
    {
        QMessageBox::warning(this, _("Warning"), _("Will be ignored for media in batch files that provide save folder paths."));
    }

    void AddDownloadDialog::onNumberTitlesPlaylistChanged(bool checked)
    {
        m_controller->setPreviousNumberTitles(checked);
        for(int i = 0; i < m_ui->listItemsPlaylist->count(); i++)
        {
            m_ui->listItemsPlaylist->item(i)->setText(QString::fromStdString(m_controller->getMediaTitle(i, checked)));
        }
    }

    void AddDownloadDialog::selectAllPlaylist()
    {
        for(int i = 0; i < m_ui->listItemsPlaylist->count(); i++)
        {
            m_ui->listItemsPlaylist->item(i)->setCheckState(::Qt::CheckState::Checked);
        }
    }

    void AddDownloadDialog::deselectAllPlaylist()
    {
        for(int i = 0; i < m_ui->listItemsPlaylist->count(); i++)
        {
            m_ui->listItemsPlaylist->item(i)->setCheckState(::Qt::CheckState::Unchecked);
        }
    }

    void AddDownloadDialog::revertToTitlePlaylist()
    {
        int index{ m_ui->listItemsPlaylist->indexFromItem(m_ui->listItemsPlaylist->selectedItems()[0]).row() };
        m_ui->listItemsPlaylist->item(index)->setText(QString::fromStdString(m_controller->getMediaTitle(index, m_ui->chkNumberTitlesPlaylist->isChecked())));
    }

    void AddDownloadDialog::downloadPlaylist()
    {
        std::unordered_map<size_t, std::string> filenames;
        for(int i = 0; i < m_ui->listItemsPlaylist->count(); i++)
        {
            QListWidgetItem* item{ m_ui->listItemsPlaylist->item(i) };
            if(item->checkState() == ::Qt::CheckState::Checked)
            {
                filenames.emplace(static_cast<size_t>(i), item->text().toStdString());
            }
        }
        m_controller->addPlaylistDownload(m_ui->txtSaveFolderPlaylist->text().toStdString(), filenames, m_ui->cmbFileTypePlaylist->currentIndex(), m_ui->chkSplitChaptersPlaylist->isChecked(), m_ui->chkExportDescriptionPlaylist->isChecked(), m_ui->chkWriteFilePlaylist->isChecked(), m_ui->chkExcludeHistoryPlaylist->isChecked());
        accept();
    }
}
