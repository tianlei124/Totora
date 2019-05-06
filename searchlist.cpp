#include <QDebug>
#include "searchlist.hpp"

searchlist::searchlist(QLineEdit *parent) : QObject(parent), editor(parent)
{
    searchEngineItem = new QTreeWidget;

    searchEngineItem->setWindowFlags(editor->windowFlags());
    searchEngineItem->setFocusPolicy(Qt::NoFocus);
    searchEngineItem->setFocusProxy(parent);
    searchEngineItem->setMouseTracking(true);
    searchEngineItem->headerItem()->setHidden(true);

    searchEngineItem->setColumnCount(1);
    searchEngineItem->setUniformRowHeights(true);
    searchEngineItem->setRootIsDecorated(false);
    searchEngineItem->setEditTriggers(QTreeWidget::NoEditTriggers);
    searchEngineItem->setSelectionBehavior(QTreeWidget::SelectRows);
    searchEngineItem->setFrameStyle(QFrame::Box | QFrame::Plain);
    searchEngineItem->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    searchEngineItem->header()->hide();

    searchEngines << "bing";
    searchEngines << "baidu";
    searchEngines << "gooogle";

    QPoint mouse_pos = QCursor::pos();
    qDebug() << "mouse position in searchlist" << mouse_pos;
    qDebug() << "editor width " << editor->width() << ", height " << editor->height();
    searchEngineItem->setGeometry(mouse_pos.x(), mouse_pos.y() + editor->height(), editor->width(), editor->height()*searchEngines.count());

    for (int i = 0;i < searchEngines.count();++i)
    {
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem(searchEngineItem);
        item->setText(0, searchEngines[i]);
    }
    searchEngineItem->setCurrentItem(searchEngineItem->topLevelItem(0));
    searchEngineItem->resizeColumnToContents(0);
    searchEngineItem->show();
}

searchlist::~searchlist()
{
    delete searchEngineItem;
}

void searchlist::followMove()
{
    searchEngineItem->move(editor->mapToGlobal(QPoint(0, editor->height())));
}

void searchlist::followMinimized()
{
    searchEngineItem->showMinimized();
}

void searchlist::followRestore()
{
    searchEngineItem->showNormal();
}

void searchlist::followClosed()
{
    searchEngineItem->close();
}
