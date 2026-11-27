#ifndef KEYRINGPAGE_H
#define KEYRINGPAGE_H

#include <QWidget>

namespace Ui
{
    class KeyringPage;
}

namespace parabolic::qt::pages
{
    class KeyringPage : public QWidget
    {
        Q_OBJECT

    public:
        KeyringPage(QWidget* parent = nullptr);
        ~KeyringPage() override;
        KeyringPage(const KeyringPage&) = delete;
        KeyringPage(KeyringPage&&) = delete;
        KeyringPage& operator=(const KeyringPage&) = delete;
        KeyringPage& operator=(KeyringPage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::KeyringPage* m_ui{ nullptr };
    };
}

#endif
