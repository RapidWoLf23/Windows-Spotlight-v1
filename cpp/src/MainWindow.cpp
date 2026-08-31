#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QEasingCurve>
#include <QSizePolicy>
#include <QIcon>
#include <QFrame>
#include <QAbstractItemView>

#include <algorithm>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif


// =========================================================
// CONSTRUCTOR
// =========================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // =========================================================
    // WINDOW
    // =========================================================

    setWindowFlags(
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::Tool
    );

    setAttribute(
        Qt::WA_TranslucentBackground
    );

    setFixedWidth(720);

    resize(
        720,
        compactHeight
    );


    // =========================================================
    // MAIN CONTAINER
    // =========================================================

    QWidget *container =
        new QWidget(this);

    container->setObjectName(
        "container"
    );

    container->setStyleSheet(
        "#container {"
        "    background: rgba(248, 248, 252, 248);"
        "    border: 1px solid rgba(180, 180, 190, 100);"
        "    border-radius: 22px;"
        "}"
    );

    setCentralWidget(container);


    // =========================================================
    // MAIN LAYOUT
    // =========================================================

    QVBoxLayout *layout =
        new QVBoxLayout(container);

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
        "Search apps..."
    );

    searchBox->setMinimumHeight(58);

    searchBox->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Fixed
    );

    searchBox->setStyleSheet(
        "QLineEdit {"
        "    background: rgba(235, 235, 242, 245);"
        "    border: none;"
        "    border-radius: 16px;"
        "    padding: 0 20px;"
        "    font-size: 20px;"
        "    color: #202124;"
        "    selection-background-color: #cfd0dc;"
        "}"
        ""
        "QLineEdit:focus {"
        "    background: rgba(228, 228, 238, 255);"
        "}"
    );

    layout->addWidget(
        searchBox
    );


    // =========================================================
    // EMPTY LABEL
    // =========================================================

    emptyLabel =
        new QLabel(container);

    emptyLabel->setVisible(false);

    emptyLabel->setAlignment(
        Qt::AlignCenter
    );

    emptyLabel->setMinimumHeight(40);

    emptyLabel->setStyleSheet(
        "QLabel {"
        "    color: #777780;"
        "    font-size: 16px;"
        "    background: transparent;"
        "    border: none;"
        "}"
    );

    layout->addWidget(
        emptyLabel
    );


    // =========================================================
    // RESULTS LIST
    // =========================================================

    resultsList =
        new QListWidget(container);

    resultsList->setVisible(false);

    resultsList->setSpacing(4);

    resultsList->setFrameShape(
        QFrame::NoFrame
    );

    resultsList->setVerticalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    resultsList->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    resultsList->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    resultsList->setFocusPolicy(
        Qt::NoFocus
    );

    resultsList->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "    font-size: 17px;"
        "}"
        ""
        "QListWidget::item {"
        "    background: rgba(235, 235, 242, 150);"
        "    border-radius: 14px;"
        "    padding: 8px 14px;"
        "    margin: 1px 0;"
        "    color: #202124;"
        "}"
        ""
        "QListWidget::item:hover {"
        "    background: rgba(225, 225, 235, 210);"
        "}"
        ""
        "QListWidget::item:selected {"
        "    background: rgba(210, 211, 225, 245);"
        "    color: #202124;"
        "}"
    );

    layout->addWidget(
        resultsList
    );


    // =========================================================
    // CLICK RESULT = LAUNCH
    // =========================================================

    connect(
        resultsList,
        &QListWidget::itemClicked,
        this,
        [this](QListWidgetItem *item)
        {
            if (!item)
            {
                return;
            }

            resultsList->setCurrentItem(item);

            launchSelectedResult();
        }
    );


    // =========================================================
    // DOUBLE CLICK RESULT = LAUNCH
    // =========================================================

    connect(
        resultsList,
        &QListWidget::itemDoubleClicked,
        this,
        [this](QListWidgetItem *item)
        {
            if (!item)
            {
                return;
            }

            resultsList->setCurrentItem(item);

            launchSelectedResult();
        }
    );


    // =========================================================
    // HEIGHT ANIMATION
    // =========================================================

    heightAnimation =
        new QPropertyAnimation(
            this,
            "geometry"
        );

    heightAnimation->setDuration(
        180
    );

    heightAnimation->setEasingCurve(
        QEasingCurve::OutCubic
    );


    // =========================================================
    // SEARCH SIGNAL
    // =========================================================

    connect(
        searchBox,
        &QLineEdit::textChanged,
        this,
        &MainWindow::onSearchTextChanged
    );


    // =========================================================
    // ENTER = LAUNCH
    // =========================================================

    connect(
        searchBox,
        &QLineEdit::returnPressed,
        this,
        &MainWindow::launchSelectedResult
    );


    // =========================================================
    // INDEX TIMER
    // =========================================================

    indexTimer =
        new QTimer(this);

    indexTimer->setSingleShot(true);

    connect(
        indexTimer,
        &QTimer::timeout,
        this,
        &MainWindow::continueIndexing
    );


    // =========================================================
    // START INDEXING
    // =========================================================

    startIndexing();


    // =========================================================
    // FOCUS
    // =========================================================

    searchBox->setFocus();


    // =========================================================
    // CENTER WINDOW
    // =========================================================

    QScreen *screen =
        QApplication::primaryScreen();

    if (screen)
    {
        const QRect available =
            screen->availableGeometry();

        move(
            available.center().x()
                - width() / 2,

            available.center().y()
                - height() / 2
        );
    }
}


