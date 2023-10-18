using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A control for displaying a simple page with a title, description, and icon
/// </summary>
public sealed partial class StatusPage : UserControl, INotifyPropertyChanged
{
    public static DependencyProperty GlyphProperty { get; } = DependencyProperty.Register("Glyph", typeof(string), typeof(StatusPage), new PropertyMetadata("", (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(Glyph))));
    public static DependencyProperty UseAppIconProperty { get; } = DependencyProperty.Register("UseAppIcon", typeof(bool), typeof(StatusPage), new PropertyMetadata("", (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(UseAppIcon))));
    public static DependencyProperty TitleProperty { get; } = DependencyProperty.Register("Title", typeof(string), typeof(StatusPage), new PropertyMetadata("", (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(Title))));
    public static DependencyProperty DescriptionProperty { get; } = DependencyProperty.Register("Description", typeof(string), typeof(StatusPage), new PropertyMetadata("", (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(Description))));
    public static DependencyProperty ChildProperty { get; } = DependencyProperty.Register("Child", typeof(UIElement), typeof(StatusPage), new PropertyMetadata(null, (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(Child))));
    public static DependencyProperty IsCompactProperty { get; } = DependencyProperty.Register("IsCompact", typeof(bool), typeof(StatusPage), new PropertyMetadata(false, (sender, e) => (sender as StatusPage)?.NotifyPropertyChanged(nameof(IsCompact))));

    public event PropertyChangedEventHandler? PropertyChanged;

    public StatusPage()
    {
        InitializeComponent();
        Title = "";
        Description = "";
        Child = null;
        IsCompact = false;
    }

    /// <summary>
    /// The glyph code for the FontIcon
    /// </summary>
    public string Glyph
    {
        get => (string)GetValue(GlyphProperty);

        set
        {
            SetValue(GlyphProperty, value);
            if (!string.IsNullOrEmpty(value))
            {
                GlyphIcon.Visibility = Visibility.Visible;
                AppIcon.Visibility = Visibility.Collapsed;
            }
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// Whether or not to use the app icon instead of a glyph
    /// </summary>
    public bool UseAppIcon
    {
        get => (bool)GetValue(UseAppIconProperty);

        set
        {
            SetValue(UseAppIconProperty, value);
            if (value)
            {
                GlyphIcon.Visibility = Visibility.Collapsed;
                AppIcon.Visibility = Visibility.Visible;
            }
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// The title of the status
    /// </summary>
    public string Title
    {
        get => (string)GetValue(TitleProperty);

        set
        {
            SetValue(TitleProperty, value);
            LblTitle.Visibility = !string.IsNullOrEmpty(value) ? Visibility.Visible : Visibility.Collapsed;
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// The message of the status
    /// </summary>
    public string Description
    {
        get => (string)GetValue(DescriptionProperty);

        set
        {
            SetValue(DescriptionProperty, value);
            LblDescription.Visibility = !string.IsNullOrEmpty(value) ? Visibility.Visible : Visibility.Collapsed;
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// The extra child of the page
    /// </summary>
    public UIElement? Child
    {
        get => (UIElement?)GetValue(ChildProperty);

        set
        {
            SetValue(ChildProperty, value);
            FrameChild.Visibility = value != null ? Visibility.Visible : Visibility.Collapsed;
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// Whether or not the page is styled in a compact fashion
    /// </summary>
    public bool IsCompact
    {
        get => (bool)GetValue(IsCompactProperty);

        set
        {
            SetValue(IsCompactProperty, value);
            if (value)
            {
                StackPanel.Spacing = 6;
                GlyphIcon.FontSize = 30;
                AppIcon.Width = 64;
                AppIcon.Height = 64;
                LblTitle.Style = (Style)Application.Current.Resources["SubtitleTextBlockStyle"];
            }
            else
            {
                StackPanel.Spacing = 12;
                GlyphIcon.FontSize = 60;
                AppIcon.Width = 128;
                AppIcon.Height = 128;
                LblTitle.Style = (Style)Application.Current.Resources["TitleTextBlockStyle"];
            }
            NotifyPropertyChanged();
        }
    }

    private void NotifyPropertyChanged([CallerMemberName] string propertyName = "") => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}
