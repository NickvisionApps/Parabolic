#include "controls/logpane.h"
#include <QLabel>
#include <QStackedWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <libnick/localization/gettext.h>
#include "controls/statuspage.h"
#include "helpers/qthelpers.h"

using namespace Nickvision::TubeConverter::Qt::Helpers;

enum LogPage
{
    None = 0,
    Has
};

namespace Ui
{
    class LogPane
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Controls::LogPane* parent)
        {
            viewStack = new QStackedWidget(parent);
            //No Log Page
            Nickvision::TubeConverter::Qt::Controls::StatusPage* statusNoLog{ new Nickvision::TubeConverter::Qt::Controls::StatusPage(parent) };
            statusNoLog->setTitle(_("No Download Log"));
            statusNoLog->setIcon(QLEMENTINE_ICON(Misc_EmptySlot));
            viewStack->addWidget(statusNoLog);
            //Log Page
            lblLog = new QLabel(parent);
            lblLog->setMargin(6);
            lblLog->setAlignment(Qt::AlignTop);
            lblLog->setWordWrap(true);
            lblLog->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
            QScrollArea* scrollLog{ new QScrollArea(parent) };
            scrollLog->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            scrollLog->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            scrollLog->setWidgetResizable(true);
            scrollLog->setWidget(lblLog);
            QObject::connect(scrollLog->verticalScrollBar(), &QScrollBar::rangeChanged, [this, scrollLog](int, int max)
            {
                scrollLog->verticalScrollBar()->setValue(max);
            });
            viewStack->addWidget(scrollLog);
            //Main Layout
            viewStack->setCurrentIndex(LogPage::None);
            parent->setWidget(viewStack);
        }

        QStackedWidget* viewStack;
        QLabel* lblLog;
    };
}

namespace Nickvision::TubeConverter::Qt::Controls
{
    LogPane::LogPane(QWidget* parent)
        : QDockWidget{ _("Log"), parent },
        m_ui{ new Ui::LogPane() },
        m_id{ -1 }
    {
        setMinimumHeight(240);
        setMaximumHeight(240);
        setFloating(false);
        setAllowedAreas(::Qt::DockWidgetArea::TopDockWidgetArea | ::Qt::DockWidgetArea::BottomDockWidgetArea);
        //Load Ui
        m_ui->setupUi(this);
        connect(this, &QDockWidget::visibilityChanged, this, &LogPane::onVisibilityChanged);
    }

    LogPane::~LogPane()
    {
        delete m_ui;
    }

    int LogPane::id() const
    {
        return m_id;
    }

    void LogPane::setId(int id)
    {
        m_id = id;
    }

    void LogPane::onVisibilityChanged(bool visible)
    {
        if(!visible)
        {
            m_id = -1;
            update("");
        }
    }

    void LogPane::update(const QString& log)
    {
        m_ui->lblLog->setText(log);
        m_ui->viewStack->setCurrentIndex(log.isEmpty() ? LogPage::None : LogPage::Has);
    }
}