// =========================================================
// START INDEXING
// =========================================================

void MainWindow::startIndexing()
{
#ifdef Q_OS_WIN

    applications.clear();

    indexDirectories.clear();

    currentIndexDirectory = 0;


    // ---------------------------------------------------------
    // USER START MENU
    // ---------------------------------------------------------

    const QString userStartMenu =
        QDir::homePath()
        + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs";


    // ---------------------------------------------------------
    // ALL USERS START MENU
    // ---------------------------------------------------------

    const QString allUsersStartMenu =
        "C:/ProgramData/Microsoft/Windows/Start Menu/Programs";


    if (QDir(userStartMenu).exists())
    {
        indexDirectories.append(
            userStartMenu
        );
    }

    if (QDir(allUsersStartMenu).exists())
    {
        indexDirectories.append(
            allUsersStartMenu
        );
    }


    // ---------------------------------------------------------
    // INDEX ASYNCHRONOUSLY
    // ---------------------------------------------------------

    if (!indexDirectories.isEmpty())
    {
        indexTimer->start(0);
    }

#endif
}


// =========================================================
// INDEX DIRECTORY
// =========================================================

void MainWindow::indexDirectory(
    const QString &directoryPath
)
{
#ifdef Q_OS_WIN

    QDir directory(
        directoryPath
    );

    if (!directory.exists())
    {
        return;
    }


    QDirIterator iterator(
        directoryPath,
        QStringList()
            << "*.lnk"
            << "*.url",
        QDir::Files,
        QDirIterator::Subdirectories
    );


    while (iterator.hasNext())
    {
        const QString path =
            iterator.next();

        const QFileInfo info(path);

        if (!info.exists() ||
            !info.isFile())
        {
            continue;
        }


        QString name =
            info.completeBaseName()
                .trimmed();

        if (name.isEmpty())
        {
            continue;
        }


        AppResult result;

        result.name = name;

        result.path = path;

        result.searchName =
            name.toLower();


        applications.append(
            result
        );
    }

#endif
}


// =========================================================
// CONTINUE INDEXING
// =========================================================

