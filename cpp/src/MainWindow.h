#pragma once

#include <QMainWindow>
#include <QPoint>

class QLineEdit;
class QLabel;
class QListWidget;

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

private:
    QLineEdit *searchBox;
    QLabel *emptyLabel;
    QListWidget *resultsList;

    bool dragging = false;
    QPoint dragOffset;
};