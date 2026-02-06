using Microsoft.VisualStudio.TestTools.UnitTesting;
using Nickvision.Parabolic.Shared.Controllers;
using System;

namespace Nickvision.Parabolic.Shared.Tests;

[TestClass]
public class MainWindowControllerTests
{
    private static MainWindowController? _controller;

    [ClassInitialize]
    public static void ClassInitialize(TestContext context)
    {
        _controller = new MainWindowController(Array.Empty<string>());
    }

    [ClassCleanup]
    public static void ClassCleanup()
    {
        _controller?.Dispose();
    }

    [TestMethod]
    public void Case001_InitalizeCheck() => Assert.IsNotNull(_controller);
}
