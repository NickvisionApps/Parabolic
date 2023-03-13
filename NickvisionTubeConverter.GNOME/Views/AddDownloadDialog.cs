using NickvisionTubeConverter.GNOME.Helpers;
using NickvisionTubeConverter.Shared.Controllers;
using NickvisionTubeConverter.Shared.Models;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace NickvisionTubeConverter.GNOME.Views;

/// <summary>
/// The AddDownloadDialog for the application
/// </summary>
public partial class AddDownloadDialog : Adw.MessageDialog
{
    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static partial bool gtk_file_chooser_set_current_folder(nint chooser, nint file, nint error);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    public static partial void gtk_file_chooser_set_current_name(nint chooser, string name);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial string g_file_get_path(nint file);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_new();

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_title(nint dialog, string title);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_filters(nint dialog, nint filters);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_initial_name(nint dialog, string name);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_set_initial_folder(nint dialog, nint folder);

    private delegate void GAsyncReadyCallback(nint source, nint res, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial void gtk_file_dialog_save(nint dialog, nint parent, nint cancellable, GAsyncReadyCallback callback, nint user_data);

    [LibraryImport("libadwaita-1.so.0", StringMarshalling = StringMarshalling.Utf8)]
    private static partial nint gtk_file_dialog_save_finish(nint dialog, nint result, nint error);

    private readonly Gtk.Window _parent;
    private readonly AddDownloadDialogController _controller;
    private GAsyncReadyCallback? _saveCallback;

    [Gtk.Connect] private Adw.ViewStack _viewStack;

    private AddDownloadDialog(Gtk.Builder builder, AddDownloadDialogController controller, Gtk.Window parent) : base(builder.GetPointer("_root"), false)
    {
        _parent = parent;
        _controller = controller;
        _saveCallback = null;
        //Dialog Settings
        SetTransientFor(parent);
        AddResponse("cancel", controller.Localizer["Cancel"]);
        SetCloseResponse("cancel");
        AddResponse("ok", controller.Localizer["Download"]);
        SetDefaultResponse("ok");
        SetResponseAppearance("ok", Adw.ResponseAppearance.Suggested);
        OnResponse += (sender, e) => controller.Accepted = e.Response == "ok";
        //Build UI
        builder.Connect(this);
        _viewStack.SetVisibleChildName("pageDownload");
    }

    /// <summary>
    /// Constructs an AddDownloadDialog
    /// </summary>
    public AddDownloadDialog(AddDownloadDialogController controller, Gtk.Window parent) : this(Builder.FromFile("add_download_dialog.ui", controller.Localizer), controller, parent)
    {
    }
}