#ifndef STATUSPAGE_H
#define STATUSPAGE_H

#include <QIcon>
#include <QString>
#include <QWidget>

namespace Ui { class StatusPage; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A page for displaying the statuses.
     */
    class StatusPage : public QWidget
    {
        Q_OBJECT

    public:
        /**
         * @brief Constructs a StatusPage.
         * @param parent The parent widget
         */
        StatusPage(QWidget* parent = nullptr);
        /**
         * @brief Destructs a StatusPage.
         */
        ~StatusPage();
        /**
         * @brief Sets the icon.
         * @param icon The icon
         */
        void setIcon(const QIcon& icon);
        /**
         * @brief Sets the title.
         * @param title The title
         */
        void setTitle(const QString& title);
        /**
         * @brief Sets the description.
         * @param description The description
         */
        void setDescription(const QString& description);
        /**
         * @brief Adds an extra widget.
         * @param widget The widget
         */
        void addWidget(QWidget* widget);
        /**
         * @brief Removes an extra widget.
         * @param widget The widget
         */
        void removeWidget(QWidget* widget);

    private:
        Ui::StatusPage* m_ui;
    };
}

#endif //STATUSPAGE_H
