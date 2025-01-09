#ifndef CLOSEEVENTFILTER_H
#define CLOSEEVENTFILTER_H

#include <QEvent>
#include <QObject>

namespace Nickvision::TubeConverter::Qt::Helpers
{
    /**
     * @brief A filter for close events on Qt objects.
     */
    class CloseEventFilter : public QObject
    {
    Q_OBJECT

    public:
        /**
         * @brief Constructs a CloseEventFilter.
         * @param parent The parent object
         */
        CloseEventFilter(QObject* parent = nullptr);

    Q_SIGNALS:
        /**
         * @brief Emitted when an object who has this filter installed is closed.
         * @param obj The object that was closed
         */
        void closed(QObject* obj);

    protected:
        /**
         * @brief Filters close events.
         * @param obj The object that the event was sent to
         * @param event The event
         * @return True if the event was handled, otherwise false
         */
        bool eventFilter(QObject* obj, QEvent* event) override;
    };
}

#endif //CLOSEEVENTFILTER_H
