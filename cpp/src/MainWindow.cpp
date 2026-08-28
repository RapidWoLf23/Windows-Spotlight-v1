#include "MainWindow.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Spotlight-style window
    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint
    );

    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(720, 260);

    // Main container
    QWidget *container = new QWidget(this);

    container->setStyleSheet(
        "QWidget {"
        "    background: rgba(248, 248, 252, 245);"
        "    border: 1px solid rgba(180, 180, 190, 100);"
        "    border-radius: 22px;"
        "}"
    );

    setCentralWidget(container);

    // Main layout
    QVBoxLayout *layout = new QVBoxLayout(container);

    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    // Search box
    searchBox = new QLineEdit();

    searchBox->setPlaceholderText("Search reminders...");
    searchBox->setMinimumHeight(58);

    searchBox->setStyleSheet(
        "QLineEdit {"
        "    background: rgba(235, 235, 242, 245);"
        "    border: none;"
        "    border-radius: 16px;"
        "    padding: 0 20px;"
        "    font-size: 20px;"
        "    color: #202124;"
        "}"
        ""
        "QLineEdit:focus {"
        "    background: rgba(228, 228, 238, 255);"
        "}"
    );

    layout->addWidget(searchBox);

    // Empty state
    emptyLabel = new QLabel("No reminders yet");

    emptyLabel->setAlignment(Qt::AlignCenter);

    emptyLabel->setStyleSheet(
        "QLabel {"
        "    color: #777780;"
        "    font-size: 17px;"
        "    background: transparent;"
        "    border: none;"
        "}"
    );

    layout->addWidget(emptyLabel);

    // Results list
    resultsList = new QListWidget();

    resultsList->setVisible(false);

    resultsList->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    font-size: 16px;"
        "}"
    );

    layout->addWidget(resultsList);

    // Focus search immediately
    searchBox->setFocus();

    // Put window roughly in the center of the screen
    QScreen *screen = QApplication::primaryScreen();

    if (screen)
    {
        QRect available = screen->availableGeometry();

        move(
            available.center().x() - width() / 2,
            available.center().y() - height() / 3
        );
    }
}


// ---------------------------------------------------------
// Dragging
// ---------------------------------------------------------

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = true;

        dragOffset =
            event->globalPosition().toPoint() - frameGeometry().topLeft();

        event->accept();
        return;
    }

    QMainWindow::mousePressEvent(event);
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging &&
        (event->buttons() & Qt::LeftButton))
    {
        move(
            event->globalPosition().toPoint() - dragOffset
        );

        event->accept();
        return;
    }

    QMainWindow::mouseMoveEvent(event);
}


void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = false;
        event->accept();
        return;
    }

    QMainWindow::mouseReleaseEvent(event);
}


// ---------------------------------------------------------
// Keyboard
// ---------------------------------------------------------

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Escape closes Spotlight
    if (event->key() == Qt::Key_Escape)
    {
        close();
        return;
    }

    QMainWindow::keyPressEvent(event);
}