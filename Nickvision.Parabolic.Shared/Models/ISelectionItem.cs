namespace Nickvision.Parabolic.Shared.Models;

public interface ISelectionItem
{
    string Label { get; }
    bool ShouldSelect { get; }
}
