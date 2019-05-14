#include "searchwindow.h"
#include "ui_searchwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDesktopWidget>

#include <QKeyEvent>
#include <QMenu>

#include <QDebug>

searchWindow::searchWindow(QWidget *parent) :
    QMainWindow(parent, Qt::Dialog),
    ui(new Ui::searchWindow)
{
    ui->setupUi(this);
    ui->statusbar->hide();
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    ui->searchEngineList->setFocusPolicy(Qt::NoFocus);
    ui->searchEngineList->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->searchBox->setFocusPolicy(Qt::NoFocus);

    ui->searchEngineList->installEventFilter(this);

    connect(this->ui->searchBox, SIGNAL(returnPressed()), this, SLOT(selectSearchEngine()));

    localSearchWindow = new LocalSearchWindow(this);
    connect(this, SIGNAL(triggerLocalSearch(const QString&)), localSearchWindow, SLOT(getContent(const QString&)));

    customSearchs = new CustomSearchs(parent);
    const std::vector<std::pair<searchEngine, bool> >& searchEngineItems = customSearchs->getSearchEngineItems();
    for (size_t i = 0;i < searchEngineItems.size();++i)
    {
        if (!searchEngineItems[i].second)
            continue;
        QString hotKey(searchEngineItems[i].first.hotKey[0]);
        for (size_t j = 1;j < searchEngineItems[i].first.hotKey.size();++j)
        {
            if (searchEngineItems[i].first.hotKey[j] >= 'A' && searchEngineItems[i].first.hotKey[j] <= 'Z')
                hotKey += searchEngineItems[i].first.hotKey[j];
        }
        int totalWhiteSpaceWidth = ui->searchEngineList->sizeHint().width() -
                ui->searchEngineList->fontMetrics().width(searchEngineItems[i].first.title) -
                ui->searchEngineList->fontMetrics().width(hotKey);
        int whiteSpaceWidth = ui->searchEngineList->fontMetrics().width(' ');

        QString itemTitle = searchEngineItems[i].first.title;
        for (int j = 0;j < totalWhiteSpaceWidth/whiteSpaceWidth;++j)
        {
            itemTitle += ' ';
        }
        itemTitle += hotKey;
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(60, 36));
        pItem->setText(itemTitle);

        ui->searchEngineList->addItem(pItem);
    }
    connect(customSearchs, SIGNAL(updateSearchEngines()), this, SLOT(addSearchEngines()));

    QPoint mouse_pos = QCursor::pos();
    QPoint window_pos = mouse_pos;
    if (QApplication::desktop()->height() - mouse_pos.y() < height())
        window_pos.setY(QApplication::desktop()->height() - height());
    else if (QApplication::desktop()->width() - mouse_pos.x() < width())
        window_pos.setX(QApplication::desktop()->width() - width()); move(window_pos);

    createActions();
    createTrayIcon();
    trayIcon->show();

    showSearchWindow = new QHotkey(QKeySequence("ctrl+alt+Q"), true);
    qDebug() << "Is Registered: " << showSearchWindow->isRegistered();
    connect(showSearchWindow, &QHotkey::activated, this, [this](){
        this->show();
        ui->searchBox->setFocus();
    });

    ui->searchBox->setFocus();
}

searchWindow::~searchWindow()
{
    delete ui;

    delete customSearchs;

    delete showPreferenceAction;
    delete showSearchBoxAction;
    delete showCustomSearchEnginesAction;
    delete quitAction;

    delete trayIcon;
    delete trayIconMenu;
}

