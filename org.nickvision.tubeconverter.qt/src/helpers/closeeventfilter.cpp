#include "helpers/closeeventfilter.h"

namespace Nickvision::TubeConverter::Qt::Helpers
{
    CloseEventFilter::CloseEventFilter(QObject* parent)
        : QObject{ parent }
    {

    }

    bool CloseEventFilter::eventFilter(QObject* obj, QEvent* event)
    {
        if(event->type() == QEvent::Close)
        {
            Q_EMIT closed(obj);
            return true;
        }
        return QObject::eventFilter(obj, event);
    }
}