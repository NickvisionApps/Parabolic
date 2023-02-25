using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A page control for a ViewStack
/// </summary>
public sealed partial class ViewStackPage : Frame, INotifyPropertyChanged
{
    public static DependencyProperty PageNameProperty { get; } = DependencyProperty.Register("PageName", typeof(string), typeof(ViewStackPage), new PropertyMetadata("", (sender, e) => (sender as ViewStackPage)?.NotifyPropertyChanged(nameof(PageName))));

    public event PropertyChangedEventHandler? PropertyChanged;

    /// <summary>
    /// Constructs a ViewStackPage
    /// </summary>
    public ViewStackPage()
    {
        InitializeComponent();
    }

    /// <summary>
    /// The name of this page
    /// </summary>
    public string PageName
    {
        get => (string)GetValue(PageNameProperty);

        set
        {
            SetValue(PageNameProperty, value);
            NotifyPropertyChanged();
        }
    }

    private void NotifyPropertyChanged([CallerMemberName] string propertyName = "") => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}
