#ifndef HISTORYDIALOG_H
#define HISTORYDIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui
{
    class HistoryDialog;
}

namespace parabolic::qt::views
{
    class HistoryDialog : public QDialog
    {
        Q_OBJECT

    public:
        HistoryDialog(QWidget* parent = nullptr);
        ~HistoryDialog() override;
        HistoryDialog(const HistoryDialog&) = delete;
        HistoryDialog(HistoryDialog&&) = delete;
        HistoryDialog& operator=(const HistoryDialog&) = delete;
        HistoryDialog& operator=(HistoryDialog&&) = delete;

    private:
        Q_SLOT void translate();
        Ui::HistoryDialog* m_ui{ nullptr };
    };
}

#endif
