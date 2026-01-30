using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Parabolic.GNOME.Controls;
using Nickvision.Parabolic.GNOME.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.GNOME.Views;

public class HistoryDialog : Adw.PreferencesDialog
{
    private readonly HistoryViewController _controller;
    private readonly Gtk.Window _parent;
    private readonly Gtk.Builder _builder;
    private readonly List<Adw.ActionRow> _historyRows;

    [Gtk.Connect("sortGroup")]
    private Adw.ToggleGroup? _sortGroup;
    [Gtk.Connect("saveLengthRow")]
    private Adw.ComboRow? _saveLengthRow;
    [Gtk.Connect("historyGroup")]
    private Adw.PreferencesGroup? _historyGroup;
    [Gtk.Connect("clearButton")]
    private Gtk.Button? _clearButton;

    public HistoryDialog(HistoryViewController controller, Gtk.Window parent) : this(controller, parent, Gtk.Builder.NewFromBlueprint("HistoryDialog", controller.Translator))
    {

    }

    public HistoryDialog(HistoryViewController controller, Gtk.Window parent, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _parent = parent;
        _builder = builder;
        _historyRows = new List<Adw.ActionRow>();
        _builder.Connect(this);
        // Load
        _sortGroup!.ActiveName = _controller.SortNewest ? "newest" : "oldest";
        _saveLengthRow!.SetModel(_controller.Lengths);
        // Events
        _sortGroup!.OnNotify += SortGroup_OnNotify;
        _saveLengthRow!.OnNotify += SaveLengthRow_OnNotify;
        _clearButton!.OnClicked += ClearButton_OnClicked;
    }

    public async Task PresentAndLoadAsync()
    {
        Present(_parent);
        await LoadDownloadsAsync();
    }

    private async void SortGroup_OnNotify(GObject.Object sender, NotifySignalArgs e)
    {
        if (e.Pspec.GetName() == "active-name")
        {
            _controller.SortNewest = _sortGroup!.ActiveName == "newest";
            await LoadDownloadsAsync();
        }
    }

    private async void SaveLengthRow_OnNotify(GObject.Object sender, NotifySignalArgs e)
    {
        if (e.Pspec.GetName() == "selected-item")
        {
            _controller.Length = _controller.Lengths[(int)_saveLengthRow!.Selected].Value;
            await LoadDownloadsAsync();
        }
    }

    private void ClearButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        var dialog = Adw.AlertDialog.New(_controller.Translator._("Clear All History?"), _controller.Translator._("Are you sure you want to clear all download history? This action is irreversible"));
        dialog.AddResponse("clear", _controller.Translator._("Clear"));
        dialog.AddResponse("cancel", _controller.Translator._("Cancel"));
        dialog.SetResponseAppearance("clear", Adw.ResponseAppearance.Destructive);
        dialog.SetDefaultResponse("cancel");
        dialog.SetCloseResponse("cancel");
        dialog.OnResponse += async (_, e) =>
        {
            if (e.Response == "clear")
            {
                await _controller.ClearAllAsync();
                await LoadDownloadsAsync();
            }
        };
        dialog.Present(this);
    }

    private async Task LoadDownloadsAsync()
    {
        var loadingDialog = new LoadingDialog(_controller.Translator);
        loadingDialog.Present(this);
        foreach (var row in _historyRows)
        {
            _historyGroup!.Remove(row);
        }
        _historyRows.Clear();
        foreach (var historicDowload in await _controller.GetAllAsync())
        {
            var downloadAgainButton = Gtk.Button.NewFromIconName("folder-download-symbolic");
            downloadAgainButton.Valign = Gtk.Align.Center;
            downloadAgainButton.TooltipText = _controller.Translator._("Download Again");
            downloadAgainButton.AddCssClass("flat");
            downloadAgainButton.OnClicked += (_, _) => _controller.RequestDownload(historicDowload.Value.Url);
            var playButton = Gtk.Button.NewFromIconName("media-playback-start-symbolic");
            playButton.Valign = Gtk.Align.Center;
            playButton.TooltipText = _controller.Translator._("Play");
            playButton.Sensitive = historicDowload.Value.ExistsOnDisk;
            playButton.AddCssClass("flat");
            playButton.OnClicked += async (_, _) => await PlayAsync(historicDowload.Value.Path);
            var deleteButton = Gtk.Button.NewFromIconName("user-trash-symbolic");
            deleteButton.Valign = Gtk.Align.Center;
            deleteButton.TooltipText = _controller.Translator._("Delete");
            deleteButton.AddCssClass("flat");
            deleteButton.OnClicked += async (_, _) => await RemoveAsync(historicDowload.Value.Url);
            var row = Adw.ActionRow.New();
            row.Title = historicDowload.Label;
            row.Subtitle = historicDowload.Value.Url.ToString();
            row.TooltipText = _controller.Translator._("Downloaded On: {0}", $"{historicDowload.Value.DownloadedOn}");
            row.AddSuffix(downloadAgainButton);
            row.AddSuffix(playButton);
            row.AddSuffix(deleteButton);
            row.ActivatableWidget = historicDowload.Value.ExistsOnDisk ? playButton : downloadAgainButton;
            _historyRows.Add(row);
            _historyGroup!.Add(row);
        }
        if (_historyRows.Count == 0)
        {
            var row = Adw.ActionRow.New();
            row.Title = _controller.Translator._("No History");
            _historyRows.Add(row);
            _historyGroup!.Add(row);
        }
        loadingDialog.ForceClose();
    }

    private async Task PlayAsync(string path)
    {
        var launcher = Gtk.FileLauncher.New(Gio.FileHelper.NewForPath(path));
        await launcher.LaunchAsync(_parent);
    }

    private async Task RemoveAsync(Uri url)
    {
        await _controller.RemoveAsync(url);
        await LoadDownloadsAsync();
    }
}
