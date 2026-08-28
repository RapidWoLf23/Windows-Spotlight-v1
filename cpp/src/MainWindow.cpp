#include "MainWindow.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QScreen>
#include <QSizePolicy>
#include <QVBoxLayout>
#include <QWidget>
#include <QFrame>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      searchBox(nullptr),
      resultsList(nullptr),
      geometryAnimation(nullptr),
      windowWidth(620),
      searchHeight(56),
      resultRowHeight(48),
      dragging(false)
{
    // =========================================================
    // WINDOW
    // =========================================================

    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint
    );

    setAttribute(Qt::WA_TranslucentBackground);

    /*
     * IMPORTANT:
     *
     * We do NOT use setFixedSize().
     *
     * The width stays fixed, but the height is allowed
     * to animate.
     */
    setMinimumWidth(windowWidth);
    setMaximumWidth(windowWidth);

    setMinimumHeight(84);
    setMaximumHeight(500);


    // =========================================================
    // CONTAINER
    // =========================================================

    QWidget *container = new QWidget(this);

    container->setObjectName("container");

    container->setStyleSheet(
        "#container {"
        "    background: rgba(248, 248, 252, 245);"
        "    border: 1px solid rgba(180, 180, 190, 100);"
        "    border-radius: 20px;"
        "}"
    );

    setCentralWidget(container);


    // =========================================================
    // MAIN LAYOUT
    // =========================================================

    QVBoxLayout *layout =
        new QVBoxLayout(container);

    /*
     * This is critical.
     *
     * Everything starts at the TOP.
     * Nothing is vertically centered.
     */
    layout->setAlignment(Qt::AlignTop);

    layout->setContentsMargins(
        14,
        14,
        14,
        14
    );

    layout->setSpacing(10);


    // =========================================================
    // SEARCH BOX
    // =========================================================

    searchBox =
        new QLineEdit(container);

    searchBox->setPlaceholderText(
        "Search reminders..."
    );

    searchBox->setFixedHeight(searchHeight);

    searchBox->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );

    searchBox->setStyleSheet(
        "QLineEdit {"
        "    background: rgba(232, 232, 240, 245);"
        "    border: 1px solid rgba(170, 170, 185, 90);"
        "    border-radius: 15px;"
        "    padding-left: 18px;"
        "    padding-right: 18px;"
        "    font-size: 19px;"
        "    color: #202124;"
        "}"
        ""
        "QLineEdit:focus {"
        "    background: rgba(226, 226, 236, 255);"
        "    border: 1px solid rgba(140, 140, 160, 130);"
        "}"
    );

    layout->addWidget(searchBox);


    // =========================================================
    // RESULTS LIST
    // =========================================================

    resultsList =
        new QListWidget(container);

    resultsList->setVisible(false);

    resultsList->setFrameShape(QFrame::NoFrame);

    resultsList->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    resultsList->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    resultsList->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );

    resultsList->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "    padding: 0px;"
        "    margin: 0px;"
        "}"
        ""
        "QListWidget::item {"
        "    background: rgba(235, 235, 242, 190);"
        "    border-radius: 12px;"
        "    padding-left: 16px;"
        "    padding-right: 16px;"
        "    margin: 2px 0px;"
        "    color: #202124;"
        "    font-size: 16px;"
        "}"
        ""
        "QListWidget::item:selected {"
        "    background: rgba(215, 215, 225, 230);"
        "}"
    );

    layout->addWidget(resultsList);


    // =========================================================
    // ANIMATION
    // =========================================================

    geometryAnimation =
        new QPropertyAnimation(
            this,
            "geometry",
            this
        );

    geometryAnimation->setDuration(220);

    geometryAnimation->setEasingCurve(
        QEasingCurve::OutCubic
    );


    // =========================================================
    // SEARCH CONNECTION
    // =========================================================

    connect(
        searchBox,
        &QLineEdit::textChanged,
        this,
        &MainWindow::handleSearchTextChanged
    );


    // =========================================================
    // INITIAL POSITION
    // =========================================================

    QScreen *screen =
        QApplication::primaryScreen();

    if (screen)
    {
        QRect available =
            screen->availableGeometry();

        int x =
            available.center().x()
            - windowWidth / 2;

        /*
         * This Y coordinate becomes the permanent TOP
         * anchor of the Spotlight window.
         */
        int y =
            available.center().y() - 180;

        compactGeometry =
            QRect(
                x,
                y,
                windowWidth,
                84
            );

        setGeometry(compactGeometry);
    }


    // =========================================================
    // INITIAL FOCUS
    // =========================================================

    searchBox->setFocus();
}


// =============================================================
// SEARCH
// =============================================================

