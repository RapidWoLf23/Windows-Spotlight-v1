import re
from datetime import datetime, timedelta


WEEKDAYS = {
    "monday": 0,
    "tuesday": 1,
    "wednesday": 2,
    "thursday": 3,
    "friday": 4,
    "saturday": 5,
    "sunday": 6,
}


def get_next_weekday(current_date, target_weekday):

    days_ahead = (
        target_weekday - current_date.weekday()
    ) % 7

    if days_ahead == 0:
        days_ahead = 7

    return current_date + timedelta(
        days=days_ahead
    )


def parse_time(command):

    time_match = re.search(
        r"\b(?:at\s*)?(\d{1,2})(?::(\d{2}))?\s*(am|pm)\b",
        command,
        re.IGNORECASE
    )

    if time_match:

        hour = int(
            time_match.group(1)
        )

        minute = int(
            time_match.group(2) or 0
        )

        period = (
            time_match.group(3)
            .lower()
        )

        if period == "pm" and hour != 12:
            hour += 12

        if period == "am" and hour == 12:
            hour = 0

        return hour, minute

    if "morning" in command:
        return 9, 0

    if "afternoon" in command:
        return 15, 0

    if "evening" in command:
        return 18, 0

    if "night" in command:
        return 21, 0

    return 9, 0


def parse_reminder(command):

    command = command.lower().strip()

    now = datetime.now()

    # --------------------------------
    # Relative time
    # --------------------------------

    relative_match = re.search(
        r"\bin\s+(\d+)\s+(minute|minutes|hour|hours)\b",
        command
    )

    if relative_match:

        amount = int(
            relative_match.group(1)
        )

        unit = relative_match.group(2)

        if "minute" in unit:

            reminder_time = (
                now + timedelta(
                    minutes=amount
                )
            )

        else:

            reminder_time = (
                now + timedelta(
                    hours=amount
                )
            )

        task = clean_task(command)

        return {
            "task": task,
            "reminder_time": reminder_time
        }

    # --------------------------------
    # Date
    # --------------------------------

    reminder_date = now

    if "tomorrow" in command:

        reminder_date = (
            now + timedelta(days=1)
        )

    elif "today" in command:

        reminder_date = now

    else:

        for day_name, weekday_number in WEEKDAYS.items():

            if day_name in command:

                reminder_date = get_next_weekday(
                    now,
                    weekday_number
                )

                break

    # --------------------------------
    # Time
    # --------------------------------

    hour, minute = parse_time(
        command
    )

    reminder_time = reminder_date.replace(
        hour=hour,
        minute=minute,
        second=0,
        microsecond=0
    )

    # --------------------------------
    # Task
    # --------------------------------

    task = clean_task(command)

    return {
        "task": task,
        "reminder_time": reminder_time
    }


def clean_task(command):

    task = command

    task = re.sub(
        r"\bremind me\b",
        "",
        task
    )

    task = re.sub(
        r"\bremember\b",
        "",
        task
    )

    task = re.sub(
        r"\btoday\b",
        "",
        task
    )

    task = re.sub(
        r"\btomorrow\b",
        "",
        task
    )

    task = re.sub(
        r"\bnext\s+"
        r"(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\b",
        "",
        task
    )

    task = re.sub(
        r"\b(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\b",
        "",
        task
    )

    task = re.sub(
        r"\bin\s+\d+\s+"
        r"(minute|minutes|hour|hours)\b",
        "",
        task
    )

    task = re.sub(
        r"\b(?:at\s*)?"
        r"\d{1,2}(?::\d{2})?\s*"
        r"(?:am|pm)\b",
        "",
        task
    )

    task = re.sub(
        r"\b(morning|afternoon|evening|night)\b",
        "",
        task
    )

    task = re.sub(
        r"^\s*to\s+",
        "",
        task
    )

    task = re.sub(
        r"\s+",
        " ",
        task
    )

    return task.strip()