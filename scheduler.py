from datetime import datetime

from database import (
    get_pending_reminders,
    mark_completed
)

from notifications import send_notification


def check_reminders():

    reminders = get_pending_reminders()

    now = datetime.now()

    for reminder in reminders:

        reminder_id = reminder[0]
        task = reminder[1]
        reminder_time_string = reminder[2]

        try:
            reminder_time = datetime.fromisoformat(
                reminder_time_string
            )

        except ValueError:
            continue

        if reminder_time <= now:

            send_notification(
                "Windows Spotlight",
                task
            )

            mark_completed(
                reminder_id
            )