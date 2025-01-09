#include "controls/navigationbar.h"

namespace Nickvision::TubeConverter::Qt::Controls
{
    NavigationBar::NavigationBar(QWidget* parent)
        : QHBoxLayout{ parent },
        m_parent{ parent },
        m_listLayout{ new QVBoxLayout() },
        m_topLayout{ new QVBoxLayout() },
        m_bottomLayout{ new QVBoxLayout() },
        m_line{ new QFrame(parent) }
    {
        setContentsMargins({ 6, 6, 0, 6 });
        setSpacing(6);
        //Make Vertical Line
        m_line->setFrameShape(QFrame::VLine);
        m_line->setFrameShadow(QFrame::Sunken);
        //Make Layout
        m_listLayout->addLayout(m_topLayout);
        m_listLayout->addStretch();
        m_listLayout->addLayout(m_bottomLayout);
        addLayout(m_listLayout);
        addWidget(m_line);
    }

    const QString& NavigationBar::getSelectedItem() const
    {
        for(const std::pair<const QString, QCommandLinkButton*>& pair : m_buttons)
        {
            if(pair.second->isChecked())
            {
                return pair.first;
            }
        }
        static QString empty;
        return empty;
    }

    bool NavigationBar::addTopItem(const QString& id, const QString& text, const QIcon& icon)
    {
        if(m_buttons.contains(id))
        {
            return false;
        }
        QCommandLinkButton* button{ new QCommandLinkButton(text, m_parent) };
        button->setIcon(icon);
        button->setCheckable(true);
        button->setChecked(false);
        m_buttons[id] = button;
        m_topLayout->addWidget(button);
        connect(button, &QCommandLinkButton::clicked, this, &NavigationBar::onItemClicked);
        return true;
    }

    bool NavigationBar::addTopItem(const QString& id, const QString& text, const QIcon& icon, QMenu* menu)
    {
        if(m_buttons.contains(id))
        {
            return false;
        }
        QCommandLinkButton* button{ new QCommandLinkButton(text, m_parent) };
        button->setIcon(icon);
        button->setMenu(menu);
        m_buttons[id] = button;
        m_topLayout->addWidget(button);
        return true;
    }

    bool NavigationBar::addBottomItem(const QString& id, const QString& text, const QIcon& icon)
    {
        if(m_buttons.contains(id))
        {
            return false;
        }
        QCommandLinkButton* button{ new QCommandLinkButton(text, m_parent) };
        button->setIcon(icon);
        button->setCheckable(true);
        button->setChecked(false);
        m_buttons[id] = button;
        m_bottomLayout->addWidget(button);
        connect(button, &QCommandLinkButton::clicked, this, &NavigationBar::onItemClicked);
        return true;
    }

    bool NavigationBar::addBottomItem(const QString& id, const QString& text, const QIcon& icon, QMenu* menu)
    {
        if(m_buttons.contains(id))
        {
            return false;
        }
        QCommandLinkButton* button{ new QCommandLinkButton(text, m_parent) };
        button->setIcon(icon);
        button->setMenu(menu);
        m_buttons[id] = button;
        m_bottomLayout->addWidget(button);
        return true;
    }

    bool NavigationBar::removeItem(const QString& id)
    {
        if(!m_buttons.contains(id))
        {
            return false;
        }
        QCommandLinkButton* button{ m_buttons[id] };
        m_buttons.erase(id);
        m_topLayout->removeWidget(button);
        m_bottomLayout->removeWidget(button);
        delete button;
        return true;
    }

    bool NavigationBar::selectItem(const QString& id)
    {
        if(!m_buttons.contains(id))
        {
            return false;
        }
        for(const std::pair<const QString, QCommandLinkButton*>& pair : m_buttons)
        {
            if(pair.second->menu())
            {
                continue;
            }
            if(pair.first == id)
            {
                pair.second->setChecked(true);
                Q_EMIT itemSelected(pair.first);
            }
            else
            {
                pair.second->setChecked(false);
            }
        }
        return true;
    }

    bool NavigationBar::changeItemText(const QString& id, const QString& text)
    {
        if(!m_buttons.contains(id))
        {
            return false;
        }
        m_buttons[id]->setText(text);
        return true;
    }

    bool NavigationBar::changeItemIcon(const QString& id, const QIcon& icon)
    {
        if(!m_buttons.contains(id))
        {
            return false;
        }
        m_buttons[id]->setIcon(icon);
        return true;
    }

    void NavigationBar::onItemClicked()
    {
        QCommandLinkButton* button{ qobject_cast<QCommandLinkButton*>(sender()) };
        if(button)
        {
            for(const std::pair<const QString, QCommandLinkButton*>& pair : m_buttons)
            {
                if(pair.second->menu())
                {
                    continue;
                }
                if(pair.second == button)
                {
                    pair.second->setChecked(true);
                    Q_EMIT itemSelected(pair.first);
                }
                else
                {
                    pair.second->setChecked(false);
                }
            }
        }
    }
}