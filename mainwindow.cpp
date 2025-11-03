#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    dManager = new DownloadManager;
    setUpUI();
    setStyles();

}

void MainWindow::setUpUI()
{
    resize(1000, 800);
    auto *central = new QWidget;
    auto *mainLayout = new QVBoxLayout(central);

    //Список для отобржения ссылок на загрузку
    urlList = new QListWidget;
    urlList->setAcceptDrops(true);
    urlList->setDragDropMode(QAbstractItemView::DropOnly);

    //Список для отображения прогресса скачивания и состояния
    progressTable = new QTableWidget(0, 3, this);
    progressTable->setHorizontalHeaderLabels({"Ссылка", "Процент скачивания", "Состояние"});
    progressTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    urlInputLine = new QLineEdit;
    urlInputLine->setPlaceholderText("Вставьте ссылку на изображение");

    auto *addUrlLayout = new QHBoxLayout;
    addUrlBtl = new QPushButton("Добавить");
    addUrlLayout->addWidget(urlInputLine);
    addUrlLayout->addWidget(addUrlBtl);

    downloadBtn = new QPushButton("Скачать");
    mainLayout->addLayout(addUrlLayout);
    mainLayout->addWidget(new QLabel("Добавленные ссылки:"));
    mainLayout->addWidget(urlList);
    mainLayout->addWidget(downloadBtn);
    mainLayout->addWidget(progressTable);
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

void MainWindow::setStyles()
{
    //Применение стилей
    QFile file(":/styles/theme1.qss");
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = file.readAll();
        setStyleSheet(styleSheet);
        file.close();
    }
}

void MainWindow::onAddUrlBtnClicked()
{
    QString text = urlInputLine->text().trimmed();

    QUrl url(text);
    if (!url.isValid() || url.scheme().isEmpty())
    {
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

    auto *label = new QLabel(url.toString());
    hLayout->setContentsMargins(8, 2, 8, 2);
    hLayout->addWidget(label, 1);

    auto *urlItem = new QListWidgetItem;
    urlItem->setData(Qt::UserRole, url);
    urlItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    urlItem->setSizeHint(QSize(-1, 150));

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
    downloadBtn->setEnabled(false);

    for (int i = urlList->count() - 1; i >= 0; --i)
    {
        QListWidgetItem *item = urlList->takeItem(i);
        QUrl url = item->data(Qt::UserRole).toUrl();

        // новая строка для таблицы
        int row = progressTable->rowCount();
        progressTable->insertRow(row);

        // ячейка 0 - текст url
        QTableWidgetItem *urlItem = new QTableWidgetItem(url.toString());
        urlItem->setData(Qt::UserRole, url); // Хранение url
        urlItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        progressTable->setItem(row, 0, urlItem);

        //ячейка 1 - виджет для прогресса
        auto *widget = new QWidget;
        auto *hLayout = new QHBoxLayout(widget);
        hLayout->setContentsMargins(8, 2, 8, 2);

        auto *label = new QLabel(url.toString());
        hLayout->addWidget(label, 1);

        auto *newBar = new QProgressBar;
        newBar->setMaximum(100);
        newBar->setMinimum(0);
        newBar->setValue(0);
        newBar->setTextVisible(true);
        newBar->setFormat("%p%");
        newBar->setMinimumWidth(250);
        newBar->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(newBar);

        // Хранение ссылки на бар (в item колонки 0, например)
        urlItem->setData(Qt::UserRole + 1, QVariant::fromValue(newBar));

        // Устанавливаем widget в ячейку 1
        progressTable->setCellWidget(row, 1, widget);

        dManager->addImgToDownload(url);
    }
}

void MainWindow::onUpdateProgress(const QUrl &url, int percentage)
{
    for (int i = 0; i < progressTable->rowCount(); ++i) {
        auto *item = progressTable->item(i, 0);
        if (item->data(Qt::UserRole).toUrl() == url)
        {
            QProgressBar *progress = qvariant_cast<QProgressBar*>(item->data(Qt::UserRole + 1));
            if (progress)
            {
                progress->setValue(percentage);
                qDebug() << "Процент скачивания:" << percentage;
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
