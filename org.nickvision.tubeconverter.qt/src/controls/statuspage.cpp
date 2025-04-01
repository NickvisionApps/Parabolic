#include "controls/statuspage.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/IconWidget.hpp>

using namespace oclero::qlementine;

namespace Ui
{
    class StatusPage
    {
    public:
        void setupUi(Nickvision::TubeConverter::Qt::Controls::StatusPage* parent)
        {
            QFont boldFont;
            boldFont.setBold(true);
            boldFont.setPointSize(boldFont.pointSize() + 4);
            //Stacked Items
            QVBoxLayout* layoutStack{ new QVBoxLayout() };
            layoutStack->setSpacing(12);
            layoutStack->addStretch();
            //Icon
            lblIcon = new IconWidget(parent);
            lblIcon->setIconSize({ 64, 64 });
            layoutStack->addWidget(lblIcon, 0, ::Qt::AlignCenter);
            //Title
            lblTitle = new QLabel(parent);
            lblTitle->setFont(boldFont);
            lblTitle->setAlignment(::Qt::AlignCenter);
            layoutStack->addWidget(lblTitle);
            //Description
            lblDescription = new QLabel(parent);
            lblDescription->setAlignment(::Qt::AlignCenter);
            layoutStack->addWidget(lblDescription);
            //Extra
            layoutExtra = new QVBoxLayout();
            layoutExtra->setSpacing(12);
            layoutStack->addLayout(layoutExtra);
            layoutStack->addStretch();
            //Main Layout
            QHBoxLayout* layoutMain{ new QHBoxLayout() };
            layoutMain->addStretch();
            layoutMain->addLayout(layoutStack);
            layoutMain->addStretch();
            parent->setLayout(layoutMain);
        }

        IconWidget* lblIcon;
        QLabel* lblTitle;
        QLabel* lblDescription;
        QVBoxLayout* layoutExtra;
    };
}

namespace Nickvision::TubeConverter::Qt::Controls
{
    StatusPage::StatusPage(QWidget* parent)
        : QWidget{ parent },
        m_ui{ new Ui::StatusPage() }
    {
        //Load Ui
        m_ui->setupUi(this);
    }

    StatusPage::~StatusPage()
    {
        delete m_ui;
    }

    void StatusPage::setIcon(const QIcon& icon)
    {
        m_ui->lblIcon->setIcon(icon);
    }

    void StatusPage::setTitle(const QString& title)
    {
        m_ui->lblTitle->setText(title);
    }

    void StatusPage::setDescription(const QString& description)
    {
        m_ui->lblDescription->setText(description);
    }

    void StatusPage::addWidget(QWidget* widget)
    {
        m_ui->layoutExtra->addWidget(widget, 0, ::Qt::AlignCenter);
    }

    void StatusPage::removeWidget(QWidget* widget)
    {
        m_ui->layoutExtra->removeWidget(widget);
    }
}
