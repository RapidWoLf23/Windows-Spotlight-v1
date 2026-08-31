#pragma once

#include <QMainWindow>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QVector>

class QLineEdit;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPropertyAnimation;
class QTimer;
class QKeyEvent;
class QMouseEvent;

struct AppResult
{
    QString name;
    QString path;
    QString searchName;
};

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onSearchTextChanged(const QString &text);
    void launchSelectedResult();
    void continueIndexing();

private:
    // Indexing
    void startIndexing();
    void indexDirectory(const QString &directoryPath);

    // Search
    void searchApplications(const QString &query);
    int calculateScore(
        const AppResult &app,
        const QString &query
    ) const;

    // UI
    void addApplicationResult(const AppResult &app);
    void clearResults();
    void updateWindowHeight(int resultCount);
    void animateWindowHeight(int targetHeight);

    // Launch
    void launchApplication(const QString &path);

    // Widgets
    QLineEdit *searchBox = nullptr;
    QLabel *emptyLabel = nullptr;
    QListWidget *resultsList = nullptr;
    QPropertyAnimation *heightAnimation = nullptr;
    QTimer *indexTimer = nullptr;

    // Application data
    QVector<AppResult> applications;

    // Indexing data
    QStringList indexDirectories;
    int currentIndexDirectory = 0;

    // Dragging
    bool dragging = false;
    QPoint dragOffset;

    // Window
    int compactHeight = 108;
    int maximumResults = 8;
};