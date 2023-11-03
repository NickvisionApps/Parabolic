using System;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// Helper methods for working with help documentation
/// </summary>
public static class DocumentationHelpers
{
    /// <summary>
    /// Get URL for given help page
    /// </summary>
    /// <param name="pageName">Help page name</param>
    /// <returns>URL to either yelp or web page</returns>
    public static string GetHelpURL(string pageName)
    {
        if (string.IsNullOrEmpty(Environment.GetEnvironmentVariable("SNAP")) && RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
        {
            return $"help:parabolic/{pageName}";
        }
        var lang = "C";
        if (!CultureInfo.CurrentCulture.Equals(CultureInfo.InvariantCulture) && CultureInfo.CurrentCulture.Name != "en-US")
        {
            using var linguasStream = Assembly.GetCallingAssembly().GetManifestResourceStream("NickvisionTubeConverter.Shared.Docs.po.LINGUAS");
            using var reader = new StreamReader(linguasStream!);
            var linguas = reader.ReadToEnd().Split(Environment.NewLine);
            if (linguas.Contains(CultureInfo.CurrentCulture.Name.Replace("-", "_")))
            {
                lang = CultureInfo.CurrentCulture.Name.Replace("-", "_");
            }
            else
            {
                foreach (var l in linguas)
                {
                    if (l.Contains(CultureInfo.CurrentCulture.TwoLetterISOLanguageName))
                    {
                        lang = l;
                        break;
                    }
                }
            }
        }
        return $"https://htmlpreview.github.io/?https://raw.githubusercontent.com/NickvisionApps/Parabolic/main/NickvisionTubeConverter.Shared/Docs/html/{lang}/{pageName}.html";
    }
}
