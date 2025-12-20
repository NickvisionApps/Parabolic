using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Desktop.Application;
using Nickvision.Desktop.Filesystem;
using Nickvision.Parabolic.Shared.Services;
using System;
using System.IO;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public class RecoveryServiceTests
{
    private static string? _recoveryDirectory;
    private static IRecoveryService? _recoveryService;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        var appInfo = new AppInfo("org.nickvision.tubeconverter.recovery.tests", "Nickvision Parabolic Recovery Tests", "Parabolic Recovery Tests")
        {
            Version = new AppVersion("2025.12.0-next")
        };
        _recoveryDirectory = Path.Combine(UserDirectories.Config, "Nickvision Parabolic Recovery Tests");
        if (Directory.Exists(_recoveryDirectory))
        {
            Directory.Delete(_recoveryDirectory, true);
        }
        _recoveryService = new RecoveryService(appInfo);
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        (_recoveryService as IDisposable)?.Dispose();
        Directory.Delete(_recoveryDirectory!, true);
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_recoveryDirectory);
}
