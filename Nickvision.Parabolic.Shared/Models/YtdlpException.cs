using System;

namespace Nickvision.Parabolic.Shared.Models;

public class YtdlpException : Exception
{
    public YtdlpException(string message, Exception? e = null) : base(message.Replace("ERROR: ", string.Empty), e)
    {

    }

    public override string ToString() => Message;
}
