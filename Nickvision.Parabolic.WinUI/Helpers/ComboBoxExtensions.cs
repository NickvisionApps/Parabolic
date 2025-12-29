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
            comboBox.SelectedIndex = 0;
            if (comboBox.ItemsSource is IReadOnlyList<SelectionItem<T>> items)
            {
                comboBox.SelectedItem = items.FirstOrDefault(item => item.ShouldSelect);
            }
        }

        public void SelectSelectionItemByFormatId(string id)
        {
            comboBox.SelectedIndex = 0;
            if (comboBox.ItemsSource is IReadOnlyList<SelectionItem<Format>> items)
            {
                comboBox.SelectedItem = items.FirstOrDefault(item => item.Value.Id == id);
            }
        }
    }
}
