using Nickvision.Avalonia.Extensions;
using Nickvision.Avalonia.Models;
using Nickvision.Avalonia.MVVM;
using Nickvision.Avalonia.MVVM.Services;
using NickvisionTubeConverter.Models;
using System;
using System.Collections.ObjectModel;

namespace NickvisionTubeConverter.ViewModels;

public class SettingsDialogViewModel : ViewModelBase
{
    private readonly ServiceCollection _serviceCollection;
    private readonly Configuration _configuration;
    private bool _isLightTheme;
    private bool _isDarkTheme;

    public ObservableCollection<AccentColor> AccentColors { get; init; }
    public ObservableCollection<int> ListMaxNumberOfActiveDownloads { get; init; }

    public SettingsDialogViewModel(ServiceCollection serviceCollection)
    {
        Title = "Settings";
        _serviceCollection = serviceCollection;
        _configuration = Configuration.Load();
        AccentColors = EnumExtensions.GetObservableCollection<AccentColor>();
        ListMaxNumberOfActiveDownloads = new ObservableCollection<int>();
        for (int i = 1; i < 11; i++)
        {
            ListMaxNumberOfActiveDownloads.Add(i);
        }
        if (_configuration.Theme == Theme.Light)
        {
            IsLightTheme = true;
        }
        else
        {
            IsDarkTheme = true;
        }
    }

    public bool IsLightTheme
    {
        get => _isLightTheme;

        set
        {
            SetProperty(ref _isLightTheme, value);
            if (value)
            {
                _configuration.Theme = Theme.Light;
                IsDarkTheme = false;
                _serviceCollection.GetService<IThemeService>()?.ChangeTheme(Theme.Light);
                if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                {
                    _serviceCollection.GetService<IThemeService>()?.ForceWin32WindowToTheme();
                }
                _configuration.Save();
            }
        }
    }

    public bool IsDarkTheme
    {
        get => _isDarkTheme;

        set
        {
            SetProperty(ref _isDarkTheme, value);
            if (value)
            {
                _configuration.Theme = Theme.Dark;
                IsLightTheme = false;
                _serviceCollection.GetService<IThemeService>()?.ChangeTheme(Theme.Dark);
                if (Environment.OSVersion.Platform == PlatformID.Win32NT)
                {
                    _serviceCollection.GetService<IThemeService>()?.ForceWin32WindowToTheme();
                }
                _configuration.Save();
            }
        }
    }

    public AccentColor AccentColor
    {
        get => _configuration.AccentColor;

        set
        {
            _configuration.AccentColor = value;
            _serviceCollection.GetService<IThemeService>()?.ChangeAccentColor(value);
            _configuration.Save();
            OnPropertyChanged();
        }
    }

    public int MaxNumberOfActiveDownloads
    {
        get => _configuration.MaxNumberOfActiveDownloads;

        set
        {
            _configuration.MaxNumberOfActiveDownloads = value;
            _configuration.Save();
            OnPropertyChanged();
        }
    }
}
