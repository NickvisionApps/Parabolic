using GetText;
using System.IO;
using System.Reflection;

namespace NickvisionTubeConverter.Shared.Helpers;

public static class Gettext
{
    private static readonly ICatalog _catalog = new Catalog("tubeconverter", Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
    
    public static string _(string text)
    {
        return _catalog.GetString(text);
    }

    public static string _(string text, params object[] args)
    {
        return _catalog.GetString(text, args);
    }

    public static string _n(string text, string pluralText, long n)
    {
        return _catalog.GetPluralString(text, pluralText, n);
    }

    public static string _n(string text, string pluralText, long n, params object[] args)
    {
        return _catalog.GetPluralString(text, pluralText, n, args);
    }

    public static string _p(string context, string text)
    {
        return _catalog.GetParticularString(context, text);
    }

    public static string _p(string context, string text, params object[] args)
    {
        return _catalog.GetParticularString(context, text, args);
    }

    public static string _pn(string context, string text, string pluralText, long n)
    {
        return _catalog.GetParticularPluralString(context, text, pluralText, n);
    }

    public static string _pn(string context, string text, string pluralText, long n, params object[] args)
    {
        return _catalog.GetParticularPluralString(context, text, pluralText, n, args);
    }
}