using System;
using System.Collections.Generic;
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
            return result;
        }

        public IEnumerable<string> Split(Func<char, bool> controller)
        {
            var nextPiece = 0;
            for (int c = 0; c < s.Length; c++)
            {
                if (controller(s[c]))
                {
                    yield return s.Substring(nextPiece, c - nextPiece);
                    nextPiece = c + 1;
                }
            }
            if (nextPiece < s.Length)
            {
                yield return s.Substring(nextPiece);
            }
        }

        public IEnumerable<string> SplitCommandLine()
        {
            var inQuotes = false;
            return s.Split(c =>
            {
                if (c == '"')
                {
                    inQuotes = !inQuotes;
                }
                return !inQuotes && char.IsWhiteSpace(c);
            }).Select(arg => arg.Trim().TrimMatchingQuotes('"')).Where(arg => !string.IsNullOrEmpty(arg));
        }

        public string TrimMatchingQuotes(char quote)
        {
            if (s.Length >= 2 && s[0] == quote && s[^1] == quote)
            {
                return s.Substring(1, s.Length - 2);
            }
            return s;
        }
    }
}
