#pragma once

#include <QMainWindow>
#include <QPoint>
#include <QRect>

class QLineEdit;
class QListWidget;
class QPropertyAnimation;
class QMouseEvent;
class QKeyEvent;
class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void handleSearchTextChanged(const QString &text);

private:
    void updateWindowSize(int resultCount);
    void animateToGeometry(const QRect &target);

    QLineEdit *searchBox;
    QListWidget *resultsList;
    QPropertyAnimation *geometryAnimation;

    QRect compactGeometry;

    int windowWidth;
    int searchHeight;
    int resultRowHeight;

    bool dragging;
    QPoint dragOffset;
};