bool searchWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj != ui->searchEngineList)
    {
        return false;
    }

    if (ev->type() == QEvent::MouseButtonPress)
    {
        return true;
    }
    if (ev->type() == QEvent::KeyPress)
    {
        int key = static_cast<QKeyEvent*>(ev)->key();
        if (key >= Qt::Key_A && key <= Qt::Key_Z)
        {
            if (selectedEngines.empty())
            {
                const std::vector<std::pair<searchEngine, bool> >& searchEngineItems = customSearchs->getSearchEngineItems();
                for (size_t i = 0;i < searchEngineItems.size();++i)
                {
                    if (!searchEngineItems[i].second)
                        continue;
                    QString tmp;
                    for (size_t j = 0;j < searchEngineItems[i].first.hotKey.size();++j)
                    {
                        if (searchEngineItems[i].first.hotKey[j] >= 'A' && searchEngineItems[i].first.hotKey[j] <= 'Z')
                            tmp.push_back(searchEngineItems[i].first.hotKey[j]);
                    }
                    selectedEngines[searchEngineItems[i].first.title] = std::make_pair(0, tmp);
                }
            }

            std::vector<QString> selected;
            bool isAnyMatched = false;
            for (auto it = selectedEngines.begin();it != selectedEngines.end();++it)
            {
                std::pair<int, QString>& IdhotKey = it->second;
                if ((IdhotKey.first < IdhotKey.second.size() &&
                     (key - Qt::Key_A + 'A') == IdhotKey.second[IdhotKey.first]) ||
                    (IdhotKey.first == IdhotKey.second.size() &&
                     (key - Qt::Key_A + 'A') == IdhotKey.second[IdhotKey.first - 1]))
                {
                    IdhotKey.first += 1;
                    isAnyMatched = true;
                }
                else if ((IdhotKey.first < IdhotKey.second.size() &&
                         (key - Qt::Key_A + 'A') != IdhotKey.second[IdhotKey.first]) ||
                         IdhotKey.first >= IdhotKey.second.size())
                {
                    selected.push_back(it->first);
                }
            }
            if (isAnyMatched)
            {
                for (size_t i = 0;i < selected.size();++i)
                {
                    selectedEngines.erase(selected[i]);
                }

                ui->searchEngineList->clear();
                for (auto it = selectedEngines.begin();it != selectedEngines.end();++it)
                {
                    std::pair<int, QString>& IdhotKey = it->second;
                    int totalWhiteSpaceWidth = ui->searchEngineList->sizeHint().width() -
                            ui->searchEngineList->fontMetrics().width(it->first) -
                            ui->searchEngineList->fontMetrics().width(IdhotKey.second);
                    int whiteSpaceWidth = ui->searchEngineList->fontMetrics().width(' ');

                    QString itemTitle = it->first;
                    for (int j = 0;j < totalWhiteSpaceWidth/whiteSpaceWidth + 1;++j)
                    {
                        itemTitle += ' ';
                    }
                    itemTitle += IdhotKey.second;

                    QListWidgetItem *pItem = new QListWidgetItem;
                    pItem->setSizeHint(QSize(60, 36));
                    pItem->setText(itemTitle);

                    ui->searchEngineList->addItem(pItem);
                }
                ui->searchEngineList->setCurrentRow(0);

                if (selectedEngines.size() == 1)
                {
                    doSearch(0);
                    resetSearchStatus();
                }
            }
            return false;
        }
        else if (key == Qt::Key_Enter || key == Qt::Key_Return)
        {
            int id = ui->searchEngineList->currentRow();

            doSearch(id);
            resetSearchStatus();

            return false;
        }
    }
    return false;
}

void searchWindow::keyPressEvent(QKeyEvent *k)
{
    if (k->key() == Qt::Key_Escape)
    {
        hide();
    }
    else
    {
        QWidget::keyPressEvent(k);
    }
}

void searchWindow::selectSearchEngine()
{
    ui->searchEngineList->setFocus();
    if (ui->searchEngineList->count() < 0 || ui->searchBox->text().count() < 0)
    {
        return ;
    }
    ui->searchEngineList->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui->searchEngineList->setCurrentRow(0);
}

void searchWindow::addSearchEngines()
{
    const std::vector<std::pair<searchEngine, bool> >& searchEngineItems = customSearchs->getSearchEngineItems();
    ui->searchEngineList->clear();

    for (size_t i = 0;i < searchEngineItems.size();++i)
    {
        if (!searchEngineItems[i].second)
            continue;
        QString hotKey(searchEngineItems[i].first.hotKey[0]);
        for (size_t j = 1;j < searchEngineItems[i].first.hotKey.size();++j)
        {
            if (searchEngineItems[i].first.hotKey[j] >= 'A' && searchEngineItems[i].first.hotKey[j] <= 'Z')
                hotKey += searchEngineItems[i].first.hotKey[j];
        }
        int totalWhiteSpaceWidth = ui->searchEngineList->sizeHint().width() -
                ui->searchEngineList->fontMetrics().width(searchEngineItems[i].first.title) -
                ui->searchEngineList->fontMetrics().width(hotKey);
        int whiteSpaceWidth = ui->searchEngineList->fontMetrics().width(' ');

        QString itemTitle = searchEngineItems[i].first.title;
        for (int j = 0;j < totalWhiteSpaceWidth/whiteSpaceWidth;++j)
        {
            itemTitle += ' ';
        }
        itemTitle += hotKey;

        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(60, 36));
        pItem->setText(itemTitle);

        ui->searchEngineList->addItem(pItem);
    }
}

