#ifndef HISTORYPAGE_H
#define HISTORYPAGE_H

#include <QWidget>

namespace Ui
{
    class HistoryPage;
}

namespace parabolic::qt::pages
{
    class HistoryPage : public QWidget
    {
        Q_OBJECT

    public:
        HistoryPage(QWidget* parent = nullptr);
        ~HistoryPage() override;
        HistoryPage(const HistoryPage&) = delete;
        HistoryPage(HistoryPage&&) = delete;
        HistoryPage& operator=(const HistoryPage&) = delete;
        HistoryPage& operator=(HistoryPage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::HistoryPage* m_ui{ nullptr };
    };
}

#endif
