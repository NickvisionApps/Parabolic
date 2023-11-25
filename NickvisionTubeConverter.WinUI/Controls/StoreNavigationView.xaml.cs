using Microsoft.UI.Xaml.Controls;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A NavigationView control mimicing that of the Microsoft Store
/// </summary>
public sealed partial class StoreNavigationView : NavigationView
{
    /// <summary>
    /// Constructs a StoreNavigationView
    /// </summary>
    public StoreNavigationView()
    {
        InitializeComponent();
        SelectionChanged += StoreNavigationView_SelectionChanged;
    }

    /// <summary>
    /// Occurs when the selected item is changed
    /// </summary>
    /// <param name="sender">NavigationView</param>
    /// <param name="args">NavigationViewSelectionChangedEventArgs</param>
    private void StoreNavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
    {
        foreach (var i in MenuItems)
        {
            if (i is StoreNavigationViewItem item)
            {
                item.IsSelected = ReferenceEquals(i, SelectedItem);
            }
        }
        foreach (var i in FooterMenuItems)
        {
            if (i is StoreNavigationViewItem item)
            {
                item.IsSelected = ReferenceEquals(i, SelectedItem);
            }
        }
    }
}
