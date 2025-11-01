#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setUpUI();

}

void MainWindow::setUpUI()
{
    auto *central = new QWidget;
    auto *mainLayout = new QVBoxLayout(central);
    urlList = new QListWidget;
    urlInputLine = new QLineEdit;
    urlInputLine->setPlaceholderText("Вставьте ссылку на изображение");


    mainLayout->addWidget(urlInputLine);
    mainLayout->addWidget(urlList);
    setCentralWidget(central);
}

MainWindow::~MainWindow() {}
