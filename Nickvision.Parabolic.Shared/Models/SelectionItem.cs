using System;

namespace Nickvision.Parabolic.Shared.Models;

public class SelectionItem<T>
{
    public T Value { get; }
    public string Label { get; }
    public bool ShouldSelect { get; set; }

    public SelectionItem(T value, string label, bool shouldSelect)
    {
        Value = value;
        Label = label;
        ShouldSelect = shouldSelect;
    }
}