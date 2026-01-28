using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Parabolic.GNOME.Helpers;
using Nickvision.Parabolic.Shared.Controllers;
using System.Collections.Generic;

namespace Nickvision.Parabolic.GNOME.Views;

public class HistoryDialog : Adw.PreferencesDialog
{
    private readonly HistoryViewController _controller;
    private readonly Gtk.Builder _builder;
    private readonly List<Adw.ActionRow> _historyRows;

    public HistoryDialog(HistoryViewController controller, Gtk.Window parent) : this(controller, parent, Gtk.Builder.NewFromBlueprint("HistoryDialog", controller.Translator))
    {

    }

    public HistoryDialog(HistoryViewController controller, Gtk.Window parent, Gtk.Builder builder) : base(new Adw.Internal.PreferencesDialogHandle(builder.GetPointer("root"), false))
    {
        _controller = controller;
        _builder = builder;
        _historyRows = new List<Adw.ActionRow>();
        _builder.Connect(this);
    }
}
