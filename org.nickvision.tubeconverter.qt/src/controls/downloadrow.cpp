#include "controls/downloadrow.h"
#include "ui_downloadrow.h"
#include <cmath>
#include <format>
#include <QDesktopServices>
#include <libnick/helpers/codehelpers.h>
#include <libnick/localization/gettext.h>

using namespace Nickvision::Helpers;
using namespace Nickvision::TubeConverter::Shared::Events;
using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::QT::Controls
{
    DownloadRow::DownloadRow(const DownloadAddedEventArgs& args, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::DownloadRow() },
        m_id{ args.getId() },
        m_log{ _("Starting download...") },
        m_path{ args.getPath() }
    {
        m_ui->setupUi(this);
        //Localize Strings
        m_ui->btnStop->setText(_("Stop"));
        m_ui->btnPlay->setText(_("Play"));
        m_ui->btnOpenFolder->setText(_("Open"));
        m_ui->btnRetry->setText(_("Retry"));
        //Load
        m_ui->lblTitle->setText(QString::fromStdString(m_path.filename().string()));
        m_ui->lblUrl->setText(QString::fromStdString(args.getUrl()));
        if(args.getStatus() == DownloadStatus::Queued)
        {
            m_ui->progressBar->setRange(0, 1);
            m_ui->lblStatus->setText(_("Queued"));
        }
        else if(args.getStatus() == DownloadStatus::Running)
        {
            m_ui->progressBar->setRange(0, 0);
            m_ui->lblStatus->setText(_("Running"));
        }
        else
        {
            m_ui->progressBar->setRange(0, 0);
            m_ui->lblStatus->setText(_("Unknown"));
        }
        m_ui->progressBar->setValue(0);
        m_ui->buttonStack->setCurrentIndex(0);
        //Signals
        connect(m_ui->btnStop, &QPushButton::clicked, [this]() { Q_EMIT stop(m_id); });
        connect(m_ui->btnPlay, &QPushButton::clicked, this, &DownloadRow::play);
        connect(m_ui->btnOpenFolder, &QPushButton::clicked, this, &DownloadRow::openFolder);
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

    const QString& DownloadRow::getLog() const
    {
        return m_log;
    }

    void DownloadRow::setProgressState(const DownloadProgressChangedEventArgs& args)
    {
        if(std::isnan(args.getProgress()))
        {
            m_ui->progressBar->setRange(0, 0);
            m_ui->progressBar->setValue(0);
            m_ui->lblStatus->setText(_("Processing"));
        }
        else
        {
            m_ui->progressBar->setRange(0, 100);
            m_ui->progressBar->setValue(args.getProgress() * 100);
            m_ui->lblStatus->setText(QString::fromStdString(std::vformat("{} | {}", std::make_format_args(CodeHelpers::unmove(_("Running")), args.getSpeedStr()))));
        }
        m_log = QString::fromStdString(args.getLog());
    }

    void DownloadRow::setCompleteState(const DownloadCompletedEventArgs& args)
    {
        m_ui->progressBar->setRange(0, 1);
        m_ui->progressBar->setValue(1);
        if(args.getStatus() == DownloadStatus::Error)
        {
            m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear));
            m_ui->lblStatus->setText(_("Error"));
            m_ui->buttonStack->setCurrentIndex(2);
        }
        else if(args.getStatus() == DownloadStatus::Success)
        {
            m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
            m_ui->lblStatus->setText(_("Success"));
            m_ui->buttonStack->setCurrentIndex(1);
            m_ui->btnPlay->setVisible(std::filesystem::exists(m_path));
        }
    }

    void DownloadRow::setStopState()
    {
        m_ui->progressBar->setRange(0, 1);
        m_ui->progressBar->setValue(1);
        m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear));
        m_ui->lblStatus->setText(_("Stopped"));
        m_ui->buttonStack->setCurrentIndex(2);
    }

    void DownloadRow::setStartFromQueueState()
    {
        m_ui->progressBar->setRange(0, 0);
        m_ui->progressBar->setValue(0);
        m_ui->lblStatus->setText(_("Running"));
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