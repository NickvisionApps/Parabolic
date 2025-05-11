#include "views/settingsdialog.h"
#include <QApplication>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStyleHints>
#include <libnick/localization/gettext.h>
#include <libnick/system/environment.h>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>
#include "helpers/qthelpers.h"

using namespace Nickvision::System;
using namespace Nickvision::TubeConverter::Shared::Controllers;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace oclero::qlementine;

namespace Ui
{
    class SettingsDialog
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Views::SettingsDialog* parent, int maxPostprocessingThreads)
        {
            viewStack = new QStackedWidget(parent);
            //User Interface Page
            QLabel* lblTheme{ new QLabel(parent) };
            lblTheme->setText(_("Theme"));
            cmbTheme = new QComboBox(parent);
            cmbTheme->addItem(_("Light"));
            cmbTheme->addItem(_("Dark"));
            cmbTheme->addItem(_("System"));
            QLabel* lblUpdates{ new QLabel(parent) };
            lblUpdates->setText(_("Automatically Check for Updates"));
            chkUpdates = new Switch(parent);
            QLabel* lblCompletedNotificationTrigger{ new QLabel(parent) };
            lblCompletedNotificationTrigger->setText(_("Completed Notification Trigger"));
            cmbCompletedNotificationTrigger = new QComboBox(parent);
            cmbCompletedNotificationTrigger->addItem(_("For each download"));
            cmbCompletedNotificationTrigger->addItem(_("When all downloads finish"));
            cmbCompletedNotificationTrigger->addItem(_("Never"));
            QLabel* lblPreventSuspend{ new QLabel(parent) };
            lblPreventSuspend->setText(_("Prevent Suspend"));
            chkPreventSuspend = new Switch(parent);
            QLabel* lblRecoverCrashedDownloads{ new QLabel(parent) };
            lblRecoverCrashedDownloads->setText(_("Recover Crashed Downloads"));
            chkRecoverCrashedDownloads = new Switch(parent);
            QLabel* lblDownloadImmediately{ new QLabel(parent) };
            lblDownloadImmediately->setText(_("Download Immediately After Validation"));
            chkDownloadImmediately = new Switch(parent);
            QLabel* lblHistoryLength{ new QLabel(parent) };
            lblHistoryLength->setText(_("Download History Length"));
            cmbHistoryLength = new QComboBox(parent);
            cmbHistoryLength->addItem(_("Never"));
            cmbHistoryLength->addItem(_("One Day"));
            cmbHistoryLength->addItem(_("One Week"));
            cmbHistoryLength->addItem(_("One Month"));
            cmbHistoryLength->addItem(_("Three Months"));
            cmbHistoryLength->addItem(_("Forever"));
            QFormLayout* layoutUserInterface{ new QFormLayout() };
            layoutUserInterface->addRow(lblTheme, cmbTheme);
            layoutUserInterface->addRow(lblUpdates, chkUpdates);
            layoutUserInterface->addRow(lblCompletedNotificationTrigger, cmbCompletedNotificationTrigger);
            layoutUserInterface->addRow(lblPreventSuspend, chkPreventSuspend);
            layoutUserInterface->addRow(lblRecoverCrashedDownloads, chkRecoverCrashedDownloads);
            layoutUserInterface->addRow(lblDownloadImmediately, chkDownloadImmediately);
            layoutUserInterface->addRow(lblHistoryLength, cmbHistoryLength);
            QWidget* userInterfacePage{ new QWidget(parent) };
            userInterfacePage->setLayout(layoutUserInterface);
            viewStack->addWidget(userInterfacePage);
            //Downloads Page
            QLabel* lblMaxNumberOfActiveDownloads{ new QLabel(parent) };
            lblMaxNumberOfActiveDownloads->setText(_("Max Number of Active Downloads"));
            spnMaxNumberOfActiveDownloads = new QSpinBox(parent);
            spnMaxNumberOfActiveDownloads->setMinimum(1);
            spnMaxNumberOfActiveDownloads->setMaximum(10);
            QLabel* lblOverwriteExistingFiles{ new QLabel(parent) };
            lblOverwriteExistingFiles->setText(_("Overwrite Existing Files"));
            chkOverwriteExistingFiles = new Switch(parent);
            lblLimitCharacters = new QLabel(parent);
            lblLimitCharacters->setText(_("Limit Filename Characters"));
            lblLimitCharacters->setToolTip(_("Restricts characters in filenames to only those supported by Windows."));
            chkLimitCharacters = new Switch(parent);
            chkLimitCharacters->setToolTip(_("Restricts characters in filenames to only those supported by Windows."));
            QLabel* lblIncludeMediaId{ new QLabel(parent) };
            lblIncludeMediaId->setText(_("Include Media Id in Title After Validation"));
            chkIncludeMediaId = new Switch(parent);
            QLabel* lblIncludeAutoGeneratedSubtitles{ new QLabel(parent) };
            lblIncludeAutoGeneratedSubtitles->setText(_("Include Auto-Generated Subtitles"));
            chkIncludeAutoGeneratedSubtitles = new Switch(parent);
            QLabel* lblPreferredVideoCodec{ new QLabel(parent) };
            lblPreferredVideoCodec->setText(_("Preferred Video Codec"));
            cmbPreferredVideoCodec = new QComboBox(parent);
            cmbPreferredVideoCodec->addItem(_("Any"));
            cmbPreferredVideoCodec->addItem("VP9");
            cmbPreferredVideoCodec->addItem("AV1");
            cmbPreferredVideoCodec->addItem(_("H.264 (AVC)"));
            cmbPreferredVideoCodec->addItem(_("H.265 (HEVC)"));
            QLabel* lblPreferredAudioCodec{ new QLabel(parent) };
            lblPreferredAudioCodec->setText(_("Preferred Audio Codec"));
            cmbPreferredAudioCodec = new QComboBox(parent);
            cmbPreferredAudioCodec->addItem(_("Any"));
            cmbPreferredAudioCodec->addItem(_("FLAC (ALAC)"));
            cmbPreferredAudioCodec->addItem(_("WAV (AIFF)"));
            cmbPreferredAudioCodec->addItem("OPUS");
            cmbPreferredAudioCodec->addItem("AAC");
            cmbPreferredAudioCodec->addItem("MP4A");
            cmbPreferredAudioCodec->addItem("MP3");
            QLabel* lblPreferredSubtitleFormat{ new QLabel(parent) };
            lblPreferredSubtitleFormat->setText(_("Preferred Subtitle Format"));
            cmbPreferredSubtitleFormat = new QComboBox(parent);
            cmbPreferredSubtitleFormat->addItem(_("Any"));
            cmbPreferredSubtitleFormat->addItem("VTT");
            cmbPreferredSubtitleFormat->addItem("SRT");
            cmbPreferredSubtitleFormat->addItem("ASS");
            cmbPreferredSubtitleFormat->addItem("LRC");
            QFormLayout* layoutDownloads{ new QFormLayout() };
            layoutDownloads->addRow(lblMaxNumberOfActiveDownloads, spnMaxNumberOfActiveDownloads);
            layoutDownloads->addRow(lblOverwriteExistingFiles, chkOverwriteExistingFiles);
            layoutDownloads->addRow(lblLimitCharacters, chkLimitCharacters);
            layoutDownloads->addRow(lblIncludeMediaId, chkIncludeMediaId);
            layoutDownloads->addRow(lblIncludeAutoGeneratedSubtitles, chkIncludeAutoGeneratedSubtitles);
            layoutDownloads->addRow(lblPreferredVideoCodec, cmbPreferredVideoCodec);
            layoutDownloads->addRow(lblPreferredAudioCodec, cmbPreferredAudioCodec);
            layoutDownloads->addRow(lblPreferredSubtitleFormat, cmbPreferredSubtitleFormat);
            QWidget* downloadsPage{ new QWidget(parent) };
            downloadsPage->setLayout(layoutDownloads);
            viewStack->addWidget(downloadsPage);
            //Downloader Page
            QLabel* lblVerboseLogging{ new QLabel(parent) };
            lblVerboseLogging->setText(_("Verbose Logging"));
            chkVerboseLogging = new Switch(parent);
            QLabel* lblSponsorBlock{ new QLabel(parent) };
            lblSponsorBlock->setText(_("Use SponsorBlock for YouTube"));
            chkSponsorBlock = new Switch(parent);
            QLabel* lblSpeedLimit{ new QLabel(parent) };
            lblSpeedLimit->setText(_("Speed Limit"));
            lblSpeedLimit->setToolTip(_("This limit is applied only to downloads that have enabled limiting download speed."));
            spnSpeedLimit = new QSpinBox(parent);
            spnSpeedLimit->setToolTip(_("This limit is applied only to downloads that have enabled limiting download speed."));
            spnSpeedLimit->setMinimum(512);
            spnSpeedLimit->setMaximum(10240);
            spnSpeedLimit->setSingleStep(512);
            QLabel* lblProxyUrl{ new QLabel(parent) };
            lblProxyUrl->setText(_("Proxy URL"));
            txtProxyUrl = new LineEdit(parent);
            txtProxyUrl->setPlaceholderText(_("Enter proxy url here"));
            lblCookiesBrowser = new QLabel(parent);
            lblCookiesBrowser->setText(_("Cookies from Browser"));
            cmbCookiesBrowser = new QComboBox(parent);
            cmbCookiesBrowser->addItem(_("None"));
            cmbCookiesBrowser->addItem(_("Brave"));
            cmbCookiesBrowser->addItem(_("Chrome"));
            cmbCookiesBrowser->addItem(_("Chromium"));
            cmbCookiesBrowser->addItem(_("Edge"));
            cmbCookiesBrowser->addItem(_("Firefox"));
            cmbCookiesBrowser->addItem(_("Opera"));
            cmbCookiesBrowser->addItem(_("Safari"));
            cmbCookiesBrowser->addItem(_("Vivaldi"));
            cmbCookiesBrowser->addItem(_("Whale"));
            QLabel* lblCookiesFile{ new QLabel(parent) };
            lblCookiesFile->setText(_("Cookies File"));
            txtCookiesFile = new LineEdit(parent);
            txtCookiesFile->setReadOnly(true);
            txtCookiesFile->setPlaceholderText(_("No file selected"));
            btnSelectCookiesFile = new QPushButton(parent);
            btnSelectCookiesFile->setAutoDefault(false);
            btnSelectCookiesFile->setDefault(false);
            btnSelectCookiesFile->setIcon(QLEMENTINE_ICON(Document_Open));
            btnSelectCookiesFile->setText(_("Select Cookies File"));
            btnClearCookiesFile = new QPushButton(parent);
            btnClearCookiesFile->setAutoDefault(false);
            btnClearCookiesFile->setDefault(false);
            btnClearCookiesFile->setIcon(QLEMENTINE_ICON(Action_Close));
            btnClearCookiesFile->setText(_("Clear Cookies File"));
            QFormLayout* layoutDownloader{ new QFormLayout() };
            layoutDownloader->addRow(lblVerboseLogging, chkVerboseLogging);
            layoutDownloader->addRow(lblSponsorBlock, chkSponsorBlock);
            layoutDownloader->addRow(lblSpeedLimit, spnSpeedLimit);
            layoutDownloader->addRow(lblProxyUrl, txtProxyUrl);
            layoutDownloader->addRow(lblCookiesBrowser, cmbCookiesBrowser);
            layoutDownloader->addRow(lblCookiesFile, txtCookiesFile);
            layoutDownloader->addRow(nullptr, btnSelectCookiesFile);
            layoutDownloader->addRow(nullptr, btnClearCookiesFile);
            QWidget* downloaderPage{ new QWidget(parent) };
            downloaderPage->setLayout(layoutDownloader);
            viewStack->addWidget(downloaderPage);
            //Converter Page
            QLabel* lblEmbedMetadata{ new QLabel(parent) };
            lblEmbedMetadata->setText(_("Embed Metadata"));
            chkEmbedMetadata = new Switch(parent);
            QLabel* lblRemoveSourceData{ new QLabel(parent) };
            lblRemoveSourceData->setText(_("Remove Source Data"));
            lblRemoveSourceData->setToolTip(_("If enabled, Parabolic will clear metadata fields containing identifying download information."));
            chkRemoveSourceData = new Switch(parent);
            chkRemoveSourceData->setToolTip(_("If enabled, Parabolic will clear metadata fields containing identifying download information."));
            QLabel* lblEmbedThumbnails{ new QLabel(parent) };
            lblEmbedThumbnails->setText(_("Embed Thumbnails"));
            lblEmbedThumbnails->setToolTip(_("If the file type does not support embedding, the thumbnail will be written to a separate image file."));
            chkEmbedThumbnails = new Switch(parent);
            chkEmbedThumbnails->setToolTip(_("If the file type does not support embedding, the thumbnail will be written to a separate image file."));
            QLabel* lblCropAudioThumbnails{ new QLabel(parent) };
            lblCropAudioThumbnails->setText(_("Crop Audio Thumbnails"));
            lblCropAudioThumbnails->setToolTip(_("If enabled, Parabolic will crop the thumbnails of audio files to squares."));
            chkCropAudioThumbnails = new Switch(parent);
            chkCropAudioThumbnails->setToolTip(_("If enabled, Parabolic will crop the thumbnails of audio files to squares."));
            QLabel* lblEmbedChapters{ new QLabel(parent) };
            lblEmbedChapters->setText(_("Embed Chapters"));
            chkEmbedChapters = new Switch(parent);
            QLabel* lblEmbedSubtitles{ new QLabel(parent) };
            lblEmbedSubtitles->setText(_("Embed Subtitles"));
            lblEmbedSubtitles->setToolTip(_("If disabled or if embedding is not supported, downloaded subtitles will be saved to a separate file."));
            chkEmbedSubtitles = new Switch(parent);
            chkEmbedSubtitles->setToolTip(_("If disabled or if embedding is not supported, downloaded subtitles will be saved to a separate file."));
            QLabel* lblPostprocessingThreads{ new QLabel(parent) };
            lblPostprocessingThreads->setText(_("Postprocessing Threads"));
            spnPostprocessingThreads = new QSpinBox(parent);
            spnPostprocessingThreads->setMinimum(1);
            spnPostprocessingThreads->setMaximum(maxPostprocessingThreads);
            QFormLayout* layoutConverter{ new QFormLayout() };
            layoutConverter->addRow(lblEmbedMetadata, chkEmbedMetadata);
            layoutConverter->addRow(lblRemoveSourceData, chkRemoveSourceData);
            layoutConverter->addRow(lblEmbedThumbnails, chkEmbedThumbnails);
            layoutConverter->addRow(lblCropAudioThumbnails, chkCropAudioThumbnails);
            layoutConverter->addRow(lblEmbedChapters, chkEmbedChapters);
            layoutConverter->addRow(lblEmbedSubtitles, chkEmbedSubtitles);
            layoutConverter->addRow(lblPostprocessingThreads, spnPostprocessingThreads);
            QWidget* converterPage{ new QWidget(parent) };
            converterPage->setLayout(layoutConverter);
            viewStack->addWidget(converterPage);
            //aria2 Page
            QLabel* lblUseAria{ new QLabel(parent) };
            lblUseAria->setText(_("Use aria2"));
            chkUseAria = new Switch(parent);
            QLabel* lblAriaMaxConnectionsPerServer{ new QLabel(parent) };
            lblAriaMaxConnectionsPerServer->setText(_("Max Connections Per Server (-x)"));
            spnAriaMaxConnectionsPerServer = new QSpinBox(parent);
            spnAriaMaxConnectionsPerServer->setMinimum(1);
            spnAriaMaxConnectionsPerServer->setMaximum(16);
            QLabel* lblAriaMinSplitSize{ new QLabel(parent) };
            lblAriaMinSplitSize->setText(_("Minimum Split Size (-k)"));
            lblAriaMinSplitSize->setToolTip(_("The minimum size of which to split a file (in MiB)."));
            spnAriaMinSplitSize = new QSpinBox(parent);
            spnAriaMinSplitSize->setToolTip(_("The minimum size of which to split a file (in MiB)."));
            spnAriaMinSplitSize->setMinimum(1);
            spnAriaMinSplitSize->setMaximum(1024);
            QFormLayout* layoutAria{ new QFormLayout() };
            layoutAria->addRow(lblUseAria, chkUseAria);
            layoutAria->addRow(lblAriaMaxConnectionsPerServer, spnAriaMaxConnectionsPerServer);
            layoutAria->addRow(lblAriaMinSplitSize, spnAriaMinSplitSize);
            QWidget* ariaPage{ new QWidget(parent) };
            ariaPage->setLayout(layoutAria);
            viewStack->addWidget(ariaPage);
            //Navigation List
            listNavigation = new QListWidget(parent);
            listNavigation->setMaximumWidth(160);
            listNavigation->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
            listNavigation->setDropIndicatorShown(false);
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Navigation_UiPanelLeft), _("User Interface"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Misc_ItemsList), _("Downloads"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Action_Download), _("Downloader"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Misc_Tool), _("Converter"), listNavigation));
            listNavigation->addItem(new QListWidgetItem(QLEMENTINE_ICON(Software_CommandLine), _("aria2"), listNavigation));
            QObject::connect(listNavigation, &QListWidget::currentRowChanged, [this]()
            {
                viewStack->setCurrentIndex(listNavigation->currentRow());
            });
            //Main Layout
            QHBoxLayout* layout{ new QHBoxLayout() };
            layout->addWidget(listNavigation);
            layout->addWidget(viewStack);
            parent->setLayout(layout);
        }

        QListWidget* listNavigation;
        QStackedWidget* viewStack;
        QComboBox* cmbTheme;
        Switch* chkUpdates;
        QComboBox* cmbCompletedNotificationTrigger;
        Switch* chkPreventSuspend;
        Switch* chkRecoverCrashedDownloads;
        Switch* chkDownloadImmediately;
        QComboBox* cmbHistoryLength;
        QSpinBox* spnMaxNumberOfActiveDownloads;
        Switch* chkOverwriteExistingFiles;
        QLabel* lblLimitCharacters;
        Switch* chkLimitCharacters;
        Switch* chkIncludeMediaId;
        Switch* chkIncludeAutoGeneratedSubtitles;
        QComboBox* cmbPreferredVideoCodec;
        QComboBox* cmbPreferredAudioCodec;
        QComboBox* cmbPreferredSubtitleFormat;
        Switch* chkVerboseLogging;
        Switch* chkSponsorBlock;
        QSpinBox* spnSpeedLimit;
        LineEdit* txtProxyUrl;
        QLabel* lblCookiesBrowser;
        QComboBox* cmbCookiesBrowser;
        LineEdit* txtCookiesFile;
        QPushButton* btnSelectCookiesFile;
        QPushButton* btnClearCookiesFile;
        Switch* chkEmbedMetadata;
        Switch* chkRemoveSourceData;
        Switch* chkEmbedThumbnails;
        Switch* chkCropAudioThumbnails;
        Switch* chkEmbedChapters;
        Switch* chkEmbedSubtitles;
        QSpinBox* spnPostprocessingThreads;
        Switch* chkUseAria;
        QSpinBox* spnAriaMaxConnectionsPerServer;
        QSpinBox* spnAriaMinSplitSize;
    };
}

