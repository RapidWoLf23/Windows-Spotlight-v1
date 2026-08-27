#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Windows Spotlight");
    window.resize(600, 120);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    QLineEdit *input = new QLineEdit();
    input->setPlaceholderText("What do you want to do?");

    layout->addWidget(input);

    window.show();

    return app.exec();
}