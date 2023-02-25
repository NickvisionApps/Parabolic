using System;
using System.Globalization;
using System.Resources;

namespace NickvisionTubeConverter.Shared.Helpers;

/// <summary>
/// A helper for getting localized strings
/// </summary>
public class Localizer : IDisposable
{
    private bool _disposed;
    private readonly ResourceManager _resourceManager;
    private readonly ResourceSet _resourceSet;
    private readonly ResourceSet _resourceFallback;

    /// <summary>
    /// Gets a localized non-plural/plural string
    /// </summary>
    /// <param name="name">The name of the string resource</param>
    /// <param name="plural">Whether or not to get a plural string resource</param>
    /// <returns>The localized string</returns>
    public string this[string name, bool plural = false] => plural ? GetPluralString(name) : GetString(name);
    /// <summary>
    /// Gets a localized string by context
    /// </summary>
    /// <param name="name">The name of the string resource</param>
    /// <param name="context">The name of the context</param>
    /// <returns>The localized string with the context</returns>
    public string this[string name, string context] => GetStringWithContext(name, context);

    /// <summary>
    /// Constructs a Localizer
    /// </summary>
    internal Localizer()
    {
        _disposed = false;
        _resourceManager = new ResourceManager("NickvisionTubeConverter.Shared.Resources.Strings", GetType().Assembly);
        _resourceSet = _resourceManager.GetResourceSet(CultureInfo.CurrentCulture, true, true)!;
        _resourceFallback = _resourceManager.GetResourceSet(new CultureInfo("en-US"), true, true)!;
    }

    /// <summary>
    /// Frees resources used by the Account object
    /// </summary>
    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Frees resources used by the Account object
    /// </summary>
    protected virtual void Dispose(bool disposing)
    {
        if (_disposed)
        {
            return;
        }
        if (disposing)
        {
            _resourceSet.Dispose();
            _resourceFallback.Dispose();
        }
        _disposed = true;
    }

    /// <summary>
    /// Gets a localized string
    /// </summary>
    /// <param name="name">The name of the string resource</param>
    /// <returns>The localized string</returns>
    public string GetString(string name) => (string.IsNullOrEmpty(_resourceSet.GetString(name)) ? _resourceFallback.GetString(name) : _resourceSet.GetString(name)) ?? string.Empty;

    /// <summary>
    /// Gets a localized string by context
    /// </summary>
    /// <param name="name">The name of the string resource</param>
    /// <param name="context">The name of the context</param>
    /// <returns>The localized string with the context</returns>
    public string GetStringWithContext(string name, string context) => GetString($"{name}.{context}");

    /// <summary>
    /// Gets a localized plural string
    /// </summary>
    /// <param name="name">The name of the string resource</param>
    /// <returns>The localized plural string</returns>
    public string GetPluralString(string name) => GetString($"{name}.Plural");
}