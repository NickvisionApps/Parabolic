using Microsoft.UI.Xaml.Controls;

namespace NickvisionTubeConverter.WinUI.Controls;

/// <summary>
/// A basic control for showing a title and description
/// </summary>
public sealed partial class ActionRow : UserControl
{
    /// <summary>
    /// Constructs an ActionRow
    /// </summary>
    /// <param name="title">The title of the row</param>
    /// <param name="description">The description of the row</param>
    /// <param name="icon">The glyph icon of the row</param>
    /// <param name="tag">An optional tag for the row</param>
    public ActionRow(string title, string description, string icon, string? tag)
    {
        InitializeComponent();
        Title = title;
        Description = description;
        Icon.Glyph = icon;
        Tag = tag;
    }

    /// <summary>
    /// The title of the row
    /// </summary>
    public string Title
    {
        get => LblTitle.Text;

        set => LblTitle.Text = value;
    }

    /// <summary>
    /// The description of the row
    /// </summary>
    public string Description
    {
        get => LblDescription.Text;

        set => LblDescription.Text = value;
    }

    /// <summary>
    /// An optional tag for the row
    /// </summary>
    public new string? Tag
    {
        get => (base.Tag as string) ?? null;

        set => base.Tag = value;
    }
}
