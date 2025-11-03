#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    dManager = new DownloadManager;
    setUpUI();

}

void MainWindow::setUpUI()
{
    resize(1000, 800);
    auto *central = new QWidget;
    auto *mainLayout = new QVBoxLayout(central);
    urlList = new QListWidget;
    urlList->setAcceptDrops(true);
    urlList->setDragDropMode(QAbstractItemView::DropOnly);
    urlInputLine = new QLineEdit;
    urlInputLine->setPlaceholderText("Вставьте ссылку на изображение");

    auto *addUrlLayout = new QHBoxLayout();
    addUrlBtl = new QPushButton("Добавить");
    addUrlLayout->addWidget(urlInputLine);
    addUrlLayout->addWidget(addUrlBtl);


    downloadBtn = new QPushButton("Скачать");
    mainLayout->addLayout(addUrlLayout);
    mainLayout->addWidget(new QLabel("Добавленные ссылки:"));
    mainLayout->addWidget(urlList);
    mainLayout->addWidget(downloadBtn);
    setCentralWidget(central);

    connect(addUrlBtl, &QPushButton::clicked, this, &MainWindow::onAddUrlBtnClicked);
    connect(urlInputLine, &QLineEdit::textChanged, this, &MainWindow::checkAddUrlBtn);
    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::onDownloadBtnClicked);

    connect(dManager, &DownloadManager::downloadProgress, this, &MainWindow::onUpdateProgress);
    connect(dManager, &DownloadManager::downloadEnd, this, &MainWindow::onDownloadFinished);
    connect(dManager, &DownloadManager::downloadError, this, [] (const QUrl &url, const QString &error)
    {
        qDebug() << "Ошибка при скачивании изображения" << error << "по ссылке" << url;
    });
}

void MainWindow::onAddUrlBtnClicked()
{
    QString text = urlInputLine->text().trimmed();
    if (text.isEmpty()) {
        return; // Или warning, если нужно
    }

    QUrl url(text);
    if (!url.isValid() || url.scheme().isEmpty()) { // Заменил UrlIsCorrect на стандартный чек
        QMessageBox::warning(this, "Ошибка", "Ссылка некорректна");
        urlInputLine->clear();
        return;
    }

    // Проверка на дубликат
    for (int i = 0; i < urlList->count(); ++i)
    {
        QListWidgetItem *item = urlList->item(i);
        if (item->data(Qt::UserRole).toUrl() == url)
        {
            QMessageBox::warning(this, "Ошибка", "Введенная ссылка уже существует");
            urlInputLine->clear();
            return;
        }
    }

    // Виджет для urlItem
    auto *widget = new QWidget;
    auto *hLayout = new QHBoxLayout(widget);
    hLayout->setContentsMargins(5, 2, 10, 2);

    auto *label = new QLabel(url.toString());
    //label->setWordWrap(true);
    hLayout->addWidget(label, 1);

    auto *newBar = new QProgressBar;
    newBar->setMaximum(100);
    newBar->setMinimum(0);
    newBar->setValue(0);
    newBar->setTextVisible(true);
    newBar->setFormat("%p%");
    newBar->setFixedHeight(20);
    newBar->setMinimumWidth(250);
    hLayout->addWidget(newBar);

    auto *urlItem = new QListWidgetItem;

    urlItem->setData(Qt::UserRole, url); // Хранение url
    urlItem->setData(Qt::UserRole + 1, QVariant::fromValue(newBar)); // Хранение ссылки на бар

    urlItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    urlList->addItem(urlItem);
    urlList->setItemWidget(urlItem, widget);

    qDebug() << "Добавлена ссылка" << url;
    qDebug() << "Размер списка:" << urlList->count();
    urlInputLine->clear();
}

void MainWindow::checkAddUrlBtn(QString text)
{
    addUrlBtl->setEnabled(!text.isEmpty());
}

void MainWindow::onDownloadBtnClicked()
{
    for (int i = 0; i < urlList->count(); ++i)
    {
        //QUrl *u = qvariant_cast<QUrl*>(urlList->item(i)->data(Qt::UserRole));
        auto *item = urlList->item(i);
        QUrl u = item->data(Qt::UserRole).toUrl();
        dManager->addImgToDownload(u);
    }
}

void MainWindow::onUpdateProgress(const QUrl &url, int percentage)
{
    for (int i = 0; i < urlList->count(); ++i) {
        QListWidgetItem *item = urlList->item(i);
        if (item->data(Qt::UserRole).toUrl() == url) {
            QProgressBar *progress = qvariant_cast<QProgressBar*>(item->data(Qt::UserRole + 1));
            if (progress) {
                progress->setValue(percentage);
            }
            break;
        }
    }
}

void MainWindow::onDownloadFinished(const QUrl &url, const QByteArray &data)
{
    auto *scale = new ScaleImage(url, data);
    //connect(scale, &ScaleImage::finished, this, &MainWindow::);

    QThreadPool::globalInstance()->start(scale);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

    for (const auto &url : mime->urls())
    {
        if (url.scheme() == "http" || url.scheme() == "https")
        {
            auto *urlitem = new QListWidgetItem(url.toString());
            urlitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            urlList->addItem(urlitem);
        }
    }
}

MainWindow::~MainWindow() {}
