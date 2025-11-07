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
    urlList->setAcceptDrops(true);

    //Список для отображения прогресса скачивания и состояния
    progressTable = new QTableWidget(0, 3, this);
    progressTable->setHorizontalHeaderLabels({"Ссылка", "Состояние скачивания", "Состояние"});
    progressTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    urlInputLine = new QLineEdit;
    urlInputLine->setPlaceholderText("Вставьте ссылку на изображение");

    auto *addUrlLayout = new QHBoxLayout;
    addUrlBtn = new QPushButton("Добавить");
    delUrlBtn = new QPushButton("Удалить");
    addUrlBtn->setMinimumSize(100, 35);
    delUrlBtn->setMinimumSize(100, 35);
    delUrlBtn->setEnabled(isEnabledDelUrlBtn());

    addUrlLayout->addWidget(urlInputLine);
    addUrlLayout->addWidget(addUrlBtn);
    addUrlLayout->addWidget(delUrlBtn);


    downloadBtn = new QPushButton("Скачать");

    mainLayout->addLayout(addUrlLayout);
    mainLayout->addWidget(new QLabel("Добавленные ссылки:"));
    mainLayout->addWidget(urlList);
    mainLayout->addWidget(downloadBtn);
    mainLayout->addWidget(progressTable);
    setCentralWidget(central);

    connect(addUrlBtn, &QPushButton::clicked, this, &MainWindow::onAddUrlBtnClicked);
    connect(delUrlBtn, &QPushButton::clicked, this, &MainWindow::onDelUrlBtnClicked);
    connect(urlInputLine, &QLineEdit::textChanged, this, &MainWindow::checkAddUrlBtn);
    connect(urlList, &QListWidget::currentItemChanged, this, [this](QListWidgetItem *current, QListWidgetItem* prev)
    {
        this->delUrlBtn->setEnabled(isEnabledDelUrlBtn());
    });

    connect(progressTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onProgressTableDoubleClicked);

    connect(downloadBtn, &QPushButton::clicked, this, &MainWindow::onDownloadBtnClicked);

    connect(dManager, &DownloadManager::downloadProgress, this, &MainWindow::onUpdateProgress);
    connect(dManager, &DownloadManager::downloadEnd, this, &MainWindow::onDownloadFinished);

    //Обработка ошибок
    connect(dManager, &DownloadManager::downloadError, this, [this] (const QUrl &url, const QString &error)
    {
        qDebug() << "Ошибка при скачивании изображения" << error << "по ссылке" << url;
        int row = hashUrl2Row[url];
        QLabel *label = qobject_cast<QLabel*>(progressTable->cellWidget(row, 2));
        label->setText("Ошибка загрузки: " + error);
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

void MainWindow::onDelUrlBtnClicked()
{
    QListWidgetItem* selectedItem = urlList->currentItem();

    if (selectedItem)
    {
        QListWidgetItem *item = urlList->takeItem(urlList->currentRow());
        qDebug () << "Удален элемент из очереди на скачивание" << item->data(32);
        delete item;
    }
    delUrlBtn->setEnabled(isEnabledDelUrlBtn());
}

void MainWindow::checkAddUrlBtn(QString text)
{
    addUrlBtn->setEnabled(!text.isEmpty());
}

bool MainWindow::isEnabledDelUrlBtn()
{
    return (urlList->count() != 0 && urlList->currentItem());
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
        urlItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        progressTable->setItem(row, 0, urlItem);
        urlItem->setData(Qt::UserRole, url);

        //ячейка 1 - виджет для прогресс бара
        auto *widget = new QWidget;
        auto *hLayout = new QHBoxLayout(widget);
        hLayout->setContentsMargins(8, 2, 8, 2);

        auto *newBar = new QProgressBar;
        newBar->setMaximum(100);
        newBar->setMinimum(0);
        newBar->setValue(0);
        newBar->setTextVisible(true);
        newBar->setFormat("%p%");
        newBar->setMinimumWidth(250);
        newBar->setAlignment(Qt::AlignCenter);
        hLayout->addWidget(newBar);

        urlItem->setData(Qt::UserRole + 1, QVariant::fromValue(newBar));

        //ячейка 2 - виджет для состояния
        QLabel *newLabel = new QLabel("Скачивание изображения");
        newLabel->setAlignment(Qt::AlignCenter);
        progressTable->setCellWidget(row, 2, newLabel);
        hashUrl2Row[url] = row;

        progressTable->setCellWidget(row, 1, widget);
        delete item;
        //запуск загрузки
        dManager->addImgToDownload(url);
    }
    downloadBtn->setEnabled(true);
}

void MainWindow::onUpscaleFinished(const QUrl &url, const QString &soureImgPath, const QString &scaledImgPath)
{
    if (!hashUrl2Row.contains(url))
    {
        qWarning() << "Не найден url в таблице:" << url;
        return;
    }
    int row = hashUrl2Row[url];
    QLabel *label = qobject_cast<QLabel*>(progressTable->cellWidget(row, 2));
    label->setText("Изображение улучшено");

    this->hashUrl2SourceImgPath[url] = soureImgPath;
    this->hashUrl2ScaledImgPath[url] = scaledImgPath;
}

void MainWindow::onProgressTableDoubleClicked(int row, int col)
{
    Q_UNUSED(col);

    QTableWidgetItem *item = progressTable->item(row, 0);
    if (!item) return;

    const QUrl url = item->data(Qt::UserRole).toUrl();

    if ( !hashUrl2SourceImgPath.contains(url) || !hashUrl2ScaledImgPath.contains(url))
    {
        QMessageBox::information(this, "Информация", "Изображения ещё не готовы.");
        return;
    }

    QString sourcePath = hashUrl2SourceImgPath[url];
    QString scaledPath = hashUrl2ScaledImgPath[url];

    CompareWindow *compare = new CompareWindow;
    compare->setImages(QImage(sourcePath), QImage(scaledPath));
    compare->show();
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
    if (!hashUrl2Row.contains(url))
    {
        qWarning() << "Не найден url в таблице:" << url;
        return;
    }

    int row = hashUrl2Row[url];
    QLabel *label = qobject_cast<QLabel*>(progressTable->cellWidget(row, 2));
    label->setText("Улучшение изображения");

    // запускаем апскейл
    auto *scale = new ScaleImage(url, data);
    QThreadPool::globalInstance()->start(scale);


    connect(scale, &ScaleImage::upscaleFinished, this, &MainWindow::onUpscaleFinished);
    connect(scale, &ScaleImage::scaleError, this, [this] (const QUrl &url, const QString &error)
    {
        int row = this->hashUrl2Row[url];
        QLabel *label = qobject_cast<QLabel*>(this->progressTable->cellWidget(row, 2));
        label->setText("Ошибка улучшения изображения: " + error);

    });

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
