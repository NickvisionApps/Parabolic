#include "controls/historypane.h"
#include <QStackedWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <libnick/localization/gettext.h>
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Shared::Models;
using namespace Nickvision::TubeConverter::Qt::Helpers;

enum HistoryPage
{
    None = 0,
    Has
};

namespace Ui
{
    class HistoryPane
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Controls::HistoryPane* parent)
        {
            viewStack = new QStackedWidget(parent);
            //No History Page
            Nickvision::TubeConverter::Qt::Controls::StatusPage* statusNoHistory{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            statusNoHistory->setTitle(_("No Download History"));
            statusNoHistory->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            viewStack->addWidget(statusNoHistory);
            //History Page
            listHistory = new QVBoxLayout();
            listHistory->setContentsMargins(0, 0, 0, 0);
            listHistory->addStretch();
            QWidget* widgetHistory{ new QWidget(parent) };
            widgetHistory->setLayout(listHistory);
            QScrollArea* scrollHistory{ new QScrollArea(parent) };
            scrollHistory->setWidgetResizable(true);
            scrollHistory->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
            scrollHistory->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
            scrollHistory->setWidget(widgetHistory);
            viewStack->addWidget(scrollHistory);
            //Main Layout
            parent->setWidget(viewStack);
        }

        QStackedWidget* viewStack;
        QVBoxLayout* listHistory;
    };
}

namespace Nickvision::TubeConverter::Qt::Controls
{
    HistoryPane::HistoryPane(QWidget* parent)
        : QDockWidget{ _("History"), parent },
        m_ui{ new Ui::HistoryPane() }
    {
        setMinimumWidth(320);
        setFloating(false);
        setAllowedAreas(::Qt::DockWidgetArea::LeftDockWidgetArea | ::Qt::DockWidgetArea::RightDockWidgetArea);
        //Load Ui
        m_ui->setupUi(this);
    }

    HistoryPane::~HistoryPane()
    {
        delete m_ui;
    }

    void HistoryPane::update(const std::vector<HistoricDownload>& history)
    {
        clearList();
        for(const HistoricDownload& download : history)
        {
            HistoryRow* row{ new HistoryRow(download, this) };
            QFrame* line{ QtHelpers::createHLine(this) };
            connect(row, &HistoryRow::downloadAgain, [this](const std::string& url){ Q_EMIT downloadAgain(url); });
            connect(row, &HistoryRow::deleteItem, [this](const HistoricDownload& download){ Q_EMIT deleteItem(download); });
            m_ui->viewStack->setCurrentIndex(HistoryPage::Has);
            m_ui->listHistory->insertWidget(0, row);
            m_ui->listHistory->insertWidget(1, line);
            m_rows.push_back(row);
            m_lines.push_back(line);
        }
    }

    void HistoryPane::clearList()
    {
        m_ui->viewStack->setCurrentIndex(HistoryPage::None);
        for(HistoryRow* row : m_rows)
        {
            m_ui->listHistory->removeWidget(row);
            delete row;
        }
        for(QFrame* line : m_lines)
        {
            m_ui->listHistory->removeWidget(line);
            delete line;
        }
        m_rows.clear();
        m_lines.clear();
    }
}
