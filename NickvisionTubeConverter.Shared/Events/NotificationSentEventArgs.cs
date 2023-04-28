using System;

namespace NickvisionTubeConverter.Shared.Events;

/// <summary>
/// Event args for when a notification is sent
/// </summary>
public class NotificationSentEventArgs : EventArgs
{
    /// <summary>
    /// The message of the notification
    /// </summary>
    public string Message { get; set; }
    /// <summary>
    /// Additional action for notification
    /// </summary>
    public string Action { get; set; }
    /// <summary>
    /// Parameter for additional action
    /// </summary>
    public string ActionParam { get; set; }
    /// <summary>
    /// The severity of the notification
    /// </summary>
    public NotificationSeverity Severity { get; set; }

    /// <summary>
    /// Constructs a NotificationSentEventArgs
    /// </summary>
    /// <param name="message">The message of the notification</param>
    /// <param name="severity">The severity of the notification</param>
    /// <param name="action">Additional action for notification</param>
    /// <param name="actionParam">Parameter for additional action</param>
    public NotificationSentEventArgs(string message, NotificationSeverity severity, string action = "", string actionParam = "")
    {
        Message = message;
        Severity = severity;
        Action = action;
        ActionParam = actionParam;
    }
}