namespace Nickvision::TubeConverter::Qt::Views
{
    SettingsDialog::SettingsDialog(const std::shared_ptr<PreferencesViewController>& controller, oclero::qlementine::ThemeManager* themeManager, QWidget* parent)
        : QDialog{ parent },
        m_ui{ new Ui::SettingsDialog() },
        m_controller{ controller },
        m_themeManager{ themeManager }
    {
        //Dialog Settings
        setWindowTitle(_("Settings"));
        setMinimumSize(600, 400);
        setModal(true);
        //Load Ui
        m_ui->setupUi(this, m_controller->getMaxPostprocessingThreads());
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_ui->cmbTheme->setCurrentIndex(static_cast<int>(m_controller->getTheme()));
        m_ui->chkUpdates->setChecked(m_controller->getAutomaticallyCheckForUpdates());
        m_ui->cmbCompletedNotificationTrigger->setCurrentIndex(static_cast<int>(m_controller->getCompletedNotificationPreference()));
        m_ui->chkPreventSuspend->setChecked(m_controller->getPreventSuspend());
        m_ui->chkRecoverCrashedDownloads->setChecked(m_controller->getRecoverCrashedDownloads());
        m_ui->chkDownloadImmediately->setChecked(m_controller->getDownloadImmediatelyAfterValidation());
        m_ui->cmbHistoryLength->setCurrentIndex(static_cast<int>(m_controller->getHistoryLengthIndex()));
        m_ui->spnMaxNumberOfActiveDownloads->setValue(options.getMaxNumberOfActiveDownloads());
        m_ui->chkOverwriteExistingFiles->setChecked(options.getOverwriteExistingFiles());
        m_ui->chkLimitCharacters->setChecked(options.getLimitCharacters());
        m_ui->chkIncludeMediaId->setChecked(options.getIncludeMediaIdInTitle());
        m_ui->chkIncludeAutoGeneratedSubtitles->setChecked(options.getIncludeAutoGeneratedSubtitles());
        m_ui->cmbPreferredVideoCodec->setCurrentIndex(static_cast<int>(options.getPreferredVideoCodec()));
        m_ui->cmbPreferredAudioCodec->setCurrentIndex(static_cast<int>(options.getPreferredAudioCodec()));
        m_ui->cmbPreferredSubtitleFormat->setCurrentIndex(static_cast<int>(options.getPreferredSubtitleFormat()));
        m_ui->chkVerboseLogging->setChecked(options.getVerboseLogging());
        m_ui->chkSponsorBlock->setChecked(options.getYouTubeSponsorBlock());
        m_ui->spnSpeedLimit->setValue(options.getSpeedLimit());
        m_ui->txtProxyUrl->setText(QString::fromStdString(options.getProxyUrl()));
        m_ui->cmbCookiesBrowser->setCurrentIndex(static_cast<int>(options.getCookiesBrowser()));
        m_ui->txtCookiesFile->setText(QString::fromStdString(options.getCookiesPath().filename().string()));
        m_ui->txtCookiesFile->setToolTip(QString::fromStdString(options.getCookiesPath().string()));
        m_ui->chkEmbedMetadata->setChecked(options.getEmbedMetadata());
        m_ui->chkRemoveSourceData->setChecked(options.getRemoveSourceData());
        m_ui->chkRemoveSourceData->setEnabled(options.getEmbedMetadata());
        m_ui->chkEmbedThumbnails->setChecked(options.getEmbedThumbnails());
        m_ui->chkCropAudioThumbnails->setChecked(options.getCropAudioThumbnails());
        m_ui->chkCropAudioThumbnails->setEnabled(options.getEmbedThumbnails());
        m_ui->chkEmbedChapters->setChecked(options.getEmbedChapters());
        m_ui->chkEmbedSubtitles->setChecked(options.getEmbedSubtitles());
        m_ui->spnPostprocessingThreads->setValue(options.getPostprocessingThreads());
        m_ui->chkUseAria->setChecked(options.getUseAria());
        m_ui->spnAriaMaxConnectionsPerServer->setValue(options.getAriaMaxConnectionsPerServer());
        m_ui->spnAriaMinSplitSize->setValue(options.getAriaMinSplitSize());
        if(Environment::getDeploymentMode() != DeploymentMode::Local)
        {
            m_ui->lblCookiesBrowser->setVisible(false);
            m_ui->cmbCookiesBrowser->setVisible(false);
        }
        if(Environment::getOperatingSystem() == OperatingSystem::Windows)
        {
            m_ui->lblLimitCharacters->setVisible(false);
            m_ui->chkLimitCharacters->setVisible(false);
        }
        m_ui->listNavigation->setCurrentRow(0);
        //Signals
        connect(m_ui->cmbTheme, &QComboBox::currentIndexChanged, this, &SettingsDialog::onThemeChanged);
        connect(m_ui->btnSelectCookiesFile, &QPushButton::clicked, this, &SettingsDialog::selectCookiesFile);
        connect(m_ui->btnClearCookiesFile, &QPushButton::clicked, this, &SettingsDialog::clearCookiesFile);
        connect(m_ui->chkEmbedMetadata, &Switch::toggled, this, &SettingsDialog::onEmbedMetadataChanged);
        connect(m_ui->chkEmbedThumbnails, &Switch::toggled, this, &SettingsDialog::onEmbedThumbnailsChanged);
    }

