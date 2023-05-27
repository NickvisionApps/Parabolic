#addin nuget:?package=Cake.FileHelpers&version=6.1.3

const string appId = "org.nickvision.tubeconverter";
const string projectName = "NickvisionTubeConverter";
const string shortName = "tubeconverter";

var sep = System.IO.Path.DirectorySeparatorChar;

var target = Argument("target", "Run");
var configuration = Argument("configuration", "Debug");
var ui = Argument("ui", EnvironmentVariable("NICK_UI", ""));
var projectSuffix = "";

//////////////////////////////////////////////////////////////////////
// TASKS
//////////////////////////////////////////////////////////////////////

Task("Clean")
    .WithCriteria(c => HasArgument("rebuild"))
    .Does(() =>
{
    CleanDirectory($".{sep}{projectName}.{projectSuffix}{sep}bin{sep}{configuration}");
});

Task("Build")
    .IsDependentOn("Clean")
    .Does(() =>
{
    DotNetBuild($".{sep}{projectName}.{projectSuffix}{sep}{projectName}.{projectSuffix}.csproj", new DotNetBuildSettings
    {
        Configuration = configuration
    });
});

Task("Run")
    .IsDependentOn("Build")
    .Does(() =>
{
    DotNetRun($".{sep}{projectName}.{projectSuffix}{sep}{projectName}.{projectSuffix}.csproj", new DotNetRunSettings
    {
        Configuration = configuration,
        NoBuild = true
    });
});

Task("Publish")
    .Does(() =>
{
    var selfContained = Argument("self-contained", false) || HasArgument("self-contained") || HasArgument("sc");
    var runtime = Argument("runtime", "");
    if (string.IsNullOrEmpty(runtime))
    {
        runtime = "linux-";
        runtime += System.Runtime.InteropServices.RuntimeInformation.ProcessArchitecture.ToString().ToLower();
    }
    var outDir = EnvironmentVariable("NICK_BUILDDIR", "_nickbuild");
    CleanDirectory(outDir);
    var prefix = Argument("prefix", "/usr");
    var libDir = string.IsNullOrEmpty(prefix) ? "lib" : $"{prefix}{sep}lib";
    var publishDir = $"{outDir}{libDir}{sep}{appId}";
    var exitCode = 0;
    Information($"Publishing {projectName}.{projectSuffix} ({runtime})...");
    DotNetPublish($".{sep}{projectName}.{projectSuffix}{sep}{projectName}.{projectSuffix}.csproj", new DotNetPublishSettings
    {
        Configuration = "Release",
        SelfContained = selfContained,
        OutputDirectory = publishDir,
        Sources = Argument("sources", "").Split(" "),
        Runtime = runtime,
        HandleExitCode = code => {
            exitCode = code;
            return false;
        }
    });
    if (exitCode != 0)
    {
        throw new Exception($"Publishing failed with exit code {exitCode}.");
    }

    FinishPublishLinux(outDir, prefix, libDir, selfContained);

    if (projectSuffix == "GNOME")
    {
        PostPublishGNOME(outDir, prefix, libDir);
    }
});

Task("Install")
    .Does(() =>
{
    var buildDir = EnvironmentVariable("NICK_BUILDDIR", "_nickbuild");
    var destDir = Argument("destdir", "/");
    CopyDirectory(buildDir, destDir);
});

Task("FlatpakSourcesGen")
    .Does(() =>
{
    StartProcess("flatpak-dotnet-generator.py", new ProcessSettings{
        Arguments = $"{projectName}.{projectSuffix}{sep}nuget-sources.json {projectName}.{projectSuffix}{sep}{projectName}.{projectSuffix}.csproj"
    });
});

//////////////////////////////////////////////////////////////////////
// FUNCTIONS
//////////////////////////////////////////////////////////////////////

