#include "controls/historyrow.h"
#include "ui_historyrow.h"
#include <QDesktopServices>
#include <libnick/localization/gettext.h>

using namespace Nickvision::TubeConverter::Shared::Models;

namespace Nickvision::TubeConverter::Qt::Controls
{
    HistoryRow::HistoryRow(const HistoricDownload& download, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::HistoryRow() },
        m_download{ download }
    {
        m_ui->setupUi(this);
        //Localize Strings
        m_ui->btnPlay->setText(_("Play"));
        m_ui->btnDownload->setText(_("Download"));
        m_ui->btnDelete->setText(_("Delete"));
        //Load
        m_ui->lblTitle->setText(QString::fromStdString(m_download.getTitle()));
        m_ui->lblUrl->setText(QString::fromStdString(m_download.getUrl()));
        if(!std::filesystem::exists(m_download.getPath()))
        {
            m_ui->btnPlay->setEnabled(false);
        }
        //Signals
        connect(m_ui->btnPlay, &QPushButton::clicked, this, &HistoryRow::play);
        connect(m_ui->btnDownload, &QPushButton::clicked, [this]() { Q_EMIT downloadAgain(m_download.getUrl()); });
        connect(m_ui->btnDelete, &QPushButton::clicked, [this]() { Q_EMIT deleteItem(m_download); });
    }

    HistoryRow::~HistoryRow()
    {
        delete m_ui;
    }

    void HistoryRow::play()
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(m_download.getPath().string())));
    }
}