    SettingsDialog::~SettingsDialog()
    {
        delete m_ui;
    }

    void SettingsDialog::closeEvent(QCloseEvent* event)
    {
        DownloaderOptions options{ m_controller->getDownloaderOptions() };
        m_controller->setTheme(static_cast<Shared::Models::Theme>(m_ui->cmbTheme->currentIndex()));
        m_controller->setAutomaticallyCheckForUpdates(m_ui->chkUpdates->isChecked());
        m_controller->setCompletedNotificationPreference(static_cast<CompletedNotificationPreference>(m_ui->cmbCompletedNotificationTrigger->currentIndex()));
        m_controller->setPreventSuspend(m_ui->chkPreventSuspend->isChecked());
        m_controller->setRecoverCrashedDownloads(m_ui->chkRecoverCrashedDownloads->isChecked());
        m_controller->setDownloadImmediatelyAfterValidation(m_ui->chkDownloadImmediately->isChecked());
        m_controller->setHistoryLengthIndex(m_ui->cmbHistoryLength->currentIndex());
        m_controller->setDownloaderOptions(options);
        options.setMaxNumberOfActiveDownloads(m_ui->spnMaxNumberOfActiveDownloads->value());
        options.setOverwriteExistingFiles(m_ui->chkOverwriteExistingFiles->isChecked());
        options.setLimitCharacters(m_ui->chkLimitCharacters->isChecked());
        options.setIncludeMediaIdInTitle(m_ui->chkIncludeMediaId->isChecked());
        options.setIncludeAutoGeneratedSubtitles(m_ui->chkIncludeAutoGeneratedSubtitles->isChecked());
        options.setPreferredVideoCodec(static_cast<VideoCodec>(m_ui->cmbPreferredVideoCodec->currentIndex()));
        options.setPreferredAudioCodec(static_cast<AudioCodec>(m_ui->cmbPreferredAudioCodec->currentIndex()));
        options.setPreferredSubtitleFormat(static_cast<SubtitleFormat>(m_ui->cmbPreferredSubtitleFormat->currentIndex()));
        options.setVerboseLogging(m_ui->chkVerboseLogging->isChecked());
        options.setYouTubeSponsorBlock(m_ui->chkSponsorBlock->isChecked());
        options.setSpeedLimit(m_ui->spnSpeedLimit->value());
        options.setProxyUrl(m_ui->txtProxyUrl->text().toStdString());
        options.setCookiesBrowser(static_cast<Browser>(m_ui->cmbCookiesBrowser->currentIndex()));
        options.setCookiesPath(m_ui->txtCookiesFile->toolTip().toStdString());
        options.setEmbedMetadata(m_ui->chkEmbedMetadata->isChecked());
        options.setRemoveSourceData(m_ui->chkRemoveSourceData->isChecked());
        options.setEmbedThumbnails(m_ui->chkEmbedThumbnails->isChecked());
        options.setCropAudioThumbnails(m_ui->chkCropAudioThumbnails->isChecked());
        options.setEmbedChapters(m_ui->chkEmbedChapters->isChecked());
        options.setEmbedSubtitles(m_ui->chkEmbedSubtitles->isChecked());
        options.setPostprocessingThreads(m_ui->spnPostprocessingThreads->value());
        options.setUseAria(m_ui->chkUseAria->isChecked());
        options.setAriaMaxConnectionsPerServer(m_ui->spnAriaMaxConnectionsPerServer->value());
        options.setAriaMinSplitSize(m_ui->spnAriaMinSplitSize->value());
        m_controller->setDownloaderOptions(options);
        m_controller->saveConfiguration();
        event->accept();
    }

