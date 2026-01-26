using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.Shared.Controllers;
using Nickvision.Parabolic.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.GNOME.Views;

public class AddDownloadDialog : Adw.Dialog
{
    private readonly AddDownloadDialogController _controller;
    private readonly Gtk.Window _parent;
    private readonly Gtk.Builder _builder;
    private CancellationTokenSource? _cancellationTokenSource;
    private DiscoveryContext? _discoveryContext;
    private readonly List<Adw.ActionRow> _singleSubtitleRows;

    [Gtk.Connect("navigationView")]
    private Adw.NavigationView? _navigationView;
    [Gtk.Connect("urlRow")]
    private Adw.EntryRow? _urlRow;
    [Gtk.Connect("selectBatchFileRow")]
    private Adw.ButtonRow? _selectBatchFileRow;
    [Gtk.Connect("authenticationRow")]
    private Adw.ExpanderRow? _authenticationRow;
    [Gtk.Connect("authenticationCredentialRow")]
    private Adw.ComboRow? _authenticationCredentialRow;
    [Gtk.Connect("authenticationUsernameRow")]
    private Adw.EntryRow? _authenticationUsernameRow;
    [Gtk.Connect("authenticationPasswordRow")]
    private Adw.PasswordEntryRow? _authenticationPasswordRow;
    [Gtk.Connect("downloadImmediatelyRow")]
    private Adw.SwitchRow? _downloadImmediatelyRow;
    [Gtk.Connect("discoverUrlButton")]
    private Gtk.Button? _discoverUrlButton;
    [Gtk.Connect("singleViewStack")]
    private Adw.ViewStack? _singleViewStack;
    [Gtk.Connect("singleGroup")]
    private Adw.PreferencesGroup? _singleGroup;
    [Gtk.Connect("singleSaveFilenameRow")]
    private Adw.EntryRow? _singleSaveFilenameRow;
    [Gtk.Connect("singleRevertToTitleButton")]
    private Gtk.Button? _singleRevertToTitleButton;
    [Gtk.Connect("singleSaveFolderRow")]
    private Adw.ActionRow? _singleSaveFolderRow;
    [Gtk.Connect("singleSelectSaveFolderButton")]
    private Gtk.Button? _singleSelectSaveFolderButton;
    [Gtk.Connect("singleFileTypeRow")]
    private Adw.ComboRow? _singleFileTypeRow;
    [Gtk.Connect("singleVideoFormatRow")]
    private Adw.ComboRow? _singleVideoFormatRow;
    [Gtk.Connect("singleAudioFormatRow")]
    private Adw.ComboRow? _singleAudioFormatRow;
    [Gtk.Connect("singleSubtitlesPage")]
    private Adw.PreferencesPage? _singleSubtitlesPage;
    [Gtk.Connect("singleSelectAllSubtitlesRow")]
    private Adw.ButtonRow? _singleSelectAllSubtitlesRow;
    [Gtk.Connect("singleDeselectAllSubtitlesRow")]
    private Adw.ButtonRow? _singleDeselectAllSubtitlesRow;
    [Gtk.Connect("singleSubtitlesGroup")]
    private Adw.PreferencesGroup? _singleSubtitlesGroup;
    [Gtk.Connect("singleSplitChaptersRow")]
    private Adw.SwitchRow? _singleSplitChaptersRow;
    [Gtk.Connect("singleExportDescriptionRow")]
    private Adw.SwitchRow? _singleExportDescriptionRow;
    [Gtk.Connect("singleExcludeFromHistoryRow")]
    private Adw.SwitchRow? _singleExcludeFromHistoryRow;
    [Gtk.Connect("singlePostProcessorArgumentRow")]
    private Adw.ComboRow? _singlePostProcessorArgumentRow;
    [Gtk.Connect("singleStartTimeRow")]
    private Adw.EntryRow? _singleStartTimeRow;
    [Gtk.Connect("singleEndTimeRow")]
    private Adw.EntryRow? _singleEndTimeRow;
    [Gtk.Connect("singleDownloadHeaderButton")]
    private Gtk.Button? _singleDownloadHeaderButton;
    [Gtk.Connect("singleDownloadButton")]
    private Gtk.Button? _singleDownloadButton;

    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(controller, parent, Gtk.Builder.NewFromBlueprint("AddDownloadDialog", controller.Translator))
    {

    }

