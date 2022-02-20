using Avalonia.Markup.Xaml;
using Avalonia.Media;
using Avalonia.Styling;
using FluentAvalonia.UI.Controls;
using System;

namespace NickvisionTubeConverter.Views;

public class SettingsDialogView : ContentDialog, IStyleable
{
    Type IStyleable.StyleKey => typeof(ContentDialog);

    public SettingsDialogView()
    {
        AvaloniaXamlLoader.Load(this);
        if (Environment.OSVersion.Platform == PlatformID.Unix)
        {
            FontFamily = new FontFamily("Cantarell");
        }
    }
}
