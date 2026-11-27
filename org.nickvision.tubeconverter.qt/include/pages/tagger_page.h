#ifndef TAGGERPAGE_H
#define TAGGERPAGE_H

#include <QWidget>

namespace Ui
{
    class TaggerPage;
}

namespace parabolic::qt::pages
{
    class TaggerPage : public QWidget
    {
        Q_OBJECT

    public:
        TaggerPage(QWidget* parent = nullptr);
        ~TaggerPage() override;
        TaggerPage(const TaggerPage&) = delete;
        TaggerPage(TaggerPage&&) = delete;
        TaggerPage& operator=(const TaggerPage&) = delete;
        TaggerPage& operator=(TaggerPage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::TaggerPage* m_ui{ nullptr };
    };
}

#endif