void searchWindow::createActions()
{
    showPreferenceAction = new QAction(QString("Show Preference"), this);
    // connect(showPreferenceAction, &QAction::triggered, this, )
    showSearchBoxAction = new QAction(QString("Show Search Window"), this);
    connect(showSearchBoxAction, &QAction::triggered, this, [this](){
        this->show();
        ui->searchBox->setFocus();
    });
    showCustomSearchEnginesAction = new QAction(QString("Show Custom Search Eigines"), this);
    connect(showCustomSearchEnginesAction, &QAction::triggered, customSearchs, &CustomSearchs::show);
    quitAction = new QAction(QString("Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void searchWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(showPreferenceAction);
    trayIconMenu->addAction(showSearchBoxAction);
    trayIconMenu->addAction(showCustomSearchEnginesAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/totoro.png"));
    trayIcon->setContextMenu(trayIconMenu);
}

void searchWindow::doSearch(int id)
{
    QString keyWord = ui->searchBox->text();
    QString URL = "https://www.baidu.com/s?wd=@totora";
    if (selectedEngines.empty())
    {
        const std::vector<std::pair<searchEngine, bool> >& searchEngineItems = customSearchs->getSearchEngineItems();
        if (searchEngineItems[id].first.title == "Local Search")
        {
            emit triggerLocalSearch(keyWord);
            return ;
        }
        URL = searchEngineItems[id].first.url;
    }
    else
    {
        const QString title = ui->searchEngineList->item(id)->text();
        for (auto it = selectedEngines.begin();it != selectedEngines.end();++it)
        {
            if (title.contains(it->first))
            {
                if (it->first == "Local Search")
                {
                    emit triggerLocalSearch(keyWord);
                    return ;
                }
                URL = customSearchs->getSearchEngineURL(it->first);
                break;
            }
        }
    }

    QUrl url = URL.replace("@totora", keyWord);
    qDebug() << "selected engies " << url;

    QDesktopServices::openUrl(QUrl(url));
}

void searchWindow::resetSearchStatus()
{
    ui->searchBox->setText("");
    ui->searchEngineList->clear();
    const std::vector<std::pair<searchEngine, bool> >& searchEngineItems = customSearchs->getSearchEngineItems();
    for (size_t i = 0;i < searchEngineItems.size();++i)
    {
        if (!searchEngineItems[i].second)
            continue;
        QString hotKey(searchEngineItems[i].first.hotKey[0]);
        for (size_t j = 1;j < searchEngineItems[i].first.hotKey.size();++j)
        {
            if (searchEngineItems[i].first.hotKey[j] >= 'A' && searchEngineItems[i].first.hotKey[j] <= 'Z')
                hotKey += searchEngineItems[i].first.hotKey[j];
        }
        int totalWhiteSpaceWidth = ui->searchEngineList->sizeHint().width() -
                ui->searchEngineList->fontMetrics().width(searchEngineItems[i].first.title) -
                ui->searchEngineList->fontMetrics().width(hotKey);
        int whiteSpaceWidth = ui->searchEngineList->fontMetrics().width(' ');

        QString itemTitle = searchEngineItems[i].first.title;
        for (int j = 0;j < totalWhiteSpaceWidth/whiteSpaceWidth;++j)
        {
            itemTitle += ' ';
        }
        itemTitle += hotKey;

        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(60, 36));
        pItem->setText(itemTitle);

        ui->searchEngineList->addItem(pItem);
    }

    ui->searchEngineList->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    this->hide();
    selectedEngines.clear();
}

void searchWindow::on_searchEngineList_doubleClicked(const QModelIndex &index)
{
    doSearch(index.row());
    resetSearchStatus();
}
