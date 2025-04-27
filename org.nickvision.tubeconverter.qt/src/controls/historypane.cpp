#include "controls/historypane.h"
#include <QStackedWidget>
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
            //TODO
            //Main Layout
            parent->setWidget(viewStack);
        }

        QStackedWidget* viewStack;
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
        //TODO
    }
}
