import sys

from PySide6.QtWidgets import (
    QApplication,
    QLineEdit,
    QWidget,
    QVBoxLayout,
    QLabel
)

from PySide6.QtCore import QTimer

from database import (
    create_database,
    add_reminder
)

from reminder_parser import (
    parse_reminder
)

from scheduler import (
    check_reminders
)


class Spotlight(QWidget):

    def __init__(self):

        super().__init__()

        self.setWindowTitle(
            "Windows Spotlight"
        )

        self.setFixedSize(
            700,
            180
        )

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

        self.setLayout(
            layout
        )

        # -----------------------------
        # Reminder scheduler
        # -----------------------------

        self.scheduler_timer = QTimer()

        self.scheduler_timer.timeout.connect(
            check_reminders
        )

        # Check every second
        self.scheduler_timer.start(
            1000
        )

    def process_command(self):

        command = self.search_box.text().strip()

        if not command:
            return

        reminder = parse_reminder(
            command
        )

        task = reminder["task"]

        reminder_time = reminder[
            "reminder_time"
        ]

        reminder_id = add_reminder(
            task,
            reminder_time.isoformat()
        )

        formatted_date = (
            reminder_time.strftime(
                "%d %B %Y"
            )
        )

        formatted_time = (
            reminder_time.strftime(
                "%I:%M %p"
            )
        )

        self.result.setText(
            f"🔔 Reminder saved!\n\n"
            f"Task: {task}\n"
            f"Date: {formatted_date}\n"
            f"Time: {formatted_time}\n"
            f"ID: {reminder_id}"
        )

        self.search_box.clear()


# -----------------------------
# Initialize database
# -----------------------------

create_database()


# -----------------------------
# Start application
# -----------------------------

app = QApplication(sys.argv)

window = Spotlight()

window.show()

sys.exit(app.exec())