using Microsoft.Extensions.Hosting;
using Nickvision.Desktop.WinUI.Helpers;
using Nickvision.Parabolic.Shared.Helpers;
using Nickvision.Parabolic.WinUI.Helpers;
using System;
using System.Diagnostics.CodeAnalysis;
using System.Runtime.InteropServices;

namespace Nickvision.Parabolic.WinUI;

public static partial class Program
{
    [LibraryImport("Microsoft.ui.xaml.dll", EntryPoint = "XamlCheckProcessRequirements")]
    private static partial void XamlCheckProcessRequirements();

    [STAThread]
    [RequiresDynamicCode("Calls ConfigureWinUI<T>() which may use dynamic code generation.")]
    private static void Main(string[] args)
    {
        XamlCheckProcessRequirements();
        var builder = Host.CreateApplicationBuilder(args);
        builder.ConfigureParabolic(args);
        builder.ConfigureWinUI<App>();
        builder.Services.AddControls();
        var app = builder.Build();
        app.Run();
    }
}
