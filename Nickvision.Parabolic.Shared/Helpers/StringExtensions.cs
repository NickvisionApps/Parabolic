using System;
using System.IO;
using System.Linq;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class StringExtensions
{
    extension(string s)
    {
        public string SanitizeForFilename(bool includeWindowsCharacters)
        {
            var chars = Path.GetInvalidFileNameChars().ToHashSet();
            chars.Add('/');
            if (includeWindowsCharacters || OperatingSystem.IsWindows())
            {
                chars.Add('<');
                chars.Add('>');
                chars.Add(':');
                chars.Add('"');
                chars.Add('\\');
                chars.Add('|');
                chars.Add('?');
                chars.Add('*');
            }
            var result = s;
            foreach (var c in chars)
            {
                result = result.Replace(c, '_');
            }
            return s;
        }
    }
}
