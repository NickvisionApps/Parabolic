#ifndef TRANSLATIONNOTIFIER_H
#define TRANSLATIONNOTIFIER_H

#include <QObject>

namespace parabolic::qt::helpers
{
    class TranslationNotifier : public QObject
    {
        Q_OBJECT

    public:
        TranslationNotifier(QObject* parent = nullptr);
        ~TranslationNotifier() override = default;
        TranslationNotifier(const TranslationNotifier&) = delete;
        TranslationNotifier(TranslationNotifier&&) = delete;
        TranslationNotifier& operator=(const TranslationNotifier&) = delete;
        TranslationNotifier& operator=(TranslationNotifier&&) = delete;

    Q_SIGNALS:
        void translation_changed();
    };
}

#endif
