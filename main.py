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

from text_normalizer import (
    normalize_text
)

from reminder_parser import (
    parse_reminder
)

from ai_parser import (
    ai_parse_reminder
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

        # --------------------------------
        # Reminder scheduler
        # --------------------------------

        self.scheduler_timer = QTimer()

        self.scheduler_timer.timeout.connect(
            check_reminders
        )

        self.scheduler_timer.start(
            1000
        )

    def process_command(self):

        original_command = (
            self.search_box.text().strip()
        )

        if not original_command:
            return

        # --------------------------------
        # Step 1: Normalize human language
        # --------------------------------

        command = normalize_text(
            original_command
        )

        # --------------------------------
        # Step 2: Try normal parser
        # --------------------------------

        reminder = parse_reminder(
            command
        )

        # --------------------------------
        # Step 3: AI fallback
        # --------------------------------

        if reminder is None:

            reminder = ai_parse_reminder(
                command
            )

        # --------------------------------
        # Still not understood
        # --------------------------------

        if reminder is None:

            self.result.setText(
                "I couldn't understand that reminder."
            )

            return

        # --------------------------------
        # Get reminder information
        # --------------------------------

        task = reminder.get(
            "task"
        )

        reminder_time = reminder.get(
            "reminder_time"
        )

        if not task or not reminder_time:

            self.result.setText(
                "I couldn't understand the reminder."
            )

            return

        # --------------------------------
        # Save reminder
        # --------------------------------

        add_reminder(
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
            f"Time: {formatted_time}"
        )

        self.search_box.clear()


# --------------------------------
# Initialize database
# --------------------------------

create_database()


# --------------------------------
# Start application
# --------------------------------

app = QApplication(sys.argv)

window = Spotlight()

window.show()

sys.exit(app.exec())