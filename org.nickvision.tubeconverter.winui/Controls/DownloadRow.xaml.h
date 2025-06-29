#ifndef DOWNLOADROW_H
#define DOWNLOADROW_H

#include "pch.h"
#include "Controls/ViewStack.g.h"
#include "Controls/DownloadRow.g.h"

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::implementation
{
    /**
     * @brief A row control to display and manage a download.
     */
    struct DownloadRow : public DownloadRowT<DownloadRow>
    {
    public:
        DownloadRow();
    };
}

namespace winrt::Nickvision::TubeConverter::WinUI::Controls::factory_implementation
{
    struct DownloadRow : public DownloadRowT<DownloadRow, implementation::DownloadRow> { };
}

#endif //DOWNLOADROW_H
