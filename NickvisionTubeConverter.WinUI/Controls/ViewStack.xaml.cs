using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.ObjectModel;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A control for showing one page at a time
/// </summary>
public sealed partial class ViewStack : Frame
{
    public static DependencyProperty PagesProperty = DependencyProperty.Register("Pages", typeof(ObservableCollection<ViewStackPage>), typeof(ViewStack), new PropertyMetadata(new ObservableCollection<ViewStackPage>()));

    /// <summary>
    /// The pages of the ViewStack
    /// </summary>
    public ObservableCollection<ViewStackPage> Pages => (ObservableCollection<ViewStackPage>)GetValue(PagesProperty);

    /// <summary>
    /// Constructs a ViewStack
    /// </summary>
    public ViewStack()
    {
        InitializeComponent();
        SetValue(PagesProperty, new ObservableCollection<ViewStackPage>());
    }

    /// <summary>
    /// Changes the page of the ViewStack
    /// </summary>
    /// <param name="pageName">The name of the page to change to</param>
    /// <returns>True if successful, else false</returns>
    public bool ChangePage(string pageName)
    {
        foreach (var page in Pages)
        {
            if (page.PageName == pageName)
            {
                Content = page;
                return true;
            }
        }
        return false;
    }
}