private void FinishPublishLinux(string outDir, string prefix, string libDir, bool selfContained)
{
    var binDir = string.IsNullOrEmpty(prefix) ? $"{outDir}/bin" : $"{outDir}{prefix}/bin";
    CreateDirectory(binDir);
    CopyFileToDirectory($"./{projectName}.Shared/{appId}.in", binDir);
    ReplaceTextInFiles($"{binDir}/{appId}.in", "@EXEC@", selfContained ? $"{libDir}/{appId}/{projectName}.{projectSuffix}" : $"dotnet {libDir}/{appId}/{projectName}.{projectSuffix}.dll");
    MoveFile($"{binDir}/{appId}.in", $"{binDir}/{appId}");
    StartProcess("chmod", new ProcessSettings{
        Arguments = $"+x {binDir}/{appId}"
    });

    var shareDir = string.IsNullOrEmpty(prefix) ? $"{outDir}/share" : $"{outDir}{prefix}/share";

    var iconsScalableDir = $"{shareDir}{sep}icons{sep}hicolor{sep}scalable{sep}apps";
    CreateDirectory(iconsScalableDir);
    CopyFileToDirectory($".{sep}{projectName}.Shared{sep}Resources{sep}{appId}.svg", iconsScalableDir);
    CopyFileToDirectory($".{sep}{projectName}.Shared{sep}Resources{sep}{appId}-devel.svg", iconsScalableDir);
    var iconsSymbolicDir = $"{shareDir}{sep}icons{sep}hicolor{sep}symbolic{sep}apps";
    CreateDirectory(iconsSymbolicDir);
    CopyFileToDirectory($".{sep}{projectName}.Shared{sep}Resources{sep}{appId}-symbolic.svg", iconsSymbolicDir);

    var desktopDir = $"{shareDir}/applications";
    CreateDirectory(desktopDir);
    CopyFileToDirectory($"./{projectName}.Shared/{appId}.desktop.in", desktopDir);
    ReplaceTextInFiles($"{desktopDir}/{appId}.desktop.in", "@EXEC@", $"{prefix}/bin/{appId}");
    MoveFile($"{desktopDir}/{appId}.desktop.in", $"{desktopDir}/{appId}.desktop");

    var metainfoDir = $"{shareDir}/metainfo";
    CreateDirectory(metainfoDir);
    CopyFileToDirectory($"./{projectName}.Shared/{appId}.metainfo.xml.in", metainfoDir);
    ReplaceTextInFiles($"{metainfoDir}/{appId}.metainfo.xml.in", "@PROJECT@", $"{projectName}.{projectSuffix}");
    MoveFile($"{metainfoDir}/{appId}.metainfo.xml.in", $"{metainfoDir}/{appId}.metainfo.xml");
}

private void PostPublishGNOME(string outDir, string prefix, string libDir)
{
    var shareDir = string.IsNullOrEmpty(prefix) ? $"{outDir}{sep}share" : $"{outDir}{prefix}{sep}share";

    CreateDirectory($"{shareDir}{sep}{appId}");
    MoveFileToDirectory($"{outDir}{libDir}{sep}{appId}{sep}{appId}.gresource", $"{shareDir}{sep}{appId}");

    var servicesDir = $"{shareDir}{sep}dbus-1{sep}services";
    CreateDirectory(servicesDir);
    CopyFileToDirectory($".{sep}{projectName}.GNOME{sep}{appId}.service.in", servicesDir);
    ReplaceTextInFiles($"{servicesDir}{sep}{appId}.service.in", "@PREFIX@", $"{prefix}");
    MoveFile($"{servicesDir}{sep}{appId}.service.in", $"{servicesDir}{sep}{appId}.service");

    FileAppendLines($"{shareDir}{sep}applications{sep}{appId}.desktop" , new string[] { "\nDBusActivatable=true" });
}

//////////////////////////////////////////////////////////////////////
// EXECUTION
//////////////////////////////////////////////////////////////////////

if (target == "Install")
{
    RunTarget(target);
    return;
}

if (string.IsNullOrEmpty(ui))
{
    throw new Exception("UI is not set. Use --ui option or NICK_UI environment variable.");
}
projectSuffix = ui.ToLower() switch
{
    "gnome" => "GNOME",
    _ => ""
};
if (string.IsNullOrEmpty(projectSuffix))
{
    throw new Exception("Unknown UI. Possible values: gnome.");
}

RunTarget(target);
