#ifndef HISTORYPANE_H
#define HISTORYPANE_H

#include <vector>
#include <QDockWidget>
#include "models/historicdownload.h"

namespace Ui { class HistoryPane; }

namespace Nickvision::TubeConverter::Qt::Controls
{
    /**
     * @brief A side pane for working with download history.
     */
    class HistoryPane : public QDockWidget
    {
    public:
        /**
         * @brief Constructs a HistoryPane.
         * @param parent The parent widget
         */
        HistoryPane(QWidget* parent = nullptr);
        /**
         * @brief Destructs a HistoryPane.
         */
        ~HistoryPane();
        /**
         * @brief Updates the history in the pane.
         * @param history The new history
         */
        void update(const std::vector<Shared::Models::HistoricDownload>& history);

    private:
        Ui::HistoryPane* m_ui;
    };
}

#endif //HISTORYPANE_H
