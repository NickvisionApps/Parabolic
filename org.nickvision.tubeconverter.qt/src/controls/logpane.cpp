#include "controls/logpane.h"
#include <QApplication>
#include <QClipboard>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
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
            btnCopy = new QPushButton(parent);
            btnCopy->setAutoDefault(false);
            btnCopy->setDefault(false);
            btnCopy->setIcon(QLEMENTINE_ICON(Action_Copy));
            btnCopy->setText(_("Copy"));
            btnCopy->setToolTip(_("Copy Log to Clipboard"));
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
            QVBoxLayout* layoutLog{ new QVBoxLayout() };
            layoutLog->addWidget(btnCopy);
            layoutLog->addWidget(scrollLog);
            QWidget* pageLog{ new QWidget(parent) };
            pageLog->setLayout(layoutLog);
            viewStack->addWidget(pageLog);
            //Main Layout
            viewStack->setCurrentIndex(LogPage::None);
            parent->setWidget(viewStack);
        }

        QStackedWidget* viewStack;
        QPushButton* btnCopy;
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
        setMinimumHeight(260);
        setMaximumHeight(260);
        setFloating(false);
        setAllowedAreas(::Qt::DockWidgetArea::TopDockWidgetArea | ::Qt::DockWidgetArea::BottomDockWidgetArea);
        //Load Ui
        m_ui->setupUi(this);
        connect(this, &QDockWidget::visibilityChanged, this, &LogPane::onVisibilityChanged);
        connect(m_ui->btnCopy, &QPushButton::clicked, this, &LogPane::copyLog);
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

    void LogPane::copyLog()
    {
        QApplication::clipboard()->setText(m_ui->lblLog->text());
    }

    void LogPane::update(const QString& log)
    {
        m_ui->lblLog->setText(log);
        m_ui->viewStack->setCurrentIndex(log.isEmpty() ? LogPage::None : LogPage::Has);
    }
}
