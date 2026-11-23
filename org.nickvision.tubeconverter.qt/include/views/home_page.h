#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QPaintEvent>
#include <QWidget>

namespace ui
{
    class HomePage;
}

namespace parabolic::qt::views
{
    class HomePage : public QWidget
    {
    public:
        HomePage(QWidget* parent = nullptr);
        ~HomePage() override;
        HomePage(const HomePage&) = delete;
        HomePage(HomePage&&) = delete;
        HomePage& operator=(const HomePage&) = delete;
        HomePage& operator=(HomePage&&) = delete;

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        ui::HomePage* m_ui{ nullptr };
    };
}

#endif
