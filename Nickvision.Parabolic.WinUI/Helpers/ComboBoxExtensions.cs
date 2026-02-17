using Microsoft.UI.Xaml.Controls;
using Nickvision.Desktop.Application;
using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class ComboBoxExtensions
{
    extension(ComboBox comboBox)
    {
        public void SelectSelectionItemByFormatId(string id)
        {
            if (comboBox.ItemsSource is IReadOnlyList<SelectionItem<Format>> items)
            {
                var selected = items.FirstOrDefault(item => item.Value.Id == id);
                if (selected is null)
                {
                    comboBox.SelectedIndex = 0;
                }
                else
                {
                    comboBox.SelectedItem = selected;
                }
            }
        }
    }
}
