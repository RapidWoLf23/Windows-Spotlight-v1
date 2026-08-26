import sqlite3
from datetime import datetime


DATABASE_NAME = "reminders.db"


def create_database():

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS reminders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task TEXT NOT NULL,
            reminder_time TEXT NOT NULL,
            completed INTEGER DEFAULT 0,
            created_at TEXT NOT NULL
        )
    """)

    connection.commit()
    connection.close()


def add_reminder(task, reminder_time):

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    created_at = datetime.now().isoformat()

    cursor.execute(
        """
        INSERT INTO reminders
        (
            task,
            reminder_time,
            created_at
        )
        VALUES (?, ?, ?)
        """,
        (
            task,
            reminder_time,
            created_at
        )
    )

    connection.commit()

    reminder_id = cursor.lastrowid

    connection.close()

    return reminder_id


def get_reminders():

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    cursor.execute("""
        SELECT
            id,
            task,
            reminder_time,
            completed,
            created_at
        FROM reminders
        ORDER BY reminder_time
    """)

    reminders = cursor.fetchall()

    connection.close()

    return reminders


def get_pending_reminders():

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    cursor.execute("""
        SELECT
            id,
            task,
            reminder_time
        FROM reminders
        WHERE completed = 0
        ORDER BY reminder_time
    """)

    reminders = cursor.fetchall()

    connection.close()

    return reminders


def mark_completed(reminder_id):

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    cursor.execute(
        """
        UPDATE reminders
        SET completed = 1
        WHERE id = ?
        """,
        (reminder_id,)
    )

    connection.commit()
    connection.close()


def delete_reminder(reminder_id):

    connection = sqlite3.connect(
        DATABASE_NAME
    )

    cursor = connection.cursor()

    cursor.execute(
        """
        DELETE FROM reminders
        WHERE id = ?
        """,
        (reminder_id,)
    )

    connection.commit()
    connection.close()