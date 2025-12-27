using Microsoft.UI.Xaml.Controls;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class ListViewExtensions
{
    extension(ListView listView)
    {
        public void SelectSelectionItems<T>()
        {
            if (listView.ItemsSource is IEnumerable<SelectionItem<T>> items)
            {
                foreach(var item in items.Where(i => i.ShouldSelect))
                {
                    listView.SelectedItems.Add(item);
                }
            }
        }
    }
}
