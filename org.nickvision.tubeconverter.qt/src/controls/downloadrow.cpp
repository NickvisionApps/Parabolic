#include "controls/downloadrow.h"
#include "ui_downloadrow.h"
#include <format>
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
        m_log{ _("Starting download...") }
    {
        m_ui->setupUi(this);
        //Localize Strings
        m_ui->btnStop->setText(_("Stop"));
        m_ui->btnPlay->setText(_("Play"));
        m_ui->btnOpenFolder->setText(_("Open"));
        m_ui->btnRetry->setText(_("Retry"));
        //Load
        m_ui->lblTitle->setText(QString::fromStdString(args.getPath().filename().string()));
        m_ui->lblUrl->setText(QString::fromStdString(args.getUrl()));
        m_ui->progressBar->setRange(0, 0);
        m_ui->progressBar->setValue(0);
        switch(args.getStatus())
        {
        case DownloadStatus::Queued:
            m_ui->lblStatus->setText(_("Queued"));
            break;
        case DownloadStatus::Running:
            m_ui->lblStatus->setText(_("Running"));
            break;
        case DownloadStatus::Stopped:
            m_ui->lblStatus->setText(_("Stopped"));
            break;
        case DownloadStatus::Error:
            m_ui->lblStatus->setText(_("Error"));
            break;
        case DownloadStatus::Success:
            m_ui->lblStatus->setText(_("Success"));
            break;
        }
        m_ui->buttonStack->setCurrentIndex(0);
        //Signals
        connect(m_ui->btnStop, &QPushButton::clicked, [this]() { stop(m_id); });
        connect(m_ui->btnPlay, &QPushButton::clicked, [this]() { play(m_id); });
        connect(m_ui->btnOpenFolder, &QPushButton::clicked, [this]() { openFolder(m_id); });
        connect(m_ui->btnRetry, &QPushButton::clicked, [this]() { retry(m_id); });
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

    void DownloadRow::update(const DownloadProgressChangedEventArgs& args)
    {
        if(std::isnan(args.getProgress()))
        {
            m_ui->progressBar->setRange(0, 0);
            m_ui->progressBar->setValue(0);
            m_ui->lblStatus->setText(_("Processing"));
        }
        else
        {
            m_ui->progressBar->setRange(0, 1);
            m_ui->progressBar->setValue(args.getProgress());
            m_ui->lblStatus->setText(QString::fromStdString(std::vformat("{} | {}", std::make_format_args(CodeHelpers::unmove(_("Running")), args.getSpeedStr()))));
        }
        m_log = QString::fromStdString(args.getLog());
    }

    void DownloadRow::complete(const DownloadCompletedEventArgs& args)
    {
        m_ui->progressBar->setRange(0, 1);
        m_ui->progressBar->setValue(1);
        switch(args.getStatus())
        {
        case DownloadStatus::Stopped:
            m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear));
            m_ui->lblStatus->setText(_("Stopped"));
            m_ui->buttonStack->setCurrentIndex(2);
            break;
        case DownloadStatus::Error:
            m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear));
            m_ui->lblStatus->setText(_("Error"));
            m_ui->buttonStack->setCurrentIndex(2);
            break;
        case DownloadStatus::Success:
            m_ui->btnIcon->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew));
            m_ui->lblStatus->setText(_("Success"));
            m_ui->buttonStack->setCurrentIndex(1);
            break;
        }
    }
}