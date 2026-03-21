using Microsoft.UI.Xaml.Controls;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class ListViewExtensions
{
    extension(ListView listView)
    {
        public void SelectMediaSelectionItems()
        {
            if (listView.ItemsSource is IEnumerable<BindableMediaSelectionItem> bindableItems)
            {
                foreach (var item in bindableItems.Where(i => i.ShouldSelect))
                {
                    listView.SelectedItems.Add(item);
                }
            }
        }
    }
}
