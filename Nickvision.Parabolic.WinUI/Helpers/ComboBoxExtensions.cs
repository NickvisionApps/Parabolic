using Microsoft.UI.Xaml.Controls;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class ComboBoxExtensions
{
    extension(ComboBox comboBox)
    {
        public void SelectSelectionItem<T>()
        {
            if(comboBox.ItemsSource is IReadOnlyCollection<SelectionItem<T>> items)
            {
                comboBox.SelectedItem = items.FirstOrDefault(item => item.ShouldSelect);
            }
        }
    }
}
