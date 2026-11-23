#include "views/home_page.h"
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <oclero/qlementine/Common.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include "application.h"
#include "translation.h"

namespace ui
{
    class HomePage
    {
    public:
        QLabel* m_icon{ nullptr };
        oclero::qlementine::Label* m_title{ nullptr };
        oclero::qlementine::Label* m_description{ nullptr };
        QVBoxLayout* m_layout{ nullptr };

        HomePage() = default;

        void setupUi(parabolic::qt::views::HomePage* home_page)
        {
            const parabolic::translation::Translator& translator{
                parabolic::application::State::instance().translator(),
            };
            m_icon = new QLabel{ home_page };
            m_icon->setPixmap(QIcon{ ":/org.nickvision.tubeconverter.svg" }.pixmap(64, 64));
            m_icon->setAlignment(Qt::AlignCenter);
            m_title = new oclero::qlementine::Label{ translator._("Download Media"), oclero::qlementine::TextRole::H1,
                                                     home_page };
            m_title->setAlignment(Qt::AlignCenter);
            m_description = new oclero::qlementine::Label{
                translator._("Add a video, audio, or playlist URL to start downloading"),
                oclero::qlementine::TextRole::Default, home_page
            };
            m_description->setAlignment(Qt::AlignCenter);
            m_description->setWordWrap(true);
            m_layout = new QVBoxLayout{ home_page };
            m_layout->setAlignment(Qt::AlignCenter);
            m_layout->setSpacing(12);
            m_layout->addWidget(m_icon);
            m_layout->addWidget(m_title);
            m_layout->addWidget(m_description);
        }
    };
}

namespace parabolic::qt::views
{
    HomePage::HomePage(QWidget* parent)
        : QWidget{ parent },
          m_ui{ new ui::HomePage() }
    {
        m_ui->setupUi(this);
    }

    HomePage::~HomePage()
    {
        delete m_ui;
    }

    void HomePage::paintEvent(QPaintEvent* event)
    {
        QPainter painter{ this };
        painter.fillRect(rect(), oclero::qlementine::appStyle()->theme().backgroundColorMain1);
        QWidget::paintEvent(event);
    }
}
