#include "controls/downloadrow.h"
#include <cmath>
#include <format>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include "helpers/qthelpers.h"

#define MAXIMUM_HEIGHT 160

using namespace Nickvision::Helpers;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::TubeConverter::Qt::Helpers;
using namespace oclero::qlementine;

enum ButtonStackPage
{
    Running = 0,
    Done,
    Error
};

namespace Ui
{
    class DownloadRow
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Controls::DownloadRow* parent)
        {
            QFont boldFont;
            boldFont.setBold(true);
            buttonStack = new QStackedWidget(parent);
            icon = new IconWidget(parent);
            lblTitle = new QLabel(parent);
            lblTitle->setMinimumWidth(200);
            lblTitle->setMaximumWidth(500);
            lblTitle->setFont(boldFont);
            lblTitle->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
            lblStatus = new QLabel(parent);
            progBar = new QProgressBar(parent);
            btnRunningShowLog = new QPushButton(parent);
            btnRunningShowLog->setAutoDefault(false);
            btnRunningShowLog->setDefault(false);
            btnRunningShowLog->setIcon(QLEMENTINE_ICON(Software_CommandLine));
            btnRunningShowLog->setText(_("Log"));
            btnRunningShowLog->setToolTip(_("Show Log"));
            btnStop = new QPushButton(parent);
            btnStop->setAutoDefault(false);
            btnStop->setDefault(false);
            btnStop->setIcon(QLEMENTINE_ICON(Media_Stop));
            btnStop->setText(_("Stop"));
            btnDoneShowLog = new QPushButton(parent);
            btnDoneShowLog->setAutoDefault(false);
            btnDoneShowLog->setDefault(false);
            btnDoneShowLog->setIcon(QLEMENTINE_ICON(Software_CommandLine));
            btnDoneShowLog->setText(_("Log"));
            btnDoneShowLog->setToolTip(_("Show Log"));
            btnPlay = new QPushButton(parent);
            btnPlay->setAutoDefault(false);
            btnPlay->setDefault(false);
            btnPlay->setIcon(QLEMENTINE_ICON(Media_Play));
            btnPlay->setText(_("Play"));
            btnOpenFolder = new QPushButton(parent);
            btnOpenFolder->setAutoDefault(false);
            btnOpenFolder->setDefault(false);
            btnOpenFolder->setIcon(QLEMENTINE_ICON(File_Folder));
            btnOpenFolder->setText(_("Open"));
            btnOpenFolder->setToolTip(_("Open Containing Folder"));
            btnErrorShowLog = new QPushButton(parent);
            btnErrorShowLog->setAutoDefault(false);
            btnErrorShowLog->setDefault(false);
            btnErrorShowLog->setIcon(QLEMENTINE_ICON(Software_CommandLine));
            btnErrorShowLog->setText(_("Log"));
            btnErrorShowLog->setToolTip(_("Show Log"));
            btnRetry = new QPushButton(parent);
            btnRetry->setAutoDefault(false);
            btnRetry->setDefault(false);
            btnRetry->setIcon(QLEMENTINE_ICON(Action_Refresh));
            btnRetry->setText(_("Retry"));
            QHBoxLayout* layoutButtonsRunning{ new QHBoxLayout() };
            layoutButtonsRunning->addWidget(btnRunningShowLog);
            layoutButtonsRunning->addWidget(btnStop);
            QWidget* wdgButtonsRunning{ new QWidget(parent) };
            wdgButtonsRunning->setLayout(layoutButtonsRunning);
            QHBoxLayout* layoutButtonsDone{ new QHBoxLayout() };
            layoutButtonsDone->addWidget(btnDoneShowLog);
            layoutButtonsDone->addWidget(btnPlay);
            layoutButtonsDone->addWidget(btnOpenFolder);
            QWidget* wdgButtonsDone{ new QWidget(parent) };
            wdgButtonsDone->setLayout(layoutButtonsDone);
            QHBoxLayout* layoutButtonsError{ new QHBoxLayout() };
            layoutButtonsError->addWidget(btnErrorShowLog);
            layoutButtonsError->addWidget(btnRetry);
            QWidget* wdgButtonsError{ new QWidget(parent) };
            wdgButtonsError->setLayout(layoutButtonsError);
            buttonStack->addWidget(wdgButtonsRunning);
            buttonStack->addWidget(wdgButtonsDone);
            buttonStack->addWidget(wdgButtonsError);
            QVBoxLayout* layoutInfo{ new QVBoxLayout() };
            layoutInfo->addStretch();
            layoutInfo->addWidget(lblTitle);
            layoutInfo->addWidget(lblStatus);
            layoutInfo->addWidget(progBar);
            layoutInfo->addStretch();
            QVBoxLayout* layoutButtons{ new QVBoxLayout() };
            layoutButtons->addStretch();
            layoutButtons->addWidget(buttonStack);
            layoutButtons->addStretch();
            QHBoxLayout* layoutMain{ new QHBoxLayout() };
            layoutMain->setSpacing(12);
            layoutMain->addWidget(icon);
            layoutMain->addLayout(layoutInfo);
            layoutMain->addStretch();
            layoutMain->addLayout(layoutButtons);
            parent->setLayout(layoutMain);
        }

        IconWidget* icon;
        QLabel* lblTitle;
        QLabel* lblStatus;
        QProgressBar* progBar;
        QStackedWidget* buttonStack;
        QPushButton* btnRunningShowLog;
        QPushButton* btnStop;
        QPushButton* btnDoneShowLog;
        QPushButton* btnPlay;
        QPushButton* btnOpenFolder;
        QPushButton* btnErrorShowLog;
        QPushButton* btnRetry;
    };
}

