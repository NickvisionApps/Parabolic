using Microsoft.Extensions.Hosting;
using Nickvision.Desktop.GNOME.Helpers;
using Nickvision.Parabolic.GNOME.Helpers;
using Nickvision.Parabolic.GNOME.Views;
using Nickvision.Parabolic.Shared.Helpers;
using System.Diagnostics.CodeAnalysis;
using System.Threading.Tasks;

namespace Nickvision.Parabolic.GNOME;

public class Program
{
    [RequiresDynamicCode("Calls ConfigureAdw<T>() which may use dynamic code generation.")]
    public static async Task Main(string[] args)
    {
        var newArgs = new string[args.Length + 1];
        newArgs[0] = "org.nickvision.tubeconverter";
        args.CopyTo(newArgs, 1);
        var builder = Host.CreateApplicationBuilder(args);
        builder.ConfigureParabolic(newArgs);
        builder.ConfigureAdw<MainWindow>(true);
        builder.Services.AddControls();
        var app = builder.Build();
        await app.RunAsync();
    }
}