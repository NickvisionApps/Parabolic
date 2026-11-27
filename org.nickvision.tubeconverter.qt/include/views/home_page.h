#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

namespace Ui
{
    class HomePage;
}

namespace parabolic::qt::views
{
    class HomePage : public QWidget
    {
        Q_OBJECT

    public:
        HomePage(QWidget* parent = nullptr);
        ~HomePage() override;
        HomePage(const HomePage&) = delete;
        HomePage(HomePage&&) = delete;
        HomePage& operator=(const HomePage&) = delete;
        HomePage& operator=(HomePage&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::HomePage* m_ui{ nullptr };
    };
}

#endif
