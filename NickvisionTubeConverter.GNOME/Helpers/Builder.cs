using NickvisionTubeConverter.Shared.Helpers;
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
    /// <returns>Gtk.Builder</returns>
    public static Gtk.Builder FromFile(string name, Localizer localizer)
    {        
        Gtk.Builder builder;
        using (var stream = Assembly.GetExecutingAssembly().GetManifestResourceStream(name))
        using (var reader = new StreamReader(stream))
        {

            var uiContents = reader.ReadToEnd();
            var xml = new XmlDocument();
            xml.LoadXml(uiContents);
            var elements = xml.GetElementsByTagName("*");
            foreach (XmlElement element in elements)
            {
                if (element.HasAttribute("translatable"))
                {
                    element.RemoveAttribute("translatable");
                    element.InnerText = localizer[element.InnerText];
                }
            }
            builder = Gtk.Builder.NewFromString(xml.OuterXml, -1);
        }
        return builder;
    }
}