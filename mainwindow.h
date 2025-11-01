#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QLineEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QListWidget *urlList;
    QLineEdit *urlInputLine;
    QPushButton *downloadBtn;
    QListWidget *progressList;

    void setUpUI();
};
#endif // MAINWINDOW_H
