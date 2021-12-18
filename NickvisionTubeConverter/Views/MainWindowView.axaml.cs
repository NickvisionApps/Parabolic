using Avalonia.Controls;
using Avalonia.Markup.Xaml;
using FluentAvalonia.UI.Controls;
using Nickvision.Avalonia.MVVM;
using Nickvision.Avalonia.MVVM.Services;
using NickvisionTubeConverter.ViewModels;
using WebViewControl;

namespace NickvisionTubeConverter.Views
{
    public partial class MainWindowView : Window, ICloseable
    {
        public MainWindowView()
        {
            AvaloniaXamlLoader.Load(this);
            var serviceCollection = new ServiceCollection();
            serviceCollection.AddService(new ThemeService(this));
            serviceCollection.AddService(new IOService(this));
            serviceCollection.AddService(new ContentDialogService());
            serviceCollection.AddService(new ProgressDialogService());
            serviceCollection.AddService(new InfoBarService(this.FindControl<InfoBar>("InfoBar")));
            serviceCollection.AddService(new WebViewService(this.FindControl<WebView>("WebView")));
            var viewModel = new MainWindowViewModel(serviceCollection);
            DataContext = viewModel;
        }
    }
}