void MainWindow::handleSearchTextChanged(
    const QString &text
)
{
    QString query =
        text.trimmed();

    /*
     * ========================================================
     * TEMPORARY TEST RESULTS
     * ========================================================
     *
     * These are ONLY here to test the animation.
     *
     * We will DELETE this section when we build the real
     * search engine.
     *
     * There are no permanent/fake suggestions anymore.
     */

    resultsList->clear();

    if (query.isEmpty())
    {
        resultsList->setVisible(false);

        updateWindowSize(0);

        return;
    }


    // ---------------------------------------------------------
    // Temporary results for animation testing
    // ---------------------------------------------------------

    if (query.contains(
            "chrome",
            Qt::CaseInsensitive))
    {
        resultsList->addItem(
            "Chrome"
        );

        resultsList->addItem(
            "Open Chrome"
        );

        resultsList->addItem(
            "Chrome settings"
        );
    }
    else
    {
        /*
         * For any other search term we currently show
         * one test result.
         *
         * This will later be replaced by the real search
         * engine.
         */
        resultsList->addItem(
            "Searching for \"" + query + "\"..."
        );
    }


    // ---------------------------------------------------------
    // Show results
    // ---------------------------------------------------------

    int count =
        resultsList->count();

    if (count > 0)
    {
        resultsList->setVisible(true);

        /*
         * Give the list exactly enough height for its
         * results.
         */
        int listHeight =
            count * resultRowHeight + 4;

        resultsList->setFixedHeight(
            listHeight
        );
    }

    updateWindowSize(count);
}


// =============================================================
// WINDOW SIZE
// =============================================================

void MainWindow::updateWindowSize(
    int resultCount
)
{
    /*
     * ---------------------------------------------------------
     * IMPORTANT GEOMETRY
     * ---------------------------------------------------------
     *
     * Top margin        = 14
     * Search            = 56
     * Gap               = 10
     * Results           = resultCount * row height
     * Bottom margin     = 14
     *
     * Therefore:
     *
     * Empty:
     *
     * 14 + 56 + 14 = 84
     *
     * Three results:
     *
     * 14 + 56 + 10 + 3*48 + 4 + 14
     * = 242
     *
     * The TOP NEVER CHANGES.
     */

    const int topMargin = 14;
    const int bottomMargin = 14;
    const int gap = 10;

    int targetHeight;

    if (resultCount <= 0)
    {
        targetHeight =
            topMargin +
            searchHeight +
            bottomMargin;
    }
    else
    {
        int resultsHeight =
            resultCount *
            resultRowHeight +
            4;

        targetHeight =
            topMargin +
            searchHeight +
            gap +
            resultsHeight +
            bottomMargin;
    }


    // ---------------------------------------------------------
    // Keep the top-left position exactly where it is.
    // ---------------------------------------------------------

    QRect current =
        geometry();

    QRect target =
        current;

    /*
     * DO NOT change target.x()
     * DO NOT change target.y()
     *
     * Only height changes.
     */
    target.setWidth(windowWidth);

    target.setHeight(targetHeight);


    animateToGeometry(target);
}


// =============================================================
// ANIMATE GEOMETRY
// =============================================================

void MainWindow::animateToGeometry(
    const QRect &target
)
{
    QRect current =
        geometry();

    if (current == target)
        return;

    geometryAnimation->stop();

    geometryAnimation->setStartValue(
        current
    );

    geometryAnimation->setEndValue(
        target
    );

    geometryAnimation->start();
}


// =============================================================
// MOUSE DRAGGING
// =============================================================

void MainWindow::mousePressEvent(
    QMouseEvent *event
)
{
    if (
        event->button() ==
        Qt::LeftButton
    )
    {
        dragging = true;

        dragOffset =
            event->globalPosition().toPoint()
            -
            frameGeometry().topLeft();

        event->accept();

        return;
    }

    QMainWindow::mousePressEvent(event);
}


void MainWindow::mouseMoveEvent(
    QMouseEvent *event
)
{
    if (
        dragging &&
        (event->buttons() &
         Qt::LeftButton)
    )
    {
        QPoint newPosition =
            event->globalPosition().toPoint()
            -
            dragOffset;

        move(newPosition);

        /*
         * Keep the compact position synchronized.
         */
        compactGeometry.moveTopLeft(
            newPosition
        );

        event->accept();

        return;
    }

    QMainWindow::mouseMoveEvent(event);
}


void MainWindow::mouseReleaseEvent(
    QMouseEvent *event
)
{
    if (
        event->button() ==
        Qt::LeftButton
    )
    {
        dragging = false;

        event->accept();

        return;
    }

    QMainWindow::mouseReleaseEvent(event);
}


// =============================================================
// KEYBOARD
// =============================================================

void MainWindow::keyPressEvent(
    QKeyEvent *event
)
{
    if (
        event->key() ==
        Qt::Key_Escape
    )
    {
        close();

        return;
    }

    QMainWindow::keyPressEvent(event);
}