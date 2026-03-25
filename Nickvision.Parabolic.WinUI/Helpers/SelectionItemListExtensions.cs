using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
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

    extension(ObservableCollection<SelectionItem<Credential>> items)
    {
        public ObservableCollection<BindableCredentialSelectionItem> ToBindableCredentialSelectionItems()
        {
            var collection = new ObservableCollection<BindableCredentialSelectionItem>(items.Select(i => new BindableCredentialSelectionItem(i)));
            items.CollectionChanged += (_, e) =>
            {
                if (collection is null)
                {
                    return;
                }
                switch (e.Action)
                {
                    case NotifyCollectionChangedAction.Add when e.NewItems is not null:
                        foreach (SelectionItem<Credential> item in e.NewItems)
                        {
                            collection.Add(new BindableCredentialSelectionItem(item));
                        }
                        break;
                    case NotifyCollectionChangedAction.Remove when e.OldItems is not null:
                        foreach (SelectionItem<Credential> item in e.OldItems)
                        {
                            var bindable = collection.FirstOrDefault(b => b.Value == item.Value);
                            if (bindable is not null)
                            {
                                collection.Remove(bindable);
                            }
                        }
                        break;
                    case NotifyCollectionChangedAction.Replace when e.NewItems is not null && e.NewStartingIndex >= 0 && e.NewStartingIndex < collection.Count:
                        collection[e.NewStartingIndex] = new BindableCredentialSelectionItem((SelectionItem<Credential>)e.NewItems[0]!);
                        break;
                    default:
                        collection.Clear();
                        foreach (var item in items)
                        {
                            collection.Add(new BindableCredentialSelectionItem(item));
                        }
                        break;
                }
            };
            return collection;
        }
    }

    extension(IEnumerable<SelectionItem<HistoricDownload>> items)
    {
        public List<BindableHistoricDownloadSelectionItem> ToBindableHistoricDownloadSelectionItems() => items.Select(i => new BindableHistoricDownloadSelectionItem(i)).ToList();
    }

    extension(ObservableCollection<PostProcessorArgument> items)
    {
        public ObservableCollection<BindablePostProcessorArgument> ToBindablePostProcessorArguments()
        {
            var collection = new ObservableCollection<BindablePostProcessorArgument>(items.Select(i => new BindablePostProcessorArgument(i)));
            items.CollectionChanged += (_, e) =>
            {
                if (collection is null)
                {
                    return;
                }
                switch (e.Action)
                {
                    case NotifyCollectionChangedAction.Add when e.NewItems is not null:
                        foreach (PostProcessorArgument item in e.NewItems)
                        {
                            collection.Add(new BindablePostProcessorArgument(item));
                        }
                        break;
                    case NotifyCollectionChangedAction.Remove when e.OldItems is not null:
                        foreach (PostProcessorArgument item in e.OldItems)
                        {
                            var bindable = collection.FirstOrDefault(b => b.Name == item.Name);
                            if (bindable is not null)
                            {
                                collection.Remove(bindable);
                            }
                        }
                        break;
                    case NotifyCollectionChangedAction.Replace when e.NewItems is not null && e.NewStartingIndex >= 0 && e.NewStartingIndex < collection.Count:
                        collection[e.NewStartingIndex] = new BindablePostProcessorArgument((PostProcessorArgument)e.NewItems[0]!);
                        break;
                    default:
                        collection.Clear();
                        foreach (var item in items)
                        {
                            collection.Add(new BindablePostProcessorArgument(item));
                        }
                        break;
                }
            };
            return collection;
        }
    }
}