    private AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _parent = parent;
        _builder = builder;
        _cancellationTokenSource = null;
        _discoveryContext = null;
        _singleSubtitleRows = new List<Adw.ActionRow>();
        _builder.Connect(this);
        // Load
        _authenticationCredentialRow!.SetModel(_controller.AvailableCredentials);
        // Events
        OnClosed += Dialog_OnClosed;
        _urlRow!.OnChanged += UrlRow_OnChanged;
        _selectBatchFileRow!.OnActivated += SelectBathFileRow_OnActivated;
        _authenticationCredentialRow!.OnNotify += AuthenticationCredentialRow_OnNotify;
        _discoverUrlButton!.OnClicked += DiscoverUrlButton_OnClicked;
        _singleRevertToTitleButton!.OnClicked += SingleRevertToTitleButton_OnClicked;
        _singleSelectSaveFolderButton!.OnClicked += SingleSelectSaveFolderButton_OnClicked;
        _singleFileTypeRow!.OnNotify += SingleFileTypeRow_OnNotify;
        _singleSelectAllSubtitlesRow!.OnActivated += SingleSelectAllSubtitlesRow_OnActivated;
        _singleDeselectAllSubtitlesRow!.OnActivated += SingleDeselectAllSubtitlesRow_OnActivated;
        _singleDownloadHeaderButton!.OnClicked += async (sender, e) => await DownloadSingleAsync();
        _singleDownloadButton!.OnClicked += async (sender, e) => await DownloadSingleAsync();
    }

    public async Task PresentWithClipboardAsync()
    {
        try
        {
            var text = await Gdk.Display.GetDefault()!.GetClipboard().ReadTextAsync();
            if (!string.IsNullOrEmpty(text) && Uri.TryCreate(text, UriKind.Absolute, out var url))
            {
                _urlRow!.Text_ = url.ToString();
            }
        }
        catch { }
        Present(_parent);
    }

    public void Present(Uri url)
    {
        _urlRow!.Text_ = url.ToString();
        Present(_parent);
    }

    private void Dialog_OnClosed(Adw.Dialog sender, EventArgs e) => _cancellationTokenSource?.Cancel();

    private void UrlRow_OnChanged(Gtk.Editable sender, EventArgs e) => _discoverUrlButton!.Sensitive = Uri.TryCreate(_urlRow!.Text_, UriKind.Absolute, out var _);

    private async void SelectBathFileRow_OnActivated(Adw.ButtonRow sender, EventArgs e)
    {
        var fileDialog = Gtk.FileDialog.New();
        fileDialog.Title = _controller.Translator._("Select Batch File");
        var filter = Gtk.FileFilter.New();
        filter.Name = _controller.Translator._("TXT Files (*.txt)");
        filter.AddPattern("*.txt");
        filter.AddPattern("*.TXT");
        var filters = Gio.ListStore.New(Gtk.FileFilter.GetGType());
        filters.Append(filter);
        fileDialog.SetFilters(filters);
        try
        {
            var res = await fileDialog.OpenAsync(_parent);
            if (res is not null)
            {
                _urlRow!.Text_ = new Uri($"file://{res.GetPath()}").ToString();
            }
        }
        catch { }
    }

    private void AuthenticationCredentialRow_OnNotify(GObject.Object sender, NotifySignalArgs e)
    {
        if (e.Pspec.GetName() == "selected-item")
        {
            var visible = _controller.AvailableCredentials[(int)_authenticationCredentialRow!.Selected].Value is null;
            _authenticationUsernameRow!.Visible = visible;
            _authenticationPasswordRow!.Visible = visible;
        }
    }

    private async void DiscoverUrlButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        _cancellationTokenSource = new CancellationTokenSource();
        _navigationView!.PushByTag("loading");
        Credential? credential = null;
        if (!string.IsNullOrEmpty(_authenticationUsernameRow!.Text_) && !string.IsNullOrEmpty(_authenticationPasswordRow!.Text_))
        {
            credential = new Credential("manual", _authenticationUsernameRow.Text_, _authenticationPasswordRow.Text_);
        }
        else
        {
            credential = _controller.AvailableCredentials[(int)_authenticationCredentialRow!.Selected].Value;
        }
        _discoveryContext = await _controller.DiscoverAsync(new Uri(_urlRow!.Text_!), credential, _cancellationTokenSource.Token);
        _cancellationTokenSource.Dispose();
        _cancellationTokenSource = null;
        if (_discoveryContext is null)
        {
            Close();
            return;
        }
        ContentWidth = 550;
        ContentHeight = 500;
        _controller.PreviousDownloadOptions.DownloadImmediately = _downloadImmediatelyRow!.Active;
        _singleGroup!.Title = _discoveryContext.Title;
        _singleGroup!.Description = GLib.Markup.EscapeText(_discoveryContext.Url.ToString());
        if (_discoveryContext.Items.Count == 1)
        {
            _navigationView.PushByTag("single");
            _singleSaveFilenameRow!.Text_ = _discoveryContext.Items[0].Label;
            _singleSaveFolderRow!.Subtitle = _controller.PreviousDownloadOptions.SaveFolder;
            _singleVideoFormatRow!.SetModel(_discoveryContext.VideoFormats, false);
            _singleAudioFormatRow!.SetModel(_discoveryContext.AudioFormats, false);
            _singleFileTypeRow!.SetModel(_discoveryContext.FileTypes);
            if(_discoveryContext.SubtitleLanguages.Count > 0)
            {
                _singleViewStack!.GetPage(_singleSubtitlesPage!).BadgeNumber = (uint)_discoveryContext.SubtitleLanguages.Count;
            }
            else
            {
                var row = Adw.ActionRow.New();
                row.Title = _controller.Translator._("No Subtitles Available");
                _singleSubtitlesGroup!.Add(row);
            }
            foreach (var subtitle in _discoveryContext.SubtitleLanguages)
            {
                var chk = Gtk.CheckButton.New();
                chk.Valign = Gtk.Align.Center;
                chk.AddCssClass("selection-mode");
                chk.Active = subtitle.ShouldSelect;
                var row = Adw.ActionRow.New();
                row.UseMarkup = false;
                row.Title = subtitle.Label;
                row.AddPrefix(chk);
                row.ActivatableWidget = chk;
                _singleSubtitleRows.Add(row);
                _singleSubtitlesGroup!.Add(row);
            }
            _singleSplitChaptersRow!.Active = _controller.PreviousDownloadOptions.SplitChapters;
            _singleExportDescriptionRow!.Active = _controller.PreviousDownloadOptions.ExportDescription;
            _singlePostProcessorArgumentRow!.SetModel(_controller.AvailablePostProcessorArguments);
            _singleStartTimeRow!.Text_ = _discoveryContext.Items[0].StartTime;
            _singleEndTimeRow!.Text_ = _discoveryContext.Items[0].EndTime;
            if(_downloadImmediatelyRow.Active)
            {
                await DownloadSingleAsync();
            }
        }
        else
        {
            _navigationView.PushByTag("playlist");
            if (_downloadImmediatelyRow.Active)
            {
                await DownloadPlaylistAsync();
            }
        }
    }

    private async Task DownloadSingleAsync()
    {
        await _controller.AddSingleDownloadAsync(_discoveryContext!,
            _singleSaveFilenameRow!.Text_ ?? string.Empty,
            _singleSaveFolderRow!.Subtitle ?? string.Empty,
            _discoveryContext!.FileTypes[(int)_singleFileTypeRow!.Selected],
            _discoveryContext!.VideoFormats[(int)_singleVideoFormatRow!.Selected],
            _discoveryContext!.AudioFormats[(int)_singleAudioFormatRow!.Selected],
            _discoveryContext!.SubtitleLanguages.Where((x, i) => _singleSubtitleRows[i].ActivatableWidget is Gtk.CheckButton chk && chk.Active),
            _singleSplitChaptersRow!.Active,
            _singleExportDescriptionRow!.Active,
            _singleExcludeFromHistoryRow!.Active,
            _controller.AvailablePostProcessorArguments[(int)_singlePostProcessorArgumentRow!.Selected],
            _singleStartTimeRow!.Text_ ?? string.Empty,
            _singleEndTimeRow!.Text_ ?? string.Empty);
        Hide();
    }

    private async Task DownloadPlaylistAsync()
    {
        Hide();
    }

    private async void SingleRevertToTitleButton_OnClicked(Gtk.Button sender, EventArgs e) => _singleSaveFilenameRow!.Text_ = _discoveryContext!.Items[0].Label;

    private async void SingleSelectSaveFolderButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        var fileDialog = Gtk.FileDialog.New();
        fileDialog.Title = _controller.Translator._("Select Save Folder");
        try
        {
            var res = await fileDialog.SelectFolderAsync(_parent);
            if (res is not null)
            {
                _singleSaveFolderRow!.Subtitle = res.GetPath();
            }
        }
        catch { }
    }

    private void SingleFileTypeRow_OnNotify(GObject.Object sender, NotifySignalArgs e)
    {
        if (e.Pspec.GetName() == "selected-item")
        {
            var selectedFileType = _discoveryContext!.FileTypes[(int)_singleFileTypeRow!.Selected];
            _singleVideoFormatRow!.Selected = (uint)_discoveryContext.VideoFormats.IndexOf(_discoveryContext.VideoFormats.First(x => x.Value.Id == _controller.PreviousDownloadOptions.VideoFormatIds[selectedFileType.Value]));
            _singleAudioFormatRow!.Selected = (uint)_discoveryContext.AudioFormats.IndexOf(_discoveryContext.AudioFormats.First(x => x.Value.Id == _controller.PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value]));
        }
    }

    private async void SingleSelectAllSubtitlesRow_OnActivated(Adw.ButtonRow sender, EventArgs e)
    {
        foreach(var row in _singleSubtitleRows)
        {
            if (row.ActivatableWidget is Gtk.CheckButton chk)
            {
                chk.Active = true;
            }
        }
    }

    private async void SingleDeselectAllSubtitlesRow_OnActivated(Adw.ButtonRow sender, EventArgs e)
    {
        foreach(var row in _singleSubtitleRows)
        {
            if (row.ActivatableWidget is Gtk.CheckButton chk)
            {
                chk.Active = false;
            }
        }
    }
}
