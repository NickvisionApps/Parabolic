using Nickvision.Aura;
using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using static Nickvision.Aura.Localization.Gettext;
using static Nickvision.GirExt.GtkExt;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A dialog to manage history
/// </summary>
public partial class HistoryDialog : Adw.Window
{
    private readonly DownloadHistory _history;
    private readonly List<Adw.ActionRow> _historyRows;

    [Gtk.Connect] private readonly Gtk.Button _clearButton;
    [Gtk.Connect] private readonly Gtk.SearchEntry _searchEntry;
    [Gtk.Connect] private readonly Adw.ViewStack _viewStack;
    [Gtk.Connect] private readonly Gtk.ScrolledWindow _scrolledWindow;
    [Gtk.Connect] private readonly Adw.PreferencesGroup _urlsGroup;

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
        _history = history;
        _historyRows = new List<Adw.ActionRow>();
        builder.Connect(this);
        //Dialog Settings
        SetIconName(iconName);
        SetTransientFor(parent);
        _clearButton.OnClicked += ClearHistory;
        _searchEntry.OnSearchChanged += SearchChanged;
        _searchEntry.SetVisible(_history.History.Count > 0);
        _viewStack.SetVisibleChildName(_history.History.Count > 0 ? "history" : "no-history");
        foreach (var pair in _history.History.OrderByDescending(x => x.Value.Date))
        {
            var row = Adw.ActionRow.New();
            if (string.IsNullOrEmpty(pair.Value.Title))
            {
                row.SetTitle(pair.Key);
            }
            else
            {
                row.SetTitle(pair.Value.Title);
                row.SetSubtitle(pair.Key);
            }
            row.SetTitleLines(1);
            row.SetSubtitleLines(1);
            if (File.Exists(pair.Value.Path))
            {
                var openButton = Gtk.Button.New();
                openButton.SetIconName("media-playback-start-symbolic");
                openButton.SetTooltipText(_("Play"));
                openButton.SetValign(Gtk.Align.Center);
                openButton.AddCssClass("flat");
                openButton.OnClicked += async (sender, e) =>
                {
                    var fileLauncher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath(pair.Value.Path));
                    try
                    {
                        await fileLauncher.LaunchAsync(this);
                    }
                    catch { }
                };
                row.AddSuffix(openButton);
            }
            var downloadButton = Gtk.Button.New();
            downloadButton.SetIconName("view-refresh-symbolic");
            downloadButton.SetTooltipText(_("Download Again"));
            downloadButton.SetValign(Gtk.Align.Center);
            downloadButton.AddCssClass("flat");
            downloadButton.OnClicked += (sender, e) =>
            {
                Close();
                DownloadAgainRequested?.Invoke(this, pair.Key);
            };
            row.AddSuffix(downloadButton);
            row.SetActivatableWidget(downloadButton);
            _urlsGroup.Add(row);
            _historyRows.Add(row);
        }
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
    /// Occurs when the clear history button is clicked
    /// </summary>
    /// <param name="sender">Gtk.Button</param>
    /// <param name="e">EventArgs</param>
    private void ClearHistory(Gtk.Button sender, EventArgs e)
    {
        _history.History.Clear();
        Aura.Active.SaveConfig("downloadHistory");
        //Update UI
        _searchEntry.SetVisible(false);
        _viewStack.SetVisibleChildName("no-history");
        foreach (var row in _historyRows)
        {
            _urlsGroup.Remove(row);
        }
        _historyRows.Clear();
    }

    /// <summary>
    /// Occurs when the search entry's text is changed
    /// </summary>
    /// <param name="sender">Gtk.SearchEntry</param>
    /// <param name="e">EventArgs</param>
    private void SearchChanged(Gtk.SearchEntry sender, EventArgs e)
    {
        var search = _searchEntry.GetText().ToLower();
        if (string.IsNullOrEmpty(search))
        {
            foreach (var row in _historyRows)
            {
                row.SetVisible(true);
            }
        }
        else
        {
            foreach (var row in _historyRows)
            {
                row.SetVisible(row.GetTitle().ToLower().Contains(search));
            }
        }
    }
}