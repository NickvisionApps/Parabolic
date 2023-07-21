using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A dialog to manage history
/// </summary>
public partial class HistoryDialog : Adw.Window
{
    private readonly List<Adw.ActionRow> _historyRows;

    [Gtk.Connect] private readonly Gtk.Button _clearButton;
    [Gtk.Connect] private readonly Gtk.SearchEntry _searchEntry;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _scrolledWindow;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _urlsGroup;

    private readonly Gtk.ShortcutController _shortcutController;
    
    /// <summary>
    /// Occurs when a download is requested to be downloaded again
    /// </summary>
    public event EventHandler<string>? DownloadAgainRequested;

    /// <summary>
    /// Constructs a HistoryDialog
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="parent">Gtk.Window</param>
    /// <param name="iconName">Icon name for the window</param>
    /// <param name="history">The DownloadHistory object</param>
    private HistoryDialog(Gtk.Builder builder, Gtk.Window parent, string iconName, DownloadHistory history) : base(builder.GetPointer("_root"), false)
    {
        _historyRows = new List<Adw.ActionRow>();
        builder.Connect(this);
        //Dialog Settings
        SetIconName(iconName);
        SetTransientFor(parent);
        _clearButton.OnClicked += (sender, e) =>
        {
            history.History.Clear();
            history.Save();
            Close();
        };
        _searchEntry.OnSearchChanged += SearchChanged;
        foreach (var pair in history.History)
        {
            var row = Adw.ActionRow.New();
            if(string.IsNullOrEmpty(pair.Value))
            {
                row.SetTitle(pair.Key);
            }
            else
            {
                row.SetTitle(pair.Value);
                row.SetSubtitle(pair.Key);
            }
            row.SetTitleLines(1);
            row.SetSubtitleLines(1);
            var button = Gtk.Button.New();
            button.SetIconName("view-refresh-symbolic");
            button.SetTooltipText(_("Download Again"));
            button.SetValign(Gtk.Align.Center);
            button.AddCssClass("flat");
            button.OnClicked += (sender, e) =>
            {
                DownloadAgainRequested?.Invoke(this, pair.Key);
            };
            row.AddSuffix(button);
            row.SetActivatableWidget(button);
            _urlsGroup.Add(row);
            _historyRows.Add(row);
        }
        //Shortcut Controller
        _shortcutController = Gtk.ShortcutController.New();
        _shortcutController.SetScope(Gtk.ShortcutScope.Managed);
        _shortcutController.AddShortcut(Gtk.Shortcut.New(Gtk.ShortcutTrigger.ParseString("Escape"), Gtk.CallbackAction.New(OnEscapeKey)));
        AddController(_shortcutController);
    }

    /// <summary>
    /// Constructs a HistoryDialog
    /// </summary>
    /// <param name="parent">Gtk.Window</param>
    /// <param name="iconName">Icon name for the window</param>
    /// <param name="history">The DownloadHistory object</param>
    public HistoryDialog(Gtk.Window parent, string iconName, DownloadHistory history) : this(Builder.FromFile("history_dialog.ui"), parent, iconName, history)
    {
    }

    /// <summary>
    /// Occurs when the escape key is pressed on the window
    /// </summary>
    /// <param name="sender">Gtk.Widget</param>
    /// <param name="e">GLib.Variant</param>
    private bool OnEscapeKey(Gtk.Widget sender, GLib.Variant e)
    {
        Close();
        return true;
    }

    /// <summary>
    /// Occurs when the search entry's text is changed
    /// </summary>
    /// <param name="sender">Gtk.SearchEntry</param>
    /// <param name="e">EventArgs</param>
    private void SearchChanged(Gtk.SearchEntry sender, EventArgs e)
    {
        var search = _searchEntry.GetText().ToLower();
        if(string.IsNullOrEmpty(search))
        {
            foreach (var row in _historyRows)
            {
                row.SetVisible(true);
            }
        }
        else
        {
            foreach(var row in _historyRows)
            {
                row.SetVisible(row.GetTitle().ToLower().Contains(search));
            }
        }
    }
}