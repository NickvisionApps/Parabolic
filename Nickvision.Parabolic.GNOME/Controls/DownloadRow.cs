using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.GNOME.Helpers;

namespace Nickvision.Parabolic.GNOME.Controls;

public class DownloadRow : Gtk.ListBoxRow
{
    private readonly ITranslationService _translator;
    private readonly Gtk.Builder _builder;

    public DownloadRow(ITranslationService translator) : this(translator, Gtk.Builder.NewFromBlueprint("DownloadRow", translator))
    {

    }

    private DownloadRow(ITranslationService translator, Gtk.Builder builder) : base(new Gtk.Internal.ListBoxRowHandle(builder.GetPointer("root"), false))
    {
        _translator = translator;
        _builder = builder;
        _builder.Connect(this);
    }
}
