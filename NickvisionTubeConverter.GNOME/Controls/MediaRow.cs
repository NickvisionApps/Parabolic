using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Text.RegularExpressions;
using static NickvisionTubeConverter.Shared.Helpers.Gettext;

namespace NickvisionTubeConverter.GNOME.Controls;

public class MediaRow : Adw.EntryRow
{
    private MediaInfo _mediaInfo;
    private string _numberString;
    private readonly Gtk.EventControllerKey _titleKeyController;
    private readonly bool _limitChars;

    [Gtk.Connect] private readonly Gtk.CheckButton _downloadCheck;
    [Gtk.Connect] private readonly Gtk.Button _undoButton;

    public event EventHandler<EventArgs> OnSelectionChanged;

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
            SetText(_numberString + _mediaInfo.OriginalTitle);
        };
    }

    public MediaRow(MediaInfo mediaInfo, bool limitChars) : this(Builder.FromFile("media_row.ui"), mediaInfo, limitChars)
    {

    }

    public void UpdateTitle(bool numbered)
    {
        SetText(_mediaInfo.Title);
        _numberString = numbered ? $"{_mediaInfo.PlaylistPosition} - " : "";
    }

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
