using System;

namespace Nickvision.Parabolic.Shared.Models;

public class SelectionItem : IComparable<SelectionItem>, IEquatable<SelectionItem>
{
    public int Index { get; }
    public string Label { get; }
    public bool ShouldSelect { get; }

    public SelectionItem(int index, string label, bool shouldSelect)
    {
        Index = index;
        Label = label;
        ShouldSelect = shouldSelect;
    }

    public int CompareTo(SelectionItem? other) => other is null ? 1 : Label.CompareTo(other.Label);

    public override bool Equals(object? obj)
    {
        if (obj is SelectionItem other)
        {
            return Equals(other);
        }
        return false;
    }

    public bool Equals(SelectionItem? other) => other is not null && Index == other.Index && Label == other.Label && ShouldSelect == other.ShouldSelect;

    public override int GetHashCode() => HashCode.Combine(Index, Label, ShouldSelect);

    public static bool operator >(SelectionItem left, SelectionItem right) => left.CompareTo(right) > 0;

    public static bool operator <(SelectionItem left, SelectionItem right) => left.CompareTo(right) < 0;

    public static bool operator >=(SelectionItem left, SelectionItem right) => left.CompareTo(right) >= 0;

    public static bool operator <=(SelectionItem left, SelectionItem right) => left.CompareTo(right) <= 0;

    public static bool operator ==(SelectionItem left, SelectionItem right) => left.Equals(right);

    public static bool operator !=(SelectionItem left, SelectionItem right) => !left.Equals(right);
}