void MainWindow::continueIndexing()
{
    if (currentIndexDirectory <
        indexDirectories.size())
    {
        indexDirectory(
            indexDirectories.at(
                currentIndexDirectory
            )
        );

        ++currentIndexDirectory;


        if (currentIndexDirectory <
            indexDirectories.size())
        {
            indexTimer->start(0);
        }
    }


    if (currentIndexDirectory >=
        indexDirectories.size())
    {
        indexTimer->stop();


        // -----------------------------------------------------
        // REMOVE DUPLICATES
        // -----------------------------------------------------

        QVector<AppResult> unique;

        unique.reserve(
            applications.size()
        );


        for (const AppResult &app :
             applications)
        {
            bool duplicate = false;


            for (const AppResult &existing :
                 unique)
            {
                if (
                    existing.path.compare(
                        app.path,
                        Qt::CaseInsensitive
                    ) == 0
                )
                {
                    duplicate = true;
                    break;
                }
            }


            if (!duplicate)
            {
                unique.append(
                    app
                );
            }
        }


        applications =
            unique;


        // -----------------------------------------------------
        // SORT
        // -----------------------------------------------------

        std::sort(
            applications.begin(),
            applications.end(),
            [](const AppResult &a,
               const AppResult &b)
            {
                return a.searchName <
                       b.searchName;
            }
        );


        // -----------------------------------------------------
        // REFRESH SEARCH
        // -----------------------------------------------------

        if (!searchBox->text()
                 .trimmed()
                 .isEmpty())
        {
            searchApplications(
                searchBox->text()
            );
        }
    }
}


// =========================================================
// SEARCH TEXT CHANGED
// =========================================================

void MainWindow::onSearchTextChanged(
    const QString &text
)
{
    searchApplications(
        text
    );
}


// =========================================================
// SEARCH APPLICATIONS
// =========================================================

void MainWindow::searchApplications(
    const QString &query
)
{
    if (!resultsList ||
        !emptyLabel)
    {
        return;
    }


    resultsList->clear();


    const QString trimmed =
        query.trimmed();


    // ---------------------------------------------------------
    // EMPTY SEARCH
    // ---------------------------------------------------------

    if (trimmed.isEmpty())
    {
        resultsList->setVisible(
            false
        );

        emptyLabel->setVisible(
            false
        );

        updateWindowHeight(
            0
        );

        return;
    }


    const QString lowerQuery =
        trimmed.toLower();


    // ---------------------------------------------------------
    // SCORED RESULT
    // ---------------------------------------------------------

    struct ScoredResult
    {
        AppResult app;
        int score;
    };


    QVector<ScoredResult> scored;


    // ---------------------------------------------------------
    // SCORE ALL APPLICATIONS
    // ---------------------------------------------------------

    for (const AppResult &app :
         applications)
    {
        const int score =
            calculateScore(
                app,
                lowerQuery
            );


        if (score > 0)
        {
            ScoredResult result;

            result.app = app;
            result.score = score;

            scored.append(
                result
            );
        }
    }


    // ---------------------------------------------------------
    // SORT BY SCORE
    // ---------------------------------------------------------

    std::sort(
        scored.begin(),
        scored.end(),
        [](const ScoredResult &a,
           const ScoredResult &b)
        {
            if (a.score != b.score)
            {
                return a.score >
                       b.score;
            }

            return a.app.searchName <
                   b.app.searchName;
        }
    );


    // ---------------------------------------------------------
    // LIMIT RESULTS
    // ---------------------------------------------------------

    const int count =
        qMin(
            maximumResults,
            scored.size()
        );


    // ---------------------------------------------------------
    // NOTHING FOUND
    // ---------------------------------------------------------

    if (count == 0)
    {
        resultsList->setVisible(
            false
        );

        emptyLabel->setText(
            "No results"
        );

        emptyLabel->setVisible(
            true
        );

        updateWindowHeight(
            0
        );

        return;
    }


    // ---------------------------------------------------------
    // SHOW RESULTS
    // ---------------------------------------------------------

    emptyLabel->setVisible(
        false
    );

    resultsList->setVisible(
        true
    );


    for (int i = 0;
         i < count;
         ++i)
    {
        addApplicationResult(
            scored.at(i).app
        );
    }


    // ---------------------------------------------------------
    // SELECT FIRST
    // ---------------------------------------------------------

    if (resultsList->count() > 0)
    {
        resultsList->setCurrentRow(
            0
        );
    }


    updateWindowHeight(
        count
    );
}


// =========================================================
// SEARCH SCORE
// =========================================================

