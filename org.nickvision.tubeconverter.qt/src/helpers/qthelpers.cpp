#include "helpers/qthelpers.h"
#include <QApplication>
#include <QString>
#include <QTimer>

namespace Nickvision::TubeConverter::QT::Helpers
{
    void QTHelpers::dispatchToMainThread(const std::function<void()>& function)
    {
        QTimer* timer{ new QTimer() };
        timer->moveToThread(QApplication::instance()->thread());
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [=]()
        {
            function();
            timer->deleteLater();
        });
        QMetaObject::invokeMethod(timer, "start", Qt::ConnectionType::AutoConnection, Q_ARG(int, 0));
    }

    void QTHelpers::setComboBoxItems(QComboBox* comboBox, const std::vector<std::string>& items)
    {
        comboBox->clear();
        for(const std::string& item : items)
        {
            comboBox->addItem(QString::fromStdString(item));
        }
        comboBox->setCurrentIndex(0);
    }
}