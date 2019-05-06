#include <QDesktopServices>
#include <QUrl>

#include <QCoreApplication>

#include <QDebug>

#include "searchbox.hpp"
#include "searchlist.hpp"

#define GSEARCH_URL "http://www.google.com/search?q=%1"


SearchBox::SearchBox(QWidget *parent): QLineEdit(parent)
{
    setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::SubWindow);

    connect(this, SIGNAL(returnPressed()),this, SLOT(doSearch()));

    adjustSize();
    resize(QApplication::desktop()->width() / 3 * 0.618, 30);
    QPoint mouse_pos = QCursor::pos();
    QPoint start_point = mouse_pos;
    if (QApplication::desktop()->height() - mouse_pos.y() < height()*5)
        start_point.setY(mouse_pos.y() - height()*5);
    else if (QApplication::desktop()->width() - mouse_pos.x() < width())
        start_point.setX(mouse_pos.x() - width());
    setGeometry(start_point.x(), start_point.y(), width(), height());

    searchEntry = new searchlist(this);

    createActions();
    createTrayIcon();
    trayIcon->show();

    setFocus();
}

SearchBox::~SearchBox()
{
    delete searchEntry;

    delete showPreferenceAction;
    delete showSearchBoxAction;
    delete showCustomSearchEnginesAction;
    delete quitAction;
    delete trayIconMenu;
    delete trayIcon;
}

void SearchBox::moveEvent(QMoveEvent *event)
{
    searchEntry->followMove();
    event->accept();
}

void SearchBox::closeEvent(QCloseEvent *event)
{
    qDebug() << "window closed\n";
    searchEntry->followClosed();
    event->accept();
}

void SearchBox::changeEvent(QEvent *event)
{
    if (event->type() != QEvent::WindowStateChange)
    {
        return ;
    }

    if (this->windowState() == Qt::WindowMinimized)
    {
        qDebug() << "window minimized\n";
        searchEntry->followMinimized();
    }
}

void SearchBox::keyPressEvent(QKeyEvent *k)
{
    if (k->key() == Qt::Key_Escape)
    {
        showMinimized();
    }
}

void SearchBox::doSearch()
{
    QString url = "http://www.baidu.com";
    QDesktopServices::openUrl(QUrl(url));
}

void SearchBox::bothShowNormal()
{
    showNormal();
    searchEntry->followRestore();
}

void SearchBox::createActions()
{
    showPreferenceAction = new QAction(QString("Show Preference"), this);
    //    connect(showPreferenceAction, &QAction::triggered, this, )
    showSearchBoxAction = new QAction(QString("Show Search Window"), this);
    connect(showSearchBoxAction, &QAction::triggered, this, bothShowNormal);
    showCustomSearchEnginesAction = new QAction(QString("Show Custom Search Eigines"), this);
  //  connect(showCustomSearchEnginesAction, &QAction::triggered, this, );
    quitAction = new QAction(QString("Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void SearchBox::createTrayIcon()
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
