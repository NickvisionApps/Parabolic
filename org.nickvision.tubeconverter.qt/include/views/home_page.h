#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

namespace parabolic::qt::views
{
    class HomePage : public QWidget
    {
    public:
        HomePage(QWidget* parent = nullptr);
        ~HomePage() override = default;
        HomePage(const HomePage&) = delete;
        HomePage(HomePage&&) = delete;
        HomePage& operator=(const HomePage&) = delete;
        HomePage& operator=(HomePage&&) = delete;
    };
}

#endif
