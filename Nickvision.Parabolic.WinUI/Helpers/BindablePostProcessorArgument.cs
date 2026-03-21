using Nickvision.Parabolic.Shared.Models;
using System.ComponentModel;
using WinRT;

namespace Nickvision.Parabolic.WinUI.Helpers;


[GeneratedBindableCustomProperty]
public sealed partial class BindablePostProcessorArgument : INotifyPropertyChanged
{
    private readonly PostProcessorArgument _argument;

    public string Name => _argument.Name;
    public string Args => _argument.Args;

    public event PropertyChangedEventHandler? PropertyChanged;

    public BindablePostProcessorArgument(PostProcessorArgument argument)
    {
        _argument = argument;
        _argument.PropertyChanged += (_, e) => PropertyChanged?.Invoke(this, e);
    }
}
