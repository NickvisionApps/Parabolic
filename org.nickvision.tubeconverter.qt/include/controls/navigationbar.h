#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <unordered_map>
#include <QCommandLinkButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>
#include <QSpacerItem>
#include <QString>
#include <QVBoxLayout>

namespace Nickvision::TubeConverter::QT::Controls
{
    /**
     * @brief A navigation side bar that can be used to display a list of items to navigate to.
     */
    class NavigationBar : public QHBoxLayout
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a NavigationBar.
         * @param parent The parent widget
         */
        NavigationBar(QWidget* parent = nullptr);
        /**
         * @brief Gets the id of the selected item.
         * @return The id of the selected item
         */
        const QString& getSelectedItem() const;
        /**
         * @brief Adds an item to the top of the list.
         * @param id The id of the item
         * @param text The text of the item
         * @param icon The icon of the item
         */
        bool addTopItem(const QString& id, const QString& text, const QIcon& icon);
        /**
         * @brief Adds an item to the top of the list.
         * @brief This item will not be selectable and will show the menu popup when clicked.
         * @param id The id of the item
         * @param text The text of the item
         * @param icon The icon of the item
         * @param menu The menu of the item
         */
        bool addTopItem(const QString& id, const QString& text, const QIcon& icon, QMenu* menu);
        /**
         * @brief Adds an item to the bottom of the list.
         * @param id The id of the item
         * @param text The text of the item
         * @param icon The icon of the item
         */
        bool addBottomItem(const QString& id, const QString& text, const QIcon& icon);
        /**
         * @brief Adds an item to the bottom of the list.
         * @brief This item will not be selectable and will show the menu popup when clicked.
         * @param id The id of the item
         * @param text The text of the item
         * @param icon The icon of the item
         * @param menu The menu of the item
         */
        bool addBottomItem(const QString& id, const QString& text, const QIcon& icon, QMenu* menu);
        /**
         * @brief Removes an item from the list.
         * @param id The id of the item
         */
        bool removeItem(const QString& id);
        /**
         * @brief Selects an item in the list.
         * @param id The id of the item
         */
        bool selectItem(const QString& id);
        /**
         * @brief Changes the text of an item.
         * @param id The id of the item
         * @param text The new text of the item
         */
        bool changeItemText(const QString& id, const QString& text);
        /**
         * @brief Changes the icon of an item.
         * @param id The id of the item
         * @param icon The new icon of the item
         */
        bool changeItemIcon(const QString& id, const QIcon& icon);

    Q_SIGNALS:
        /**
         * @brief Emitted when an item is selected.
         * @param id The id of the item selected
         */
        void itemSelected(const QString& id);

    private Q_SLOTS:
        /**
         * @brief Handles when an item is clicked.
         */
        void onItemClicked();

    private:
        std::unordered_map<QString, QCommandLinkButton*> m_buttons;
        QWidget* m_parent;
        QVBoxLayout* m_listLayout;
        QVBoxLayout* m_topLayout;
        QVBoxLayout* m_bottomLayout;
        QFrame* m_line;
    };
}

#endif //NAVIAGTIONBAR_H