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

namespace Nickvision::TubeConverter::Qt::Controls
{
    DownloadRow::DownloadRow(const DownloadAddedEventArgs& args, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::DownloadRow() },
        m_id{ args.getId() },
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

    DownloadRow::DownloadRow(const DownloadRow& row)
        : QWidget{ row.parentWidget() },
        m_ui{ new Ui::DownloadRow() },
        m_id{ row.m_id },
        m_path{ row.m_path }
    {
        m_ui->setupUi(this);
        //Localize Strings
        m_ui->btnStop->setText(_("Stop"));
        m_ui->btnPlay->setText(_("Play"));
        m_ui->btnOpenFolder->setText(_("Open"));
        m_ui->btnRetry->setText(_("Retry"));
        //Copy UI
        m_ui->btnIcon->setIcon(row.m_ui->btnIcon->icon());
        m_ui->lblTitle->setText(row.m_ui->lblTitle->text());
        m_ui->lblUrl->setText(row.m_ui->lblUrl->text());
        m_ui->progressBar->setRange(row.m_ui->progressBar->minimum(), row.m_ui->progressBar->maximum());
        m_ui->progressBar->setValue(row.m_ui->progressBar->value());
        m_ui->lblStatus->setText(row.m_ui->lblStatus->text());
        m_ui->buttonStack->setCurrentIndex(row.m_ui->buttonStack->currentIndex());
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
    }

    void DownloadRow::setCompleteState(const DownloadCompletedEventArgs& args)
    {
        m_path = args.getPath();
        m_ui->lblTitle->setText(QString::fromStdString(m_path.filename().string()));
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
