#include "controls/infobar.h"
#include "ui_infobar.h"
#include <libnick/localization/gettext.h>

using namespace Nickvision::Notifications;

namespace Nickvision::TubeConverter::Qt::Controls
{
    InfoBar::InfoBar(QWidget* parent)
        : QDockWidget{ parent },
        m_ui{ new Ui::InfoBar() }
    {
        m_ui->setupUi(this);
        setWindowTitle(_("Notification"));
        connect(m_ui->btnAction, &QPushButton::clicked, this, &InfoBar::action);
        hide();
    }

    InfoBar::~InfoBar()
    {
        delete m_ui;
    }

    void InfoBar::action()
    {
        hide();
        m_callback();
    }

    void InfoBar::show(const NotificationSentEventArgs& args, const QString& actionButtonText, const std::function<void()>& callback)
    {
        static QPixmap info{ QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation).pixmap(16, 16) };
        static QPixmap warning{ QIcon::fromTheme(QIcon::ThemeIcon::DialogWarning).pixmap(16, 16) };
        static QPixmap error{ QIcon::fromTheme(QIcon::ThemeIcon::DialogError).pixmap(16, 16) };
        QDockWidget::show();
        if(!actionButtonText.isEmpty() && callback)
        {
            m_ui->btnAction->setText(actionButtonText);
            m_ui->btnAction->setVisible(true);
            m_callback = callback;
        }
        else
        {
            m_ui->btnAction->setVisible(false);
            m_callback = {};
        }
        m_ui->lblMessage->setText(QString::fromStdString(args.getMessage()));
        switch(args.getSeverity())
        {
        case NotificationSeverity::Informational:
        case NotificationSeverity::Success:
            m_ui->lblIcon->setPixmap(info);
            break;
        case NotificationSeverity::Warning:
            m_ui->lblIcon->setPixmap(warning);
            break;
        case NotificationSeverity::Error:
            m_ui->lblIcon->setPixmap(error);
            break;
        }
    }
}
