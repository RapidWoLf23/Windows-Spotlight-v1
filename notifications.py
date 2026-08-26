from winotify import Notification


def send_notification(title, message):

    notification = Notification(
        app_id="Windows Spotlight",
        title=title,
        msg=message
    )

    notification.show()
    