int MainWindow::calculateScore(
    const AppResult &app,
    const QString &query
) const
{
    const QString name =
        app.searchName;


    // ---------------------------------------------------------
    // EXACT MATCH
    // ---------------------------------------------------------

    if (name == query)
    {
        return 10000;
    }


    // ---------------------------------------------------------
    // STARTS WITH
    // ---------------------------------------------------------

    if (name.startsWith(query))
    {
        return 8500
            - qMin(
                name.length(),
                200
            );
    }


    // ---------------------------------------------------------
    // WORD START MATCH
    // ---------------------------------------------------------

    int wordStartScore = 0;


    for (int position = 0;
         position < name.length();
         ++position)
    {
        const bool wordStart =
            position == 0 ||
            name.at(position - 1).isSpace() ||
            name.at(position - 1) == '-' ||
            name.at(position - 1) == '_' ||
            name.at(position - 1) == '.';


        if (!wordStart)
        {
            continue;
        }


        if (
            name.mid(
                position,
                query.length()
            ) == query
        )
        {
            wordStartScore =
                6500
                - qMin(
                    position * 10,
                    1000
                );

            break;
        }
    }


    if (wordStartScore > 0)
    {
        return wordStartScore;
    }


    // ---------------------------------------------------------
    // CONTAINS
    // ---------------------------------------------------------

    const int containsPosition =
        name.indexOf(
            query
        );


    if (containsPosition >= 0)
    {
        return 4000
            - qMin(
                containsPosition * 10,
                1500
            );
    }


    // ---------------------------------------------------------
    // FUZZY MATCH
    // ---------------------------------------------------------

    int queryIndex = 0;

    int consecutive = 0;

    int fuzzyScore = 0;


    for (
        int i = 0;
        i < name.length() &&
        queryIndex < query.length();
        ++i
    )
    {
        if (
            name.at(i) ==
            query.at(queryIndex)
        )
        {
            ++queryIndex;

            ++consecutive;

            fuzzyScore +=
                300 +
                consecutive * 25;
        }
        else
        {
            consecutive = 0;
        }
    }


    if (queryIndex ==
        query.length())
    {
        return 1000 +
               qMax(
                   0,
                   fuzzyScore -
                   name.length() * 3
               );
    }


    return 0;
}


// =========================================================
// ADD RESULT
// =========================================================

void MainWindow::addApplicationResult(
    const AppResult &app
)
{
    QListWidgetItem *item =
        new QListWidgetItem();


    // ---------------------------------------------------------
    // ICON
    // ---------------------------------------------------------

    QFileIconProvider iconProvider;

    QIcon icon =
        iconProvider.icon(
            QFileInfo(app.path)
        );


    if (!icon.isNull())
    {
        item->setIcon(
            icon
        );
    }


    // ---------------------------------------------------------
    // NAME
    // ---------------------------------------------------------

    item->setText(
        app.name
    );


    // ---------------------------------------------------------
    // PATH
    // ---------------------------------------------------------

    item->setData(
        Qt::UserRole,
        app.path
    );


    // ---------------------------------------------------------
    // SIZE
    // ---------------------------------------------------------

    item->setSizeHint(
        QSize(
            0,
            52
        )
    );


    resultsList->addItem(
        item
    );
}


// =========================================================
// CLEAR RESULTS
// =========================================================

void MainWindow::clearResults()
{
    if (!resultsList)
    {
        return;
    }

    resultsList->clear();
}


// =========================================================
// UPDATE WINDOW HEIGHT
// =========================================================

void MainWindow::updateWindowHeight(
    int resultCount
)
{
    int targetHeight =
        compactHeight;


    if (resultCount > 0)
    {
        constexpr int rowHeight = 52;

        constexpr int spacing = 4;


        const int resultAreaHeight =
            resultCount * rowHeight
            +
            (resultCount - 1) * spacing;


        targetHeight =
            compactHeight
            +
            resultAreaHeight
            +
            10;
    }


    targetHeight =
        qBound(
            compactHeight,
            targetHeight,
            520
        );


    animateWindowHeight(
        targetHeight
    );
}


// =========================================================
// ANIMATE WINDOW HEIGHT
// =========================================================

