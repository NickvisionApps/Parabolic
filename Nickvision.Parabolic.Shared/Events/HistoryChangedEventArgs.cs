using Nickvision.Parabolic.Shared.Models;
using System;

namespace Nickvision.Parabolic.Shared.Events;

public class HistoryChangedEventArgs : EventArgs
{
    public ModificationType Modification { get; }

    public HistoryChangedEventArgs(ModificationType modification)
    {
        Modification = modification;
    }
}