namespace Nickvision::TubeConverter::Qt::Controls
{
    DownloadRow::DownloadRow(const DownloadAddedEventArgs& args, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::DownloadRow() },
        m_id{ args.getId() },
        m_path{ args.getPath() }
    {
        setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        setMaximumHeight(MAXIMUM_HEIGHT);
        m_ui->setupUi(this);
        //Load
        m_ui->icon->setIcon(QLEMENTINE_ICON(Action_Download));
        m_ui->lblTitle->setText(QString::fromStdString(m_path.filename().string()));
        if(args.getStatus() == DownloadStatus::Queued)
        {
            m_ui->lblStatus->setText(_("Queued"));
            m_ui->progBar->setRange(0, 1);
        }
        else if(args.getStatus() == DownloadStatus::Running)
        {
            m_ui->lblStatus->setText(_("Running"));
            m_ui->progBar->setRange(0, 0);
        }
        else
        {
            m_ui->lblStatus->setText(_("Unknown"));
            m_ui->progBar->setRange(0, 0);
        }
        m_ui->progBar->setValue(0);
        m_ui->buttonStack->setCurrentIndex(ButtonStackPage::Running);
        //Signals
        connect(m_ui->btnRunningShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnStop, &QPushButton::clicked, [this]() { Q_EMIT stop(m_id); });
        connect(m_ui->btnDoneShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnPlay, &QPushButton::clicked, this, &DownloadRow::play);
        connect(m_ui->btnOpenFolder, &QPushButton::clicked, this, &DownloadRow::openFolder);
        connect(m_ui->btnErrorShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnRetry, &QPushButton::clicked, [this]() { Q_EMIT retry(m_id); });
    }

    DownloadRow::DownloadRow(const DownloadRow& row)
        : QWidget{ row.parentWidget() },
        m_ui{ new Ui::DownloadRow() },
        m_id{ row.m_id },
        m_path{ row.m_path }
    {
        setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        setMaximumHeight(MAXIMUM_HEIGHT);
        m_ui->setupUi(this);
        //Copy UI
        m_ui->icon->setIcon(row.m_ui->icon->icon());
        m_ui->lblTitle->setText(row.m_ui->lblTitle->text());
        m_ui->lblStatus->setText(row.m_ui->lblStatus->text());
        m_ui->progBar->setRange(row.m_ui->progBar->minimum(), row.m_ui->progBar->maximum());
        m_ui->progBar->setValue(row.m_ui->progBar->value());
        m_ui->buttonStack->setCurrentIndex(row.m_ui->buttonStack->currentIndex());
        //Signals
        connect(m_ui->btnRunningShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnStop, &QPushButton::clicked, [this]() { Q_EMIT stop(m_id); });
        connect(m_ui->btnDoneShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnPlay, &QPushButton::clicked, this, &DownloadRow::play);
        connect(m_ui->btnOpenFolder, &QPushButton::clicked, this, &DownloadRow::openFolder);
        connect(m_ui->btnErrorShowLog, &QPushButton::clicked, [this]() { Q_EMIT showLog(m_id); });
        connect(m_ui->btnRetry, &QPushButton::clicked, [this]() { Q_EMIT retry(m_id); });
    }

    DownloadRow::~DownloadRow()
    {
        delete m_ui;
    }

    int DownloadRow::getId() const
    {
        return m_id;
    }

    void DownloadRow::setProgressState(const DownloadProgressChangedEventArgs& args)
    {
        m_ui->icon->setIcon(QLEMENTINE_ICON(Action_Download));
        if(std::isnan(args.getProgress()))
        {
            m_ui->lblStatus->setText(_("Processing"));
            m_ui->progBar->setRange(0, 0);
            m_ui->progBar->setValue(0);
        }
        else
        {
            m_ui->lblStatus->setText(QString::fromStdString(std::vformat("{} | {}", std::make_format_args(CodeHelpers::unmove(_("Running")), args.getSpeedStr()))));
            m_ui->progBar->setRange(0, 100);
            m_ui->progBar->setValue(args.getProgress() * 100);
        }
    }

    void DownloadRow::setCompleteState(const DownloadCompletedEventArgs& args)
    {
        m_path = args.getPath();
        m_ui->lblTitle->setText(QString::fromStdString(m_path.filename().string()));
        m_ui->progBar->setRange(0, 1);
        m_ui->progBar->setValue(1);
        if(args.getStatus() == DownloadStatus::Error)
        {
            m_ui->icon->setIcon(QLEMENTINE_ICON(Action_Clear));
            m_ui->lblStatus->setText(_("Error"));
            m_ui->buttonStack->setCurrentIndex(ButtonStackPage::Error);
        }
        else if(args.getStatus() == DownloadStatus::Success)
        {
            m_ui->icon->setIcon(QLEMENTINE_ICON(Misc_Success));
            m_ui->lblStatus->setText(_("Success"));
            m_ui->buttonStack->setCurrentIndex(ButtonStackPage::Done);
        }
    }

    void DownloadRow::setStopState()
    {
        m_ui->icon->setIcon(QLEMENTINE_ICON(Action_Clear));
        m_ui->lblStatus->setText(_("Stopped"));
        m_ui->progBar->setRange(0, 1);
        m_ui->progBar->setValue(1);
        m_ui->buttonStack->setCurrentIndex(ButtonStackPage::Error);
    }

    void DownloadRow::setStartFromQueueState()
    {
        m_ui->lblStatus->setText(_("Running"));
        m_ui->progBar->setRange(0, 0);
        m_ui->progBar->setValue(0);
    }

    void DownloadRow::play()
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(m_path.string())));
    }

    void DownloadRow::openFolder()
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(m_path.parent_path().string())));
    }
}
