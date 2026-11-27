#ifndef DOWNLOADSPAGE_H
#define DOWNLOADSPAGE_H

#include <QWidget>

namespace Ui
{
    class DownloadsPage;
}

namespace parabolic::qt::views
{
    class DownloadsPage : public QWidget
    {
        Q_OBJECT

    public:
        DownloadsPage(QWidget* parent = nullptr);
        ~DownloadsPage() override;
        DownloadsPage(const DownloadsPage&) = delete;
        DownloadsPage(DownloadsPage&&) = delete;
        DownloadsPage& operator=(const DownloadsPage&) = delete;
        DownloadsPage& operator=(DownloadsPage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::DownloadsPage* m_ui{ nullptr };
    };
}

#endif
