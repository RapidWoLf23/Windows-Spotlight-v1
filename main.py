import sys
import re
from datetime import datetime, timedelta

from PySide6.QtWidgets import (
    QApplication,
    QLineEdit,
    QWidget,
    QVBoxLayout,
    QLabel
)

from database import create_database, add_reminder


def parse_reminder(command):
    command = command.lower().strip()

    now = datetime.now()

    # -------------------------
    # Find the date
    # -------------------------

    if "tomorrow" in command:
        reminder_date = now + timedelta(days=1)

    elif "today" in command:
        reminder_date = now

    else:
        reminder_date = now

    # -------------------------
    # Find the time
    # -------------------------

    time_match = re.search(
        r"\b(?:at\s*)?(\d{1,2})(?::(\d{2}))?\s*(am|pm)\b",
        command
    )

    if time_match:

        hour = int(time_match.group(1))
        minute = int(time_match.group(2) or 0)
        period = time_match.group(3)

        if period == "pm" and hour != 12:
            hour += 12

        if period == "am" and hour == 12:
            hour = 0

        reminder_time = reminder_date.replace(
            hour=hour,
            minute=minute,
            second=0,
            microsecond=0
        )

    else:

        reminder_time = reminder_date.replace(
            hour=9,
            minute=0,
            second=0,
            microsecond=0
        )

    # -------------------------
    # Find the task
    # -------------------------

    task = command

    task = re.sub(
        r"\bremind me\b",
        "",
        task
    )

    task = re.sub(
        r"\btomorrow\b",
        "",
        task
    )

    task = re.sub(
        r"\btoday\b",
        "",
        task
    )

    task = re.sub(
        r"\b(?:at\s*)?\d{1,2}(?::\d{2})?\s*(?:am|pm)\b",
        "",
        task
    )

    task = re.sub(
        r"\bto\b",
        "",
        task
    )

    task = re.sub(
        r"\s+",
        " ",
        task
    ).strip()

    return task, reminder_time


class Spotlight(QWidget):

    def __init__(self):

        super().__init__()

        self.setWindowTitle("Windows Spotlight")

        self.setFixedSize(700, 150)

        layout = QVBoxLayout()

        self.search_box = QLineEdit()

        self.search_box.setPlaceholderText(
            "What do you want to do?"
        )

        self.result = QLabel("")

        self.search_box.returnPressed.connect(
            self.process_command
        )

        layout.addWidget(
            self.search_box
        )

        layout.addWidget(
            self.result
        )

        self.setLayout(layout)

    def process_command(self):

        command = self.search_box.text()

        if not command:
            return

        task, reminder_time = parse_reminder(
            command
        )

        # Save reminder to database
        add_reminder(
            task,
            reminder_time.isoformat()
        )

        formatted_date = reminder_time.strftime(
            "%d %B %Y"
        )

        formatted_time = reminder_time.strftime(
            "%I:%M %p"
        )

        self.result.setText(
            f"🔔 Reminder saved!\n\n"
            f"Task: {task}\n"
            f"Date: {formatted_date}\n"
            f"Time: {formatted_time}"
        )


# Create database when application starts
create_database()


# Start application
app = QApplication(sys.argv)

window = Spotlight()

window.show()

sys.exit(app.exec())