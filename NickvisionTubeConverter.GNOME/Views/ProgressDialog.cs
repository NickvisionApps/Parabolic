using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

public class ProgressDialog
{
    private readonly Adw.Window _window;
    private readonly Gtk.Label _lblDescription;
    private readonly Gtk.ProgressBar _progBar;
    private readonly Gtk.Box _mainBox;

    public ProgressDialog(Gtk.Window parent, string description)
    {
        _window = Adw.Window.New();
        //Window Settings
        _window.SetTransientFor(parent);
        _window.SetDefaultSize(400, 60);
        _window.SetModal(true);
        _window.SetResizable(false);
        _window.SetDeletable(false);
        _window.SetDestroyWithParent(false);
        //Description Label
        _lblDescription = Gtk.Label.New(null);
        _lblDescription.SetMarkup("<b>" + description + "</b>");
        _lblDescription.SetHalign(Gtk.Align.Start);
        //Progress Bar
        _progBar = Gtk.ProgressBar.New();
        //Main Box
        _mainBox = Gtk.Box.New(Gtk.Orientation.Vertical, 20);
        _mainBox.SetMarginStart(10);
        _mainBox.SetMarginTop(10);
        _mainBox.SetMarginEnd(10);
        _mainBox.SetMarginBottom(10);
        _mainBox.Append(_lblDescription);
        _mainBox.Append(_progBar);
        _window.SetContent(_mainBox);
    }
}