using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class SelectionItemListExtensions
{
    extension(IEnumerable<MediaSelectionItem> items)
    {
        public List<BindableMediaSelectionItem> ToBindableMediaSelectionItems() => items.Select(i => new BindableMediaSelectionItem(i)).ToList();
    }

    extension(IEnumerable<SelectionItem<Credential>> items)
    {
        public ObservableCollection<BindableCredentialSelectionItem> ToBindableCredentialSelectionItems() => new ObservableCollection<BindableCredentialSelectionItem>(items.Select(i => new BindableCredentialSelectionItem(i)));
    }

    extension(IEnumerable<SelectionItem<HistoricDownload>> items)
    {
        public List<BindableHistoricDownloadSelectionItem> ToBindableHistoricDownloadSelectionItems() => items.Select(i => new BindableHistoricDownloadSelectionItem(i)).ToList();
    }
}
