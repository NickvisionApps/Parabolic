#include "controls/historyrow.h"
#include <QDesktopServices>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <libnick/localization/gettext.h>
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::TubeConverter::Qt::Helpers;

namespace Ui
{
    class HistoryRow
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Controls::HistoryRow* parent)
        {
            QFont boldFont;
            boldFont.setBold(true);
            lblTitle = new QLabel(parent);
            lblTitle->setFont(boldFont);
            lblUrl = new QLabel(parent);
            btnPlay = new QPushButton(parent);
            btnPlay->setAutoDefault(false);
            btnPlay->setDefault(false);
            btnPlay->setIcon(QLEMENTINE_ICON(Media_Play));
            btnPlay->setText(_("Play"));
            btnDownload = new QPushButton(parent);
            btnDownload->setAutoDefault(false);
            btnDownload->setDefault(false);
            btnDownload->setIcon(QLEMENTINE_ICON(Action_Download));
            btnDownload->setText(_("Download"));
            btnDelete = new QPushButton(parent);
            btnDelete->setAutoDefault(false);
            btnDelete->setDefault(false);
            btnDelete->setIcon(QLEMENTINE_ICON(Action_Trash));
            btnDelete->setText(_("Delete"));
            QVBoxLayout* layoutInfo{ new QVBoxLayout() };
            layoutInfo->addWidget(lblTitle);
            layoutInfo->addWidget(lblUrl);
            QVBoxLayout* layoutButtons{ new QVBoxLayout() };
            layoutButtons->addWidget(btnPlay);
            layoutButtons->addWidget(btnDownload);
            layoutButtons->addWidget(btnDelete);
            QHBoxLayout* layout{ new QHBoxLayout() };
            layout->addSpacing(12);
            layout->setContentsMargins(6, 6, 6, 6);
            layout->addLayout(layoutInfo);
            layout->addStretch();
            layout->addLayout(layoutButtons);
            parent->setLayout(layout);
        }

        QLabel* lblTitle;
        QLabel* lblUrl;
        QPushButton* btnPlay;
        QPushButton* btnDownload;
        QPushButton* btnDelete;
    };
}

namespace Nickvision::TubeConverter::Qt::Controls
{
    HistoryRow::HistoryRow(const HistoricDownload& download, QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::HistoryRow() }
    {
        setMaximumHeight(260);
        m_ui->setupUi(this);
        //Load
        m_ui->lblTitle->setText(QString::fromStdString(download.getTitle()));
        m_ui->lblUrl->setText(QString::fromStdString(download.getUrl()));
        m_ui->btnPlay->setEnabled(std::filesystem::exists(download.getPath()));
        //Signals
        connect(m_ui->btnPlay, &QPushButton::clicked, [this, download]()
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdString(download.getPath().string())));
        });
        connect(m_ui->btnDownload, &QPushButton::clicked, [this, download](){ Q_EMIT downloadAgain(download.getUrl()); });
        connect(m_ui->btnDelete, &QPushButton::clicked, [this, download](){ Q_EMIT deleteItem(download); });
    }

    HistoryRow::~HistoryRow()
    {
        delete m_ui;
    }
}
