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


def get_next_weekday(current_date, target_weekday, force_next=False):

    days_ahead = (
        target_weekday - current_date.weekday()
    ) % 7

    if days_ahead == 0:
        days_ahead = 7

    if force_next and days_ahead == 0:
        days_ahead = 7

    return current_date + timedelta(
        days=days_ahead
    )


def parse_time(command):

    # --------------------------------
    # Explicit clock time
    # --------------------------------

    time_match = re.search(
        r"\b(?:at\s*)?"
        r"(\d{1,2})"
        r"(?::(\d{2}))?"
        r"\s*(am|pm)\b",
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

        # Invalid hour
        if hour < 1 or hour > 12:
            return None

        # Invalid minute
        if minute < 0 or minute > 59:
            return None

        # Convert PM
        if period == "pm" and hour != 12:
            hour += 12

        # Convert 12 AM
        elif period == "am" and hour == 12:
            hour = 0

        return hour, minute

    # --------------------------------
    # Day parts
    # --------------------------------

    if "morning" in command:
        return 9, 0

    if "afternoon" in command:
        return 15, 0

    if "evening" in command:
        return 18, 0

    if "night" in command:
        return 21, 0

    # --------------------------------
    # IMPORTANT:
    # Do NOT guess a default time.
    # --------------------------------

    return None


def parse_relative_time(command):

    match = re.search(
        r"\bin\s+(\d+)\s+"
        r"(second|seconds|minute|minutes|hour|hours)\b",
        command,
        re.IGNORECASE
    )

    if not match:
        return None

    amount = int(
        match.group(1)
    )

    unit = match.group(2).lower()

    # --------------------------------
    # Seconds
    # --------------------------------

    if "second" in unit:

        return datetime.now() + timedelta(
            seconds=amount
        )

    # --------------------------------
    # Minutes
    # --------------------------------

    if "minute" in unit:

        return datetime.now() + timedelta(
            minutes=amount
        )

    # --------------------------------
    # Hours
    # --------------------------------

    return datetime.now() + timedelta(
        hours=amount
    )


def parse_date(command, now):

    # --------------------------------
    # Tomorrow
    # --------------------------------

    if "tomorrow" in command:

        return now + timedelta(
            days=1
        )

    # --------------------------------
    # Today
    # --------------------------------

    if "today" in command:

        return now

    # --------------------------------
    # Weekdays
    # --------------------------------

    for day_name, weekday_number in WEEKDAYS.items():

        if day_name not in command:
            continue

        # --------------------------------
        # Next Monday, Tuesday, etc.
        # --------------------------------

        if f"next {day_name}" in command:

            return get_next_weekday(
                now,
                weekday_number,
                force_next=True
            )

        # --------------------------------
        # This Monday, Tuesday, etc.
        # --------------------------------

        if f"this {day_name}" in command:

            days_ahead = (
                weekday_number - now.weekday()
            ) % 7

            return now + timedelta(
                days=days_ahead
            )

        # --------------------------------
        # Just Monday, Tuesday, etc.
        # --------------------------------

        return get_next_weekday(
            now,
            weekday_number
        )

    # --------------------------------
    # No date specified
    # --------------------------------

    return now


def clean_task(command):

    task = command

    # --------------------------------
    # Remove reminder phrases
    # --------------------------------

    task = re.sub(
        r"\bremind me\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\breminder\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\bremember\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Remove relative time
    # --------------------------------

    task = re.sub(
        r"\bin\s+\d+\s+"
        r"(second|seconds|minute|minutes|hour|hours)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Remove dates
    # --------------------------------

    task = re.sub(
        r"\btomorrow\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\btoday\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\bnext\s+"
        r"(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\bthis\s+"
        r"(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    task = re.sub(
        r"\b(monday|tuesday|wednesday|thursday|"
        r"friday|saturday|sunday)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Remove clock times
    # --------------------------------

    task = re.sub(
        r"\b(?:at\s*)?"
        r"\d{1,2}(?::\d{2})?"
        r"\s*(?:am|pm)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Remove day parts
    # --------------------------------

    task = re.sub(
        r"\b(morning|afternoon|evening|night)\b",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Remove common connector words
    # --------------------------------

    task = re.sub(
        r"^\s*(to|for|about)\s+",
        "",
        task,
        flags=re.IGNORECASE
    )

    # --------------------------------
    # Clean spaces
    # --------------------------------

    task = re.sub(
        r"\s+",
        " ",
        task
    )

    # --------------------------------
    # Remove punctuation at edges
    # --------------------------------

    task = task.strip(
        " ,.-"
    )

    return task


def parse_reminder(command):

    command = command.strip()

    if not command:
        return None

    command_lower = command.lower()

    now = datetime.now()

    # --------------------------------
    # 1. Relative reminders
    # --------------------------------

    relative_time = parse_relative_time(
        command_lower
    )

    if relative_time is not None:

        task = clean_task(
            command
        )

        if not task:
            return None

        return {
            "task": task,
            "reminder_time": relative_time
        }

    # --------------------------------
    # 2. Find date
    # --------------------------------

    reminder_date = parse_date(
        command_lower,
        now
    )

    # --------------------------------
    # 3. Find time
    # --------------------------------

    parsed_time = parse_time(
        command_lower
    )

    # --------------------------------
    # IMPORTANT:
    # If there is no time, do NOT guess.
    # Return None so AI fallback can handle it.
    # --------------------------------

    if parsed_time is None:

        return None

    hour, minute = parsed_time

    # --------------------------------
    # 4. Create final reminder time
    # --------------------------------

    reminder_time = reminder_date.replace(
        hour=hour,
        minute=minute,
        second=0,
        microsecond=0
    )

    # --------------------------------
    # 5. Get task
    # --------------------------------

    task = clean_task(
        command
    )

    if not task:
        return None

    # --------------------------------
    # 6. Return reminder
    # --------------------------------

    return {
        "task": task,
        "reminder_time": reminder_time
    }