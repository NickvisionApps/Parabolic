using System.Collections.Generic;

namespace Nickvision.Parabolic.GNOME.Helpers;

public static class ListExtensions
{
    extension(IReadOnlyList<Adw.ActionRow> list)
    {
        public void DeselectAll()
        {
            foreach (var row in list)
            {
                if (row.ActivatableWidget is Gtk.CheckButton chk)
                {
                    chk.Active = false;
                }
            }
        }

        public void SelectAll()
        {
            foreach (var row in list)
            {
                if (row.ActivatableWidget is Gtk.CheckButton chk)
                {
                    chk.Active = true;
                }
            }
        }
    }
}