    void SettingsDialog::onThemeChanged()
    {
        switch (static_cast<Shared::Models::Theme>(m_ui->cmbTheme->currentIndex()))
        {
        case Shared::Models::Theme::Light:
            QApplication::styleHints()->setColorScheme(::Qt::ColorScheme::Light);
            m_themeManager->setCurrentTheme("Light");
            break;
        case Shared::Models::Theme::Dark:
            QApplication::styleHints()->setColorScheme(::Qt::ColorScheme::Dark);
            m_themeManager->setCurrentTheme("Dark");
            break;
        default:
            QApplication::styleHints()->unsetColorScheme();
            m_themeManager->setCurrentTheme(QApplication::styleHints()->colorScheme() == ::Qt::ColorScheme::Light ? "Light" : "Dark");
            break;
        }
    }

    void SettingsDialog::selectCookiesFile()
    {
        QString file{ QFileDialog::getOpenFileName(this, _("Select Cookies File"), {}, _("TXT Files (*.txt)")) };
        if(!file.isEmpty())
        {
            std::filesystem::path path{ file.toStdString() };
            m_ui->txtCookiesFile->setText(QString::fromStdString(path.filename().string()));
            m_ui->txtCookiesFile->setToolTip(QString::fromStdString(path.string()));
        }
    }

    void SettingsDialog::clearCookiesFile()
    {
        m_ui->txtCookiesFile->setText("");
        m_ui->txtCookiesFile->setToolTip("");
    }

    void SettingsDialog::onEmbedMetadataChanged(bool checked)
    {
        m_ui->chkRemoveSourceData->setEnabled(checked);
    }

    void SettingsDialog::onEmbedThumbnailsChanged(bool checked)
    {
        m_ui->chkCropAudioThumbnails->setEnabled(checked);
    }
}
