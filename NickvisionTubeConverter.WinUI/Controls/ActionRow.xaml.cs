using CommunityToolkit.WinUI.UI.Controls;
using Microsoft.UI.Xaml;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A row with a title, subtitle, and optional suffix content
/// </summary>
public sealed partial class ActionRow : DockPanel, INotifyPropertyChanged
{
    public static DependencyProperty TitleProperty { get; } = DependencyProperty.Register("Title", typeof(string), typeof(ActionRow), new PropertyMetadata(null, (sender, e) => (sender as ActionRow)?.NotifyPropertyChanged(nameof(Title))));
    public static DependencyProperty SubtitleProperty { get; } = DependencyProperty.Register("Subtitle", typeof(string), typeof(ActionRow), new PropertyMetadata(null, (sender, e) => (sender as ActionRow)?.NotifyPropertyChanged(nameof(Subtitle))));

    public event PropertyChangedEventHandler? PropertyChanged;

    /// <summary>
    /// Constructs an ActionRow
    /// </summary>
    public ActionRow()
    {
        InitializeComponent();
    }

    /// <summary>
    /// Constructs an ActionRow
    /// </summary>
    /// <param name="title">The title of the row</param>
    /// <param name="subtitle">The subtitle of the row</param>
    public ActionRow(string title, string? subtitle)
    {
        InitializeComponent();
        Title = title;
        Subtitle = subtitle;
    }

    /// <summary>
    /// The title of the row
    /// </summary>
    public string? Title
    {
        get => (string?)GetValue(TitleProperty);

        set
        {
            SetValue(TitleProperty, value);
            NotifyPropertyChanged();
        }
    }

    /// <summary>
    /// The subtitle of the row
    /// </summary>
    public string? Subtitle
    {
        get => (string?)GetValue(SubtitleProperty);

        set
        {
            SetValue(SubtitleProperty, value);
            NotifyPropertyChanged();
        }
    }

    private void NotifyPropertyChanged([CallerMemberName] string propertyName = "") => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
}
