using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using static Nickvision.Aura.Localization.Gettext;

namespace NickvisionTubeConverter.GNOME.Controls;

/// <summary>
/// A row for a media in the AddDownloadDialog
/// </summary>
public class MediaRow : Adw.EntryRow
{
    private readonly MediaInfo _mediaInfo;
    private string _numberString;
    private readonly Gtk.EventControllerKey _titleKeyController;
    private readonly bool _limitChars;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _undoButton;

    /// <summary>
    /// Occurs when the check to select the download is changed
    /// </summary>
    public event EventHandler<EventArgs>? OnSelectionChanged;

    /// <summary>
    /// Constructs a MediaRow
    /// </summary>
    /// <param name="builder">Gtk.Builder</param>
    /// <param name="mediaInfo">MediaInfo</param>
    /// <param name="limitChars">Whether or not to limit characters to those only supported by Windows</param>
    private MediaRow(Gtk.Builder builder, MediaInfo mediaInfo, bool limitChars) : base(builder.GetPointer("_root"), false)
    {
        _mediaInfo = mediaInfo;
        _numberString = "";
        _limitChars = limitChars;
        //Build UI
        builder.Connect(this);
        SetText(_mediaInfo.Title);
        SetTitle(_mediaInfo.PlaylistPosition > 0 ? _mediaInfo.Url : _("File Name"));
        _downloadCheck.GetParent().SetVisible(_mediaInfo.PlaylistPosition > 0);
        _downloadCheck.SetActive(_mediaInfo.ToDownload);
        _downloadCheck.OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "active")
            {
                _mediaInfo.ToDownload = _downloadCheck.GetActive();
                OnSelectionChanged?.Invoke(this, EventArgs.Empty);
            }
        };
        OnNotify += (sender, e) =>
        {
            if (e.Pspec.GetName() == "text")
            {
                _mediaInfo.Title = GetText();
            }
        };
        _titleKeyController = Gtk.EventControllerKey.New();
        _titleKeyController.SetPropagationPhase(Gtk.PropagationPhase.Capture);
        _titleKeyController.OnKeyPressed += OnKeyPressed;
        AddController(_titleKeyController);
        _undoButton.OnClicked += (sender, e) =>
        {
            SetText($"{_numberString}{_mediaInfo.OriginalTitle}");
        };
    }

    /// <summary>
    /// Constructs a MediaRow
    /// </summary>
    /// <param name="mediaInfo">MediaInfo</param>
    /// <param name="limitChars">Whether or not to limit characters to those only supported by Windows</param>
    public MediaRow(MediaInfo mediaInfo, bool limitChars) : this(Builder.FromFile("media_row.ui"), mediaInfo, limitChars)
    {

    }

    /// <summary>
    /// The active status of the row's check button
    /// </summary>
    public bool Active
    {
        get => _downloadCheck.GetActive();

        set => _downloadCheck.SetActive(value);
    }

    /// <summary>
    /// Updates the title of the row
    /// </summary>
    /// <param name="numbered"></param>
    public void UpdateTitle(bool numbered)
    {
        SetText(_mediaInfo.Title);
        _numberString = numbered ? $"{_mediaInfo.PlaylistPosition} - " : "";
    }

    /// <summary>
    /// Occurs when the row's text is changed
    /// </summary>
    /// <param name="sender">Gtk.EventControllerKey</param>
    /// <param name="e">Gtk.EventControllerKey.KeyPressedSignalArgs</param>
    private bool OnKeyPressed(Gtk.EventControllerKey sender, Gtk.EventControllerKey.KeyPressedSignalArgs e)
    {
        var res = e.Keyval == 0x2f; // '/'
        if (!res && _limitChars)
        {
            res = e.Keyval switch
            {
                0x22 or 0x3c or 0x3e or 0x3a or 0x5c or 0x7c or 0x3f or 0x2a => true, // '"', '<', '>', ':', '\\', '|', '?', '*'
                _ => false
            };
        }
        return res;
    }
}
