#include "helpers/translation_notifier.h"
#include <QObject>

namespace parabolic::qt::helpers
{
    TranslationNotifier::TranslationNotifier(QObject* parent)
        : QObject{ parent }
    {
    }
}
