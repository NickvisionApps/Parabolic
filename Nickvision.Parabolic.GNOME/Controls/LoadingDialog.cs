using Nickvision.Desktop.Globalization;
using Nickvision.Desktop.GNOME.Helpers;

namespace Nickvision.Parabolic.GNOME.Controls;

public class LoadingDialog : Adw.Dialog
{
    private readonly Gtk.Builder _builder;

    public LoadingDialog(ITranslationService translator) : this(Gtk.Builder.NewFromBlueprint("LoadingDialog", translator))
    {

    }

    private LoadingDialog(Gtk.Builder builder) : base(new Adw.Internal.DialogHandle(builder.GetPointer("root"), false))
    {
        _builder = builder;
        _builder.Connect(this);
    }
}
