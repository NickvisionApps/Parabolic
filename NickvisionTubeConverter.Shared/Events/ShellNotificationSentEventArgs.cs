namespace NickvisionTubeConverter.Shared.Events;

/// <summary>
/// Event args for when a notification is sent
/// </summary>
public class ShellNotificationSentEventArgs : NotificationSentEventArgs
{
    /// <summary>
    /// The title of the notification
    /// </summary>
    public string Title { get; set; }

    /// <summary>
    /// Constructs a ShellNotificationSentEventArgs
    /// </summary>
    /// <param name="title">The title of the notification</param>
    /// <param name="message">The message of the notification</param>
    /// <param name="severity">The severity of the notification</param>
    /// <param name="action">Additional action for notification</param>
    /// <param name="actionParam">Parameter for additional action</param>
    public ShellNotificationSentEventArgs(string title, string message, NotificationSeverity severity, string action = "", string actionParam = "") : base(message, severity, action, actionParam)
    {
        Title = title;
    }
}
