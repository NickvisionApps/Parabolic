using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Desktop.Keyring;
using Nickvision.Parabolic.GNOME.Helpers;
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
    private readonly List<Adw.ActionRow> _singleSubtitlesRows;
    private readonly List<Adw.EntryRow> _playlistItemsRows;
    private readonly List<Gtk.CheckButton> _playlistItemsCheckButtons;
    private readonly List<Adw.ActionRow> _playlistSubtitlesRows;

    [Gtk.Connect("navigationView")]
    private Adw.NavigationView? _navigationView;
    [Gtk.Connect("urlRow")]
    private Adw.EntryRow? _urlRow;
    [Gtk.Connect("selectBatchFileRow")]
    private Adw.ButtonRow? _selectBatchFileRow;
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
    [Gtk.Connect("singleSelectAllSubtitlesButton")]
    private Gtk.Button? _singleSelectAllSubtitlesButton;
    [Gtk.Connect("singleDeselectAllSubtitlesButton")]
    private Gtk.Button? _singleDeselectAllSubtitlesButton;
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
    [Gtk.Connect("playlistViewStack")]
    private Adw.ViewStack? _playlistViewStack;
    [Gtk.Connect("playlistGroup")]
    private Adw.PreferencesGroup? _playlistGroup;
    [Gtk.Connect("playlistSaveFolderRow")]
    private Adw.ActionRow? _playlistSaveFolderRow;
    [Gtk.Connect("playlistSelectSaveFolderButton")]
    private Gtk.Button? _playlistSelectSaveFolderButton;
    [Gtk.Connect("playlistFileTypeRow")]
    private Adw.ComboRow? _playlistFileTypeRow;
    [Gtk.Connect("playlistVideoResolutionRow")]
    private Adw.ComboRow? _playlistVideoResolutionRow;
    [Gtk.Connect("playlistAudioBitrateRow")]
    private Adw.ComboRow? _playlistAudioBitrateRow;
    [Gtk.Connect("playlistItemsPage")]
    private Adw.PreferencesPage? _playlistItemsPage;
    [Gtk.Connect("playlistSelectAllItemsButton")]
    private Gtk.Button? _playlistSelectAllItemsButton;
    [Gtk.Connect("playlistDeselectAllItemsButton")]
    private Gtk.Button? _playlistDeselectAllItemsButton;
    [Gtk.Connect("playlistReverseOrderRow")]
    private Adw.SwitchRow? _playlistReverseOrderRow;
    [Gtk.Connect("playlistNumberTitlesRow")]
    private Adw.SwitchRow? _playlistNumberTitlesRow;
    [Gtk.Connect("playlistItemsGroup")]
    private Adw.PreferencesGroup? _playlistItemsGroup;
    [Gtk.Connect("playlistSubtitlesPage")]
    private Adw.PreferencesPage? _playlistSubtitlesPage;
    [Gtk.Connect("playlistSelectAllSubtitlesButton")]
    private Gtk.Button? _playlistSelectAllSubtitlesButton;
    [Gtk.Connect("playlistDeselectAllSubtitlesButton")]
    private Gtk.Button? _playlistDeselectAllSubtitlesButton;
    [Gtk.Connect("playlistSubtitlesGroup")]
    private Adw.PreferencesGroup? _playlistSubtitlesGroup;
    [Gtk.Connect("playlistExportM3URow")]
    private Adw.SwitchRow? _playlistExportM3URow;
    [Gtk.Connect("playlistSplitChaptersRow")]
    private Adw.SwitchRow? _playlistSplitChaptersRow;
    [Gtk.Connect("playlistExportDescriptionRow")]
    private Adw.SwitchRow? _playlistExportDescriptionRow;
    [Gtk.Connect("playlistExcludeFromHistoryRow")]
    private Adw.SwitchRow? _playlistExcludeFromHistoryRow;
    [Gtk.Connect("playlistPostProcessorArgumentRow")]
    private Adw.ComboRow? _playlistPostProcessorArgumentRow;
    [Gtk.Connect("playlistDownloadHeaderButton")]
    private Gtk.Button? _playlistDownloadHeaderButton;
    [Gtk.Connect("playlistDownloadButton")]
    private Gtk.Button? _playlistDownloadButton;

    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(controller, parent, Gtk.Builder.NewFromBlueprint("AddDownloadDialog", controller.Translator))
    {

    }

    private AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent, Gtk.Builder builder) : base(new Adw.Internal.DialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _parent = parent;
        _builder = builder;
        _cancellationTokenSource = null;
        _discoveryContext = null;
        _singleSubtitlesRows = new List<Adw.ActionRow>();
        _playlistItemsRows = new List<Adw.EntryRow>();
        _playlistItemsCheckButtons = new List<Gtk.CheckButton>();
        _playlistSubtitlesRows = new List<Adw.ActionRow>();
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
        _singleSelectAllSubtitlesButton!.OnClicked += (_, _) => _singleSubtitlesRows.SelectAll();
        _singleDeselectAllSubtitlesButton!.OnClicked += (_, _) => _singleSubtitlesRows.DeselectAll();
        _singleDownloadHeaderButton!.OnClicked += async (sender, e) => await DownloadSingleAsync();
        _singleDownloadButton!.OnClicked += async (sender, e) => await DownloadSingleAsync();
        _playlistSelectSaveFolderButton!.OnClicked += PlaylistSelectSaveFolderButton_OnClicked;
        _playlistSelectAllItemsButton!.OnClicked += (_, _) => _playlistItemsCheckButtons.SelectAll();
        _playlistDeselectAllItemsButton!.OnClicked += (_, _) => _playlistItemsCheckButtons.DeselectAll();
        _playlistSelectAllSubtitlesButton!.OnClicked += (_, _) => _playlistSubtitlesRows.SelectAll();
        _playlistDeselectAllSubtitlesButton!.OnClicked += (_, _) => _playlistSubtitlesRows.DeselectAll();
        _playlistDownloadHeaderButton!.OnClicked += async (sender, e) => await DownloadPlaylistAsync();
        _playlistDownloadButton!.OnClicked += async (sender, e) => await DownloadPlaylistAsync();
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

    private async Task DownloadSingleAsync()
    {
        await _controller.AddSingleDownloadAsync(_discoveryContext!,
            _singleSaveFilenameRow!.Text_ ?? string.Empty,
            _singleSaveFolderRow!.Subtitle ?? string.Empty,
            _discoveryContext!.FileTypes[(int)_singleFileTypeRow!.Selected],
            _discoveryContext!.VideoFormats[(int)_singleVideoFormatRow!.Selected],
            _discoveryContext!.AudioFormats[(int)_singleAudioFormatRow!.Selected],
            _discoveryContext!.SubtitleLanguages.Where((x, i) => _singleSubtitlesRows[i].ActivatableWidget is Gtk.CheckButton chk && chk.Active),
            _singleSplitChaptersRow!.Active,
            _singleExportDescriptionRow!.Active,
            _singleExcludeFromHistoryRow!.Active,
            _controller.AvailablePostProcessorArguments[(int)_singlePostProcessorArgumentRow!.Selected],
            _singleStartTimeRow!.Text_ ?? string.Empty,
            _singleEndTimeRow!.Text_ ?? string.Empty);
        Close();
    }

    private async Task DownloadPlaylistAsync()
    {
        await _controller.AddPlaylistDownloadsAsync(_discoveryContext!,
            _discoveryContext!.Items.Where((x, i) => _playlistItemsCheckButtons[i].Active),
            _playlistSaveFolderRow!.Subtitle ?? string.Empty,
            _discoveryContext!.FileTypes[(int)_playlistFileTypeRow!.Selected],
            _discoveryContext!.VideoResolutions[(int)_playlistVideoResolutionRow!.Selected],
            _discoveryContext!.AudioBitrates[(int)_playlistAudioBitrateRow!.Selected],
            _playlistReverseOrderRow!.Active,
            _playlistNumberTitlesRow!.Active,
            _discoveryContext!.SubtitleLanguages.Where((x, i) => _playlistSubtitlesRows[i].ActivatableWidget is Gtk.CheckButton chk && chk.Active),
            _playlistExportM3URow!.Active,
            _playlistSplitChaptersRow!.Active,
            _playlistExportM3URow!.Active,
            _playlistExcludeFromHistoryRow!.Active,
            _controller.AvailablePostProcessorArguments[(int)_playlistPostProcessorArgumentRow!.Selected]);
        Close();
    }

    private void Dialog_OnClosed(Adw.Dialog sender, EventArgs e)
    {
        _cancellationTokenSource?.Cancel();
        _cancellationTokenSource?.Dispose();
    }

    private void UrlRow_OnChanged(Gtk.Editable sender, EventArgs e) => _discoverUrlButton!.Sensitive = !(_urlRow!.Text_?.StartsWith("//") ?? false) && Uri.TryCreate(_urlRow!.Text_, UriKind.Absolute, out var _);

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
        if (!string.IsNullOrEmpty(_authenticationUsernameRow!.Text_) || !string.IsNullOrEmpty(_authenticationPasswordRow!.Text_))
        {
            credential = new Credential("manual", _authenticationUsernameRow!.Text_ ?? string.Empty, _authenticationPasswordRow!.Text_ ?? string.Empty);
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
        ContentHeight = 550;
        _controller.PreviousDownloadOptions.DownloadImmediately = _downloadImmediatelyRow!.Active;
        if (_discoveryContext.Items.Count == 1)
        {
            ContentWidth = 550;
            _navigationView.PushByTag("single");
            _singleGroup!.Title = _discoveryContext.Title;
            _singleGroup!.Description = GLib.Markup.EscapeText(_discoveryContext.Url.ToString());
            _singleSaveFilenameRow!.Text_ = _discoveryContext.Items[0].Label;
            _singleSaveFolderRow!.Subtitle = _controller.PreviousDownloadOptions.SaveFolder;
            _singleVideoFormatRow!.SetModel(_discoveryContext.VideoFormats, false);
            _singleAudioFormatRow!.SetModel(_discoveryContext.AudioFormats, false);
            _singleFileTypeRow!.SetModel(_discoveryContext.FileTypes);
            if (_discoveryContext.SubtitleLanguages.Count > 0)
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
                _singleSubtitlesRows.Add(row);
                _singleSubtitlesGroup!.Add(row);
            }
            _singleSplitChaptersRow!.Active = _controller.PreviousDownloadOptions.SplitChapters;
            _singleExportDescriptionRow!.Active = _controller.PreviousDownloadOptions.ExportDescription;
            _singlePostProcessorArgumentRow!.SetModel(_controller.AvailablePostProcessorArguments);
            _singleStartTimeRow!.Text_ = _discoveryContext.Items[0].StartTime;
            _singleEndTimeRow!.Text_ = _discoveryContext.Items[0].EndTime;
            if (_downloadImmediatelyRow.Active)
            {
                await DownloadSingleAsync();
            }
        }
        else
        {
            ContentWidth = 600;
            _navigationView.PushByTag("playlist");
            _playlistGroup!.Title = _discoveryContext.Title;
            _playlistGroup!.Description = GLib.Markup.EscapeText(_discoveryContext.Url.ToString());
            _playlistSaveFolderRow!.Subtitle = _controller.PreviousDownloadOptions.SaveFolder;
            _playlistFileTypeRow!.SetModel(_discoveryContext.FileTypes);
            _playlistVideoResolutionRow!.SetModel(_discoveryContext.VideoResolutions);
            _playlistAudioBitrateRow!.SetModel(_discoveryContext.AudioBitrates);
            _playlistViewStack!.GetPage(_playlistItemsPage!).BadgeNumber = (uint)_discoveryContext.Items.Count;
            _playlistReverseOrderRow!.Active = _controller.PreviousDownloadOptions.ReverseDownloadOrder;
            _playlistNumberTitlesRow!.Active = _controller.PreviousDownloadOptions.NumberTitles;
            foreach (var item in _discoveryContext.Items)
            {
                var row = Adw.EntryRow.New();
                row.UseMarkup = false;
                row.Title = item.Label;
                row.Text_ = item.Filename;
                row.OnChanged += (_, _) => item.Filename = row.Text_ ?? string.Empty;
                var chk = Gtk.CheckButton.New();
                chk.Valign = Gtk.Align.Center;
                chk.AddCssClass("selection-mode");
                chk.Active = item.ShouldSelect;
                row.AddPrefix(chk);
                var revertBtn = Gtk.Button.New();
                revertBtn.Valign = Gtk.Align.Center;
                revertBtn.IconName = "edit-undo-symbolic";
                revertBtn.TooltipText = _controller.Translator._("Revert to Title");
                revertBtn.AddCssClass("flat");
                revertBtn.OnClicked += (_, _) => row.Text_ = item.Label;
                row.AddSuffix(revertBtn);
                var startTimeRow = Adw.EntryRow.New();
                startTimeRow.Title = item.StartTimeHeader;
                startTimeRow.Text_ = item.StartTime;
                startTimeRow.OnChanged += (_, _) => item.StartTime = startTimeRow.Text_ ?? string.Empty;
                var endTimeRow = Adw.EntryRow.New();
                endTimeRow.Title = item.EndTimeHeader;
                endTimeRow.Text_ = item.EndTime;
                endTimeRow.OnChanged += (_, _) => item.EndTime = endTimeRow.Text_;
                var preferencesGroup = Adw.PreferencesGroup.New();
                preferencesGroup.Add(startTimeRow);
                preferencesGroup.Add(endTimeRow);
                var preferncesPage = Adw.PreferencesPage.New();
                preferncesPage.WidthRequest = 200;
                preferncesPage.Add(preferencesGroup);
                var popover = Gtk.Popover.New();
                popover.Child = preferncesPage;
                var flyoutBtn = Gtk.MenuButton.New();
                flyoutBtn.Valign = Gtk.Align.Center;
                flyoutBtn.IconName = "wrench-wide-symbolic";
                flyoutBtn.TooltipText = _controller.Translator._("Properties");
                flyoutBtn.AddCssClass("flat");
                flyoutBtn.Popover = popover;
                row.AddSuffix(flyoutBtn);
                _playlistItemsRows.Add(row);
                _playlistItemsCheckButtons.Add(chk);
                _playlistItemsGroup!.Add(row);
            }
            if (_discoveryContext.SubtitleLanguages.Count > 0)
            {
                _playlistViewStack!.GetPage(_playlistSubtitlesPage!).BadgeNumber = (uint)_discoveryContext.SubtitleLanguages.Count;
            }
            else
            {
                var row = Adw.ActionRow.New();
                row.Title = _controller.Translator._("No Subtitles Available");
                _playlistSubtitlesGroup!.Add(row);
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
                _playlistSubtitlesRows.Add(row);
                _playlistSubtitlesGroup!.Add(row);
            }
            _playlistExportM3URow!.Active = _controller.PreviousDownloadOptions.ExportM3U;
            _playlistSplitChaptersRow!.Active = _controller.PreviousDownloadOptions.SplitChapters;
            _playlistExportDescriptionRow!.Active = _controller.PreviousDownloadOptions.ExportDescription;
            _playlistPostProcessorArgumentRow!.SetModel(_controller.AvailablePostProcessorArguments);
            if (_downloadImmediatelyRow.Active)
            {
                await DownloadPlaylistAsync();
            }
        }
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
            var foundVideoFormat = _discoveryContext.VideoFormats.FirstOrDefault(x => x.Value.Id == _controller.PreviousDownloadOptions.VideoFormatIds[selectedFileType.Value]);
            var foundAudioFormat = _discoveryContext.AudioFormats.FirstOrDefault(x => x.Value.Id == _controller.PreviousDownloadOptions.AudioFormatIds[selectedFileType.Value]);
            var foundVideoFormatIndex = foundVideoFormat is null ? -1 : _discoveryContext.VideoFormats.IndexOf(foundVideoFormat);
            var foundAudioFormatIndex = foundAudioFormat is null ? -1 : _discoveryContext.AudioFormats.IndexOf(foundAudioFormat);
            _singleVideoFormatRow!.Selected = foundVideoFormatIndex == -1 ? 0 : (uint)foundVideoFormatIndex;
            _singleAudioFormatRow!.Selected = foundAudioFormatIndex == -1 ? 0 : (uint)foundAudioFormatIndex;
        }
    }

    private async void PlaylistSelectSaveFolderButton_OnClicked(Gtk.Button sender, EventArgs e)
    {
        var fileDialog = Gtk.FileDialog.New();
        fileDialog.Title = _controller.Translator._("Select Save Folder");
        try
        {
            var res = await fileDialog.SelectFolderAsync(_parent);
            if (res is not null)
            {
                _playlistSaveFolderRow!.Subtitle = res.GetPath();
            }
        }
        catch { }
    }
}
