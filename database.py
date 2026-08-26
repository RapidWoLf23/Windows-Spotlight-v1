import sqlite3


DATABASE_NAME = "reminders.db"


def create_database():
    connection = sqlite3.connect(DATABASE_NAME)

    cursor = connection.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS reminders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task TEXT NOT NULL,
            reminder_time TEXT NOT NULL,
            completed INTEGER DEFAULT 0
        )
    """)

    connection.commit()
    connection.close()


def add_reminder(task, reminder_time):
    connection = sqlite3.connect(DATABASE_NAME)

    cursor = connection.cursor()

    cursor.execute(
        """
        INSERT INTO reminders (task, reminder_time)
        VALUES (?, ?)
        """,
        (task, reminder_time)
    )

    connection.commit()
    connection.close()


def get_reminders():
    connection = sqlite3.connect(DATABASE_NAME)

    cursor = connection.cursor()

    cursor.execute("""
        SELECT id, task, reminder_time, completed
        FROM reminders
        ORDER BY reminder_time
    """)

    reminders = cursor.fetchall()

    connection.close()

    return reminders