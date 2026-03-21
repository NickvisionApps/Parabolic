using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.WinUI.Helpers;

public static class MediaSelectionItemListExtensions
{
    extension(IEnumerable<MediaSelectionItem> items)
    {
        public List<BindableMediaSelectionItem> ToBindableMediaSelectionItems() => items.Select(i => new BindableMediaSelectionItem(i)).ToList();
    }
}
