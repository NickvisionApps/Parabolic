using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.Collections.ObjectModel;

namespace NickvisionTubeConverter.WinUI.Controls;

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
                _currentPageName = value;
            }
        }
    }
}