void MainWindow::animateWindowHeight(
    int targetHeight
)
{
    if (!heightAnimation)
    {
        resize(
            width(),
            targetHeight
        );

        return;
    }


    const QRect current =
        geometry();


    if (current.height() ==
        targetHeight)
    {
        return;
    }


    heightAnimation->stop();


    const int heightDifference =
        targetHeight -
        current.height();


    QRect target =
        current;


    target.setHeight(
        targetHeight
    );


    target.moveTop(
        current.top()
        -
        heightDifference / 2
    );


    heightAnimation->setStartValue(
        current
    );

    heightAnimation->setEndValue(
        target
    );


    heightAnimation->start();
}


// =========================================================
// LAUNCH SELECTED RESULT
// =========================================================

void MainWindow::launchSelectedResult()
{
    if (!resultsList)
    {
        return;
    }


    QListWidgetItem *item =
        resultsList->currentItem();


    if (!item)
    {
        return;
    }


    const QString path =
        item->data(
            Qt::UserRole
        ).toString();


    if (path.isEmpty())
    {
        return;
    }


    launchApplication(
        path
    );
}


// =========================================================
// LAUNCH APPLICATION
// =========================================================

void MainWindow::launchApplication(
    const QString &path
)
{
#ifdef Q_OS_WIN

    if (path.isEmpty())
    {
        return;
    }


    const std::wstring widePath =
        QDir::toNativeSeparators(path)
            .toStdWString();


    const HINSTANCE result =
        ShellExecuteW(
            nullptr,
            L"open",
            widePath.c_str(),
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        );


    // Windows returns a value greater
    // than 32 when ShellExecute succeeds.

    if (
        reinterpret_cast<INT_PTR>(
            result
        ) > 32
    )
    {
        close();
    }

#else

    Q_UNUSED(path);

#endif
}


// =========================================================
// MOUSE PRESS
// =========================================================

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
            event->globalPosition()
                .toPoint()
            -
            frameGeometry().topLeft();


        event->accept();

        return;
    }


    QMainWindow::mousePressEvent(
        event
    );
}


// =========================================================
// MOUSE MOVE
// =========================================================

void MainWindow::mouseMoveEvent(
    QMouseEvent *event
)
{
    if (
        dragging &&
        (
            event->buttons()
            &
            Qt::LeftButton
        )
    )
    {
        move(
            event->globalPosition()
                .toPoint()
            -
            dragOffset
        );


        event->accept();

        return;
    }


    QMainWindow::mouseMoveEvent(
        event
    );
}


// =========================================================
// MOUSE RELEASE
// =========================================================

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


    QMainWindow::mouseReleaseEvent(
        event
    );
}


// =========================================================
// KEYBOARD
// =========================================================

void MainWindow::keyPressEvent(
    QKeyEvent *event
)
{
    // ---------------------------------------------------------
    // ESCAPE
    // ---------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_Escape
    )
    {
        close();

        return;
    }


    // ---------------------------------------------------------
    // DOWN
    // ---------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_Down
    )
    {
        if (
            resultsList &&
            resultsList->isVisible() &&
            resultsList->count() > 0
        )
        {
            const int row =
                resultsList->currentRow();


            if (
                row <
                resultsList->count() - 1
            )
            {
                resultsList->setCurrentRow(
                    row + 1
                );
            }

            return;
        }
    }


    // ---------------------------------------------------------
    // UP
    // ---------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_Up
    )
    {
        if (
            resultsList &&
            resultsList->isVisible() &&
            resultsList->count() > 0
        )
        {
            const int row =
                resultsList->currentRow();


            if (row > 0)
            {
                resultsList->setCurrentRow(
                    row - 1
                );
            }

            return;
        }
    }


    // ---------------------------------------------------------
    // ENTER
    // ---------------------------------------------------------

    if (
        event->key() ==
        Qt::Key_Return ||
        event->key() ==
        Qt::Key_Enter
    )
    {
        if (
            resultsList &&
            resultsList->isVisible() &&
            resultsList->count() > 0
        )
        {
            launchSelectedResult();

            return;
        }
    }


    QMainWindow::keyPressEvent(
        event
    );
}