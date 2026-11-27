#ifndef UPDATESPAGE_H
#define UPDATESPAGE_H

#include <QWidget>

namespace Ui
{
    class UpdatesPage;
}

namespace parabolic::qt::pages
{
    class UpdatesPage : public QWidget
    {
        Q_OBJECT

    public:
        UpdatesPage(QWidget* parent = nullptr);
        ~UpdatesPage() override;
        UpdatesPage(const UpdatesPage&) = delete;
        UpdatesPage(UpdatesPage&&) = delete;
        UpdatesPage& operator=(const UpdatesPage&) = delete;
        UpdatesPage& operator=(UpdatesPage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::UpdatesPage* m_ui{ nullptr };
    };
}

#endif
