using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.ObjectModel;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// EventArgs for when a page is changed
/// </summary>
public class PageChangedEventArgs : EventArgs
{
    /// <summary>
    /// The previous page name
    /// </summary>
    public string Previous { get; init; }
    /// <summary>
    /// The current (new) page name
    /// </summary>
    public string Current { get; init; }

    /// <summary>
    /// Constricts a PageChangedEventArgs
    /// </summary>
    /// <param name="previous">The previous page name</param>
    /// <param name="current">The current (new) page name</param>
    public PageChangedEventArgs(string previous, string current)
    {
        Previous = previous;
        Current = current;
    }
}

/// <summary>
/// A control for showing a group of pages, but one page at a time
/// </summary>
public sealed partial class ViewStack : Frame
{
    public static DependencyProperty PagesProperty = DependencyProperty.Register("Pages", typeof(ObservableCollection<ViewStackPage>), typeof(ViewStack), new PropertyMetadata(new ObservableCollection<ViewStackPage>()));

    private string _currentPageName;

    /// <summary>
    /// The pages of the ViewStack
    /// </summary>
    public ObservableCollection<ViewStackPage> Pages => (ObservableCollection<ViewStackPage>)GetValue(PagesProperty);

    /// <summary>
    /// Occurs when the page is successfully changed
    /// </summary>
    public event EventHandler<PageChangedEventArgs>? PageChanged;

    /// <summary>
    /// Constructs a ViewStack
    /// </summary>
    public ViewStack()
    {
        InitializeComponent();
        _currentPageName = "";
        SetValue(PagesProperty, new ObservableCollection<ViewStackPage>());
    }

    /// <summary>
    /// The name of the current page;
    /// </summary>
    public string CurrentPageName
    {
        get => _currentPageName;

        set
        {
            var changed = false;
            foreach (var page in Pages)
            {
                if (page.PageName == value)
                {
                    Content = page;
                    changed = true;
                    break;
                }
            }
            if (changed)
            {
                var previous = _currentPageName;
                _currentPageName = value;
                PageChanged?.Invoke(this, new PageChangedEventArgs(previous, _currentPageName));
            }
        }
    }
}
