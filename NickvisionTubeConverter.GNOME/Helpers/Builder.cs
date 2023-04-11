using NickvisionTubeConverter.Shared.Helpers;
using System;
using System.IO;
using System.Reflection;
using System.Xml;

namespace NickvisionTubeConverter.GNOME.Helpers;

public class Builder
{
    /// <summary>
    /// Creates a Gtk.Builder from an embedded resource and replaces all translatable strings with the localized version
    /// </summary>
    /// <param name="name">The name of the embedded resource</param>
    /// <param name="localizer">The localizer</param>
    /// <param name="translatableTransformer">Optional closure to override behavior of transforming localization keys to the translated text</param>
    /// <returns>Gtk.Builder</returns>
    public static Gtk.Builder FromFile(string name, Localizer localizer, Func<string, string>? translatableTransformer = null)
    {
        using var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name);
        using var reader = new StreamReader(stream!);
        var uiContents = reader.ReadToEnd();
        var xml = new XmlDocument();
        xml.LoadXml(uiContents);
        var elements = xml.GetElementsByTagName("*");
        foreach (XmlElement element in elements)
        {
            if (element.HasAttribute("translatable"))
            {
                element.RemoveAttribute("translatable");
                element.InnerText = translatableTransformer == null ? localizer[element.InnerText] : translatableTransformer(element.InnerText);
            }
        }
        return Gtk.Builder.NewFromString(xml.OuterXml, -1);
    }
}