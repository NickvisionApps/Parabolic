using Nickvision.Desktop.Application;
using Nickvision.Desktop.Keyring;
using System.ComponentModel;
using WinRT;

namespace Nickvision.Parabolic.WinUI.Helpers;

[GeneratedBindableCustomProperty]
public sealed partial class BindableCredentialSelectionItem : INotifyPropertyChanged
{
    private readonly SelectionItem<Credential> _selectionItem;

    public event PropertyChangedEventHandler? PropertyChanged;

    public string Label => _selectionItem.Label;
    public Credential Value => _selectionItem.Value;

    public BindableCredentialSelectionItem(SelectionItem<Credential> selectionItem)
    {
        _selectionItem = selectionItem;
        _selectionItem.PropertyChanged += (_, e) => PropertyChanged?.Invoke(this, e);
    }

    public bool ShouldSelect
    {
        get => _selectionItem.ShouldSelect;

        set => _selectionItem.ShouldSelect = value;
    }
}
