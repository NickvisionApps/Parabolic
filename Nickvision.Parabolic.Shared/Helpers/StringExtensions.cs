using System;
using System.IO;
using System.Linq;

namespace Nickvision.Parabolic.Shared.Helpers;

public static class StringExtensions
{
    extension(string s)
    {
        public int AriaEtaToSeconds()
        {
            var hours = 0;
            var minutes = 0;
            var seconds = 0;
            var index = -1;
            if ((index = s.IndexOf('h')) != -1)
            {
                hours = int.Parse(s.Substring(0, index)) * 3600;
                s = s.Substring(index + 1);
            }
            if ((index = s.IndexOf('m')) != -1)
            {
                minutes = int.Parse(s.Substring(0, index)) * 60;
                s = s.Substring(index + 1);
            }
            if ((index = s.IndexOf('s')) != -1)
            {
                seconds = int.Parse(s.Substring(0, index));
            }
            return hours + minutes + seconds;
        }

        public double AriaSizeToBytes()
        {
            var index = -1;
            if ((index = s.IndexOf("GiB")) != -1)
            {
                return double.Parse(s.Substring(0, index)) * Math.Pow(1024, 3);
            }
            else if ((index = s.IndexOf("MiB")) != -1)
            {
                return double.Parse(s.Substring(0, index)) * Math.Pow(1024, 2);
            }
            else if ((index = s.IndexOf("KiB")) != -1)
            {
                return double.Parse(s.Substring(0, index)) * 1024;
            }
            else if ((index = s.IndexOf("B")) != -1)
            {
                return double.Parse(s.Substring(0, index));
            }
            return 0.0;
        }

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
