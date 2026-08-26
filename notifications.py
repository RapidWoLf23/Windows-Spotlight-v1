from winotify import Notification, audio


def send_notification(title, message):

    notification = Notification(
        app_id="Windows Spotlight",
        title=title,
        msg=message
    )

    notification.set_audio(
        audio.Default,
        loop=False
    )

    notification.show()