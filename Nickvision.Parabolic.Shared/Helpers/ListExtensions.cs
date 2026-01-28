using Nickvision.Parabolic.Shared.Models;
using System.Collections.Generic;
using System.Linq;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class ListExtensions
{
    extension(List<Format> formats)
    {
        public bool HasFormats(MediaType type) => formats.Any(f => f != Format.NoneVideo && f != Format.NoneAudio && f.Type == type);
